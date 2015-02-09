/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2015 The Qt Company Ltd and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgraphicscontext_p.h"

#include <Qt3DRenderer/qopenglfilter.h>
#include <Qt3DRenderer/qparameter.h>
#include <Qt3DRenderer/private/renderlogging_p.h>
#include <Qt3DRenderer/private/rendershader_p.h>
#include <Qt3DRenderer/private/rendermaterial_p.h>
#include <Qt3DRenderer/private/rendertexture_p.h>
#include <Qt3DRenderer/private/rendercommand_p.h>
#include <Qt3DRenderer/private/renderstate_p.h>
#include <Qt3DRenderer/private/rendertarget_p.h>
#include <Qt3DRenderer/private/qgraphicshelperinterface_p.h>
#include <Qt3DRenderer/private/renderer_p.h>
#include <Qt3DRenderer/private/managers_p.h>
#include <Qt3DRenderer/private/attachmentpack_p.h>
#include <QOpenGLShaderProgram>

#if !defined(QT_OPENGL_ES_2)
#include <QOpenGLFunctions_2_0>
#include <QOpenGLFunctions_3_2_Core>
#include <Qt3DRenderer/private/qgraphicshelpergl2_p.h>
#include <Qt3DRenderer/private/qgraphicshelpergl3_p.h>
#endif
#include <Qt3DRenderer/private/qgraphicshelperes2_p.h>

#include <QSurface>
#include <QOpenGLTexture>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

static QHash<unsigned int, QGraphicsContext*> static_contexts;

unsigned int nextFreeContextId()
{
    for (unsigned int i=0; i < 0xffff; ++i) {
        if (!static_contexts.contains(i))
            return i;
    }

    qFatal("Couldn't find free context ID");
    return 0;
}

QGraphicsContext::QGraphicsContext()
    : m_initialized(false)
    , m_id(nextFreeContextId())
    , m_gl(Q_NULLPTR)
    , m_surface(Q_NULLPTR)
    , m_glHelper(Q_NULLPTR)
    , m_activeShader(Q_NULLPTR)
    , m_material(Q_NULLPTR)
    , m_stateSet(Q_NULLPTR)
    , m_renderer(Q_NULLPTR)
    , m_contextInfo(new QOpenGLFilter())
    , m_uboTempArray(QByteArray(1024, 0))
    , m_supportsVAO(true)
{
    static_contexts[m_id] = this;
}

QGraphicsContext::~QGraphicsContext()
{
    releaseOpenGL();

    Q_ASSERT(static_contexts[m_id] == this);
    static_contexts.remove(m_id);
}

void QGraphicsContext::initialize()
{
    m_initialized = true;

    Q_ASSERT(m_gl);

    GLint numTexUnits;
    m_gl->functions()->glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &numTexUnits);
    qCDebug(Backend) << "context supports" << numTexUnits << "texture units";

    m_pinnedTextureUnits = QBitArray(numTexUnits);
    m_activeTextures.resize(numTexUnits);
    m_textureScopes.resize(numTexUnits);

    if (m_gl->format().majorVersion() >= 3) {
        m_supportsVAO = true;
    } else {
        QSet<QByteArray> extensions = m_gl->extensions();
        m_supportsVAO = extensions.contains(QByteArrayLiteral("GL_OES_vertex_array_object"))
                || extensions.contains(QByteArrayLiteral("GL_ARB_vertex_array_object"))
                || extensions.contains(QByteArrayLiteral("GL_APPLE_vertex_array_object"));
    }
    qCDebug(Backend) << "VAO support = " << m_supportsVAO;
}

bool QGraphicsContext::beginDrawing(QSurface *surface, const QColor &clearColor)
{
    Q_ASSERT(surface);
    Q_ASSERT(m_gl);

    m_surface = surface;

    bool ok = m_gl->makeCurrent(m_surface);
    if (!ok) {
        qCWarning(Backend) << Q_FUNC_INFO << "make current failed";
        return false;
    }

    GLint err = m_gl->functions()->glGetError();
    if (err != 0) {
        qCWarning(Backend) << Q_FUNC_INFO << "glGetError:" << err;
    }

    if (!m_initialized) {
        initialize();
    }

    m_gl->functions()->glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), clearColor.alphaF());

    if (m_activeShader)
        m_activeShader = NULL;

    return true;
}

void QGraphicsContext::clearBackBuffer(QClearBuffer::BufferType buffers)
{
    if (buffers != QClearBuffer::None) {
        GLbitfield mask = 0;

        if (buffers & QClearBuffer::ColorBuffer)
            mask |= GL_COLOR_BUFFER_BIT;
        if (buffers & QClearBuffer::DepthBuffer)
            mask |= GL_DEPTH_BUFFER_BIT;
        if (buffers & QClearBuffer::StencilBuffer)
            mask |= GL_STENCIL_BUFFER_BIT;

        m_gl->functions()->glClear(mask);
    }
}

void QGraphicsContext::endDrawing(bool swapBuffers)
{
    if (swapBuffers)
        m_gl->swapBuffers(m_surface);
    m_gl->doneCurrent();
    m_stateSet = Q_NULLPTR;
    decayTextureScores();
}

void QGraphicsContext::setViewport(const QRectF &viewport)
{
    m_viewport = viewport;
    QSize renderTargetSize;
    if (m_activeFBO != 0) {
        // For external FBOs we may not have an m_renderTargets entry. Do not call glViewport in that case.
        if (m_renderTargetsSize.contains(m_activeFBO))
            renderTargetSize = m_renderTargetsSize[m_activeFBO];
        else
            return;
    } else {
        renderTargetSize = m_surface->size();
    }

    // Qt3D 0------------------> 1  OpenGL  1^
    //      |                                |
    //      |                                |
    //      |                                |
    //      V                                |
    //      1                                0---------------------> 1
    // The Viewport is defined between 0 and 1 which allows us to automatically
    // scale to the size of the provided window surface
    m_gl->functions()->glViewport(m_viewport.x() * renderTargetSize.width(),
                                  (1.0 - m_viewport.y() - m_viewport.height()) * renderTargetSize.height(),
                                  m_viewport.width() * renderTargetSize.width(),
                                  m_viewport.height() * renderTargetSize.height());
}

void QGraphicsContext::releaseOpenGL()
{
    // m_shaderHash.clear
    m_bufferHash.clear();
}

void QGraphicsContext::setOpenGLContext(QOpenGLContext* ctx, QSurface *surface)
{
    Q_ASSERT(surface);
    Q_ASSERT(ctx);

    releaseOpenGL();
    m_gl = ctx;

    m_gl->makeCurrent(surface);
    resolveHighestOpenGLFunctions();
    m_gl->doneCurrent();
}

// That assumes that the shaderProgram in RenderShader stays the same
void QGraphicsContext::activateShader(RenderShader *shader)
{
    if (shader == Q_NULLPTR) {
        m_activeShader = Q_NULLPTR;
        m_material = Q_NULLPTR;
        m_glHelper->useProgram(0);
        return;
    }

    // If RenderShader has no QOpenGLShaderProgram or !shader->isLoaded (shader sources have changed)
    if (!m_shaderHash.contains(shader->dna()) || !shader->isLoaded()) {
        QOpenGLShaderProgram *prog = shader->getOrCreateProgram(this);
        Q_ASSERT(prog);
        m_shaderHash.insert(shader->dna(), prog);
        qCDebug(Backend) << Q_FUNC_INFO << "shader count =" << m_shaderHash.count();
        shader->initializeUniforms(m_glHelper->programUniformsAndLocations(prog->programId()));
        shader->initializeAttributes(m_glHelper->programAttributesAndLocations(prog->programId()));
        if (m_glHelper->supportsFeature(QGraphicsHelperInterface::UniformBufferObject))
            shader->initializeUniformBlocks(m_glHelper->programUniformBlocks(prog->programId()));
        m_activeShader = Q_NULLPTR;
    }

    if (m_activeShader != Q_NULLPTR && m_activeShader->dna() == shader->dna()) {

        // no op
    } else {
        m_activeShader = shader;
        QOpenGLShaderProgram* prog = m_shaderHash[shader->dna()];
        prog->bind();
        // ensure material uniforms are re-applied
        m_material = Q_NULLPTR;
    }
}

void QGraphicsContext::activateRenderTarget(RenderTarget *renderTarget, const AttachmentPack &attachments, GLuint defaultFboId)
{
    GLuint fboId = defaultFboId; // Default FBO
    if (renderTarget != Q_NULLPTR) {
        // New RenderTarget
        if (!m_renderTargets.contains(renderTarget->peerUuid())) {
            // The FBO is created and its attachments are set once
            if ((fboId = m_glHelper->createFrameBufferObject()) != 0) {
                // Insert FBO into hash
                m_renderTargets.insert(renderTarget->peerUuid(), fboId);
                // Bind FBO
                m_glHelper->bindFrameBufferObject(fboId);
                bindFrameBufferAttachmentHelper(fboId, attachments);
            } else {
                qCritical() << "Failed to create FBO";
            }
        } else {
            fboId = m_renderTargets.value(renderTarget->peerUuid());

            // We need to check if  one of the attachment was resized
            bool needsResize = false;
            Q_FOREACH (const Attachment &attachment, attachments.attachments()) {
                RenderTexture *rTex = m_renderer->textureManager()->lookupResource(attachment.m_textureUuid);
                if (rTex != Q_NULLPTR)
                    needsResize |= rTex->isTextureReset();
            }
            if (needsResize) {
                m_glHelper->bindFrameBufferObject(fboId);
                bindFrameBufferAttachmentHelper(fboId, attachments);
            }
        }
    }
    m_activeFBO = fboId;
    m_glHelper->bindFrameBufferObject(m_activeFBO);
}

void QGraphicsContext::bindFrameBufferAttachmentHelper(GLuint fboId, const AttachmentPack &attachments)
{
    // Set FBO attachments
    int drawBuffers[QRenderAttachment::ColorAttachment15 + 1];
    int i = 0;
    QSize fboSize;
    Q_FOREACH (const Attachment &attachment, attachments.attachments()) {
        RenderTexture *rTex = m_renderer->textureManager()->lookupResource(attachment.m_textureUuid);
        if (rTex != Q_NULLPTR) {
            QOpenGLTexture *glTex = rTex->getOrCreateGLTexture();
            if (glTex != Q_NULLPTR) {
                if (fboSize.isEmpty())
                    fboSize = QSize(glTex->width(), glTex->height());
                else
                    fboSize = QSize(qMin(fboSize.width(), glTex->width()), qMin(fboSize.width(), glTex->width()));
                m_glHelper->bindFrameBufferAttachment(glTex, attachment);
                if (attachment.m_type <= QRenderAttachment::ColorAttachment15)
                    drawBuffers[i++] = attachment.m_type;
            }
        }
    }
    if (m_glHelper->checkFrameBufferComplete()) {
        if (i > 1) {// We need MRT
            if (m_glHelper->supportsFeature(QGraphicsHelperInterface::MRT)) {
                // Set up MRT, glDrawBuffers...
                m_glHelper->drawBuffers(i, drawBuffers);
            }
            qDebug() << "FBO Complete";
        }
    } else {
        qWarning() << "FBO incomplete";
    }
    m_renderTargetsSize.insert(fboId, fboSize);
}


void QGraphicsContext::setActiveMaterial(RenderMaterial *rmat)
{
    if (m_material == rmat)
        return;

    deactivateTexturesWithScope(TextureScopeMaterial);
    m_material = rmat;
}

// TO DO : Try to move what's in Renderer here
void QGraphicsContext::executeCommand(const RenderCommand *)
{
}

int QGraphicsContext::activateTexture(TextureScope scope, RenderTexture *tex, int onUnit)
{
    // Returns the texture unit to use for the texture
    // This always return a valid unit, unless there are more textures than
    // texture unit available for the current material
    onUnit = assignUnitForTexture(tex);

    // check we didn't overflow the available units
    if (onUnit == -1)
        return -1;

    // actually re-bind if required, the tex->dna on the unit not being the same
    // Note: tex->dna() could be 0 if the texture has not been created yet
    if (m_activeTextures[onUnit] != tex->dna() || tex->dna() == 0) {
        QOpenGLTexture *glTex = tex->getOrCreateGLTexture();
        glTex->bind(onUnit);
        m_activeTextures[onUnit] = tex->dna();
    }

    int err = m_gl->functions()->glGetError();
    if (err)
        qCWarning(Backend) << "GL error after activating texture" << QString::number(err, 16)
                           << tex->textureId() << "on unit" << onUnit;

    m_textureScores.insert(m_activeTextures[onUnit], 200);
    m_pinnedTextureUnits[onUnit] = true;
    m_textureScopes.insert(onUnit, scope);

    return onUnit;
}

void QGraphicsContext::deactivateTexturesWithScope(TextureScope ts)
{
    for (int u=0; u<m_activeTextures.size(); ++u) {
        if (!m_pinnedTextureUnits[u])
            continue; // inactive, ignore

        if (m_textureScopes[u] == ts) {
            m_pinnedTextureUnits[u] = false;
            m_textureScores.insert(m_activeTextures[u], m_textureScores.value(m_activeTextures[u], 1) - 1);
        }
    } // of units iteration
}

/*!
 * Finds the highest supported opengl version and internally use the most optimized
 * helper for a given version.
 */
void QGraphicsContext::resolveHighestOpenGLFunctions()
{
    Q_ASSERT(m_gl);

    if (m_gl->isOpenGLES()) {
        m_glHelper = new QGraphicsHelperES2();
        m_glHelper->initializeHelper(m_gl, Q_NULLPTR);
        qCDebug(Backend) << Q_FUNC_INFO << " Building OpenGL 2/ES2 Helper";
    }
#ifndef QT_OPENGL_ES_2
    else {
        QAbstractOpenGLFunctions *glFunctions = Q_NULLPTR;
        if ((glFunctions = m_gl->versionFunctions<QOpenGLFunctions_3_2_Core>()) != Q_NULLPTR) {
            qCDebug(Backend) << Q_FUNC_INFO << " Building OpenGL 3.2";
            m_glHelper = new QGraphicsHelperGL3();
        }
        else if ((glFunctions = m_gl->versionFunctions<QOpenGLFunctions_2_0>()) != Q_NULLPTR) {
            qCDebug(Backend) << Q_FUNC_INFO << " Building OpenGL 2 Helper";
            m_glHelper = new QGraphicsHelperGL2();
        }
        Q_ASSERT_X(m_glHelper, "QGraphicsContext::resolveHighestOpenGLFunctions", "unable to create valid helper for available OpenGL version");
        m_glHelper->initializeHelper(m_gl, glFunctions);
    }
#endif

    // Set Vendor and Extensions of reference OpenGLFilter
    QStringList extensions;
    Q_FOREACH (const QByteArray &ext, m_gl->extensions().values())
        extensions << QString::fromUtf8(ext);
    m_contextInfo->setMajorVersion(m_gl->format().version().first);
    m_contextInfo->setMinorVersion(m_gl->format().version().second);
    m_contextInfo->setApi(m_gl->isOpenGLES() ? QOpenGLFilter::ES : QOpenGLFilter::Desktop);
    m_contextInfo->setProfile(static_cast<QOpenGLFilter::Profile>(m_gl->format().profile()));
    m_contextInfo->setExtensions(extensions);
    m_contextInfo->setVendor(QString::fromUtf8(reinterpret_cast<const char *>(m_gl->functions()->glGetString(GL_VENDOR))));
}

void QGraphicsContext::deactivateTexture(RenderTexture* tex)
{
    for (int u=0; u<m_activeTextures.size(); ++u) {
        if (m_activeTextures[u] == tex->dna()) {
            Q_ASSERT(m_pinnedTextureUnits[u]);
            m_pinnedTextureUnits[u] = false;
            return;
        }
    } // of units iteration

    qCWarning(Backend) << Q_FUNC_INFO << "texture not active:" << tex;
}

void QGraphicsContext::setCurrentStateSet(RenderStateSet *ss)
{
    if (ss == m_stateSet)
        return;

    ss->apply(this);
    m_stateSet = ss;
}

RenderStateSet *QGraphicsContext::currentStateSet() const
{
    return m_stateSet;
}

QOpenGLFilter *QGraphicsContext::contextInfo() const
{
    return m_contextInfo;
}

/*!
 * Wraps an OpenGL call to glDrawElementsInstanced.
 * If the call is not supported by the system's OpenGL version,
 * it is simulated with a loop.
 */
void QGraphicsContext::drawElementsInstanced(GLenum primitiveType,
                                             GLsizei primitiveCount,
                                             GLint indexType,
                                             void *indices,
                                             GLsizei instances)
{
    m_glHelper->drawElementsInstanced(primitiveType,
                                      primitiveCount,
                                      indexType,
                                      indices,
                                      instances);
}

/*!
 * Wraps an OpenGL call to glDrawArraysInstanced.
 */
void QGraphicsContext::drawArraysInstanced(GLenum primitiveType,
                                           GLint first,
                                           GLsizei count,
                                           GLsizei instances)
{
    m_glHelper->drawArraysInstanced(primitiveType,
                                    first,
                                    count,
                                    instances);
}

/*!
 * Wraps an OpenGL call to glDrawElements.
 */
void QGraphicsContext::drawElements(GLenum primitiveType,
                                    GLsizei primitiveCount,
                                    GLint indexType,
                                    void *indices)
{
    m_glHelper->drawElements(primitiveType,
                             primitiveCount,
                             indexType,
                             indices);
}

/*!
 * Wraps an OpenGL call to glDrawArrays.
 */
void QGraphicsContext::drawArrays(GLenum primitiveType,
                                  GLint first,
                                  GLsizei count)
{
    m_glHelper->drawArrays(primitiveType,
                           first,
                           count);
}

void QGraphicsContext::setVerticesPerPatch(GLint verticesPerPatch)
{
    m_glHelper->setVerticesPerPatch(verticesPerPatch);
}

void QGraphicsContext::blendEquation(GLenum mode)
{
    m_glHelper->blendEquation(mode);
}

void QGraphicsContext::alphaTest(GLenum mode1, GLenum mode2)
{
    m_glHelper->alphaTest(mode1, mode2);
}

void QGraphicsContext::depthTest(GLenum mode)
{
    m_glHelper->depthTest(mode);
}

void QGraphicsContext::depthMask(GLenum mode)
{
    m_glHelper->depthMask(mode);
}

void QGraphicsContext::cullFace(GLenum mode)
{
    m_glHelper->cullFace(mode);
}

void QGraphicsContext::frontFace(GLenum mode)
{
    m_glHelper->frontFace(mode);
}

void QGraphicsContext::bindFragOutputs(GLuint shader, const QHash<QString, int> &outputs)
{
    if (m_glHelper->supportsFeature(QGraphicsHelperInterface::MRT) &&
            m_glHelper->supportsFeature(QGraphicsHelperInterface::BindableFragmentOutputs))
        m_glHelper->bindFragDataLocation(shader, outputs);
}

void QGraphicsContext::bindUniform(const QVariant &v, const ShaderUniform &description)
{
    m_glHelper->bindUniform(v, description);
}

void QGraphicsContext::bindUniformBlock(GLuint programId, GLuint uniformBlockIndex, GLuint uniformBlockBinding)
{
    m_glHelper->bindUniformBlock(programId, uniformBlockIndex, uniformBlockBinding);
}

void QGraphicsContext::bindBufferBase(GLenum target, GLuint bindingIndex, GLuint buffer)
{
    m_glHelper->bindBufferBase(target, bindingIndex, buffer);
}

void QGraphicsContext::buildUniformBuffer(const QVariant &v, const ShaderUniform &description, QByteArray &buffer)
{
    m_glHelper->buildUniformBuffer(v, description, buffer);
}

void QGraphicsContext::enableAlphaCoverage()
{
    m_glHelper->enableAlphaCoverage();
}

void QGraphicsContext::disableAlphaCoverage()
{
    m_glHelper->disableAlphaCoverage();
}

GLuint QGraphicsContext::boundFrameBufferObject()
{
    return m_glHelper->boundFrameBufferObject();
}

/*!
    \internal
    \returns a texture unit for a texture, -1 if all texture units are assigned.
    Tries to use the texture unit with the texture that hasn't been used for the longest time
    if the texture happens not to be already pinned on a texture unit.
 */
GLint QGraphicsContext::assignUnitForTexture(RenderTexture *tex)
{
    int lowestScoredUnit = -1;
    int lowestScore = 0xfffffff;

    for (int u=0; u<m_activeTextures.size(); ++u) {
        if (m_activeTextures[u] == tex->dna())
            return u;

        // No texture is currently active on the texture unit
        // we save the texture unit with the texture that has been on there
        // the longest time while not being used
        if (!m_pinnedTextureUnits[u]) {
            int score = m_textureScores.value(m_activeTextures[u], 0);
            if (score < lowestScore) {
                lowestScore = score;
                lowestScoredUnit = u;
            }
        }
    } // of units iteration

    if (lowestScoredUnit == -1)
        qCWarning(Backend) << Q_FUNC_INFO << "No free texture units!";

    return lowestScoredUnit;
}

void QGraphicsContext::decayTextureScores()
{
    QHash<uint, int>::iterator it = m_textureScores.begin();
    const QHash<uint, int>::iterator end = m_textureScores.end();

    while (it != end) {
        it.value()--;
        if (it.value() <= 0) {
            qCDebug(Backend) << "removing inactive texture" << it.key();
            it = m_textureScores.erase(it);
        } else {
            ++it;
        }
    }
}

QOpenGLShaderProgram* QGraphicsContext::activeShader()
{
    Q_ASSERT(m_activeShader);
    return m_shaderHash[m_activeShader->dna()];
}

void QGraphicsContext::setRenderer(Renderer *renderer)
{
    m_renderer = renderer;
}

// It will be easier if the QGraphicContext applies the QUniformPack
// than the other way around
void QGraphicsContext::setUniforms(QUniformPack &uniforms)
{
    // Activate textures and update TextureUniform in the pack
    // with the correct textureUnit

    // Set the pinned texture of the previous material texture
    // to pinable so that we should easily find an available texture unit
    deactivateTexturesWithScope(TextureScopeMaterial);
    // Update the uniforms with the correct texture unit id's
    QHash<QString, const QUniformValue *> &uniformValues = uniforms.uniforms();
    for (int i = 0; i < uniforms.textures().size(); ++i) {
        const QUniformPack::NamedTexture &namedTex = uniforms.textures().at(i);
        RenderTexture *t = m_renderer->textureManager()->lookupResource(namedTex.texId);
        const TextureUniform *texUniform = Q_NULLPTR;
        // TO DO : Rework the way textures are loaded
        if (t != Q_NULLPTR) {
            int texUnit = activateTexture(TextureScopeMaterial, t);
            if (uniformValues.contains(namedTex.glslName)) {
                texUniform = static_cast<const TextureUniform *>(uniformValues[namedTex.glslName]);
                if (texUniform != Q_NULLPTR)
                    const_cast<TextureUniform *>(texUniform)->setTextureUnit(texUnit);
            }
        }
    }

    // Bind UniformBlocks to UBO and update UBO from RenderShaderData
    const QVector<BlockToUBO> &blockToUbos = uniforms.uniformBuffers();
    for (int i = 0; i < blockToUbos.length(); ++i) {
        const ShaderUniformBlock &block = m_activeShader->uniformBlock(blockToUbos[i].m_blockIndex);
        if (block.m_index != -1 && block.m_size > 0) {
            UniformBuffer *ubo = m_renderer->uboManager()->lookupResource(ShaderDataShaderUboKey(blockToUbos[i].m_shaderDataID,
                                                                                                 m_activeShader->peerUuid()));
            // bind Uniform Block of index ubos[i].m_index to binding point i
            bindUniformBlock(m_shaderHash.value(m_activeShader->dna())->programId(), block.m_index, i);
            // bind the UBO to the binding point i
            // Specs specify that there are at least 14 binding points

            // Allocate ubo if not allocated previously
            if (!ubo->isCreated()) {
                ubo->create(this);
                ubo->allocate(this, block.m_size);
            }

            // update the ubo if needed
            if (blockToUbos[i].m_needsUpdate) {
                const QHash<QString, ShaderUniform> &activeUniformsInBlock = m_activeShader->activeUniformsForBlock(block.m_index);
                const QHash<QString, ShaderUniform>::const_iterator uniformsEnd = activeUniformsInBlock.end();
                QHash<QString, ShaderUniform>::const_iterator uniformsIt = activeUniformsInBlock.begin();

                while (uniformsIt != uniformsEnd) {
                    if (blockToUbos[i].m_updatedProperties.contains(uniformsIt.key())) {
                        buildUniformBuffer(blockToUbos[i].m_updatedProperties.value(uniformsIt.key()),
                                           uniformsIt.value(),
                                           m_uboTempArray);
                        ubo->update(this, m_uboTempArray.constData() + uniformsIt.value().m_offset,
                                    uniformsIt.value().m_rawByteSize,
                                    uniformsIt.value().m_offset);
                    }
                    ++uniformsIt;
                }
            }
            // bind UBO to binding point
            ubo->bindToUniformBlock(this, i);
        }
    }

    // Update uniforms in the Default Uniform Block
    m_activeShader->updateUniforms(this, uniforms);
}

void QGraphicsContext::specifyAttribute(QString nm, AttributePtr attr)
{
    QOpenGLBuffer buf = glBufferFor(attr->buffer().staticCast<Buffer>());
    buf.bind();

    QOpenGLShaderProgram* prog = activeShader();
    int location = prog->attributeLocation(nm);
    if (location < 0) {
        qCWarning(Backend) << "failed to resolve location for attribute:" << nm;
        return;
    }
    prog->enableAttributeArray(location);
    prog->setAttributeBuffer(location,
                             QGraphicsContext::elementType(attr->type()),
                             attr->byteOffset(),
                             QGraphicsContext::tupleSizeFromType(attr->type()),
                             attr->byteStride());

    if (attr->divisor() != 0) {
        // Done by the helper if it supports it
        m_glHelper->vertexAttribDivisor(location, attr->divisor());
    }

    buf.release();
}

void QGraphicsContext::specifyIndices(AttributePtr attr)
{
    if (attr->buffer().staticCast<Buffer>()->type() != QOpenGLBuffer::IndexBuffer) {
        qCWarning(Backend) << Q_FUNC_INFO << "provided buffer is not correct type";
        return;
    }

    QOpenGLBuffer buf = glBufferFor(attr->buffer().staticCast<Buffer>());
    if (!buf.bind())
        qCWarning(Backend) << Q_FUNC_INFO << "binding index buffer failed";

    // bind within the current VAO
}

QOpenGLBuffer QGraphicsContext::glBufferFor(BufferPtr buf)
{
    if (m_bufferHash.contains(buf))
        return m_bufferHash.value(buf);

    QOpenGLBuffer glbuf = buf->createGL();
    m_bufferHash[buf] = glbuf;
    buf->upload(glbuf);

    return glbuf;
}

GLint QGraphicsContext::elementType(GLint type)
{
    switch (type) {
    case GL_FLOAT:
    case GL_FLOAT_VEC2:
    case GL_FLOAT_VEC3:
    case GL_FLOAT_VEC4:
        return GL_FLOAT;

#ifndef QT_OPENGL_ES_2 // Otherwise compile error as Qt defines GL_DOUBLE as GL_FLOAT when using ES2
    case GL_DOUBLE:
#ifdef GL_DOUBLE_VEC3 // For compiling on pre GL 4.1 systems
    case GL_DOUBLE_VEC2:
    case GL_DOUBLE_VEC3:
    case GL_DOUBLE_VEC4:
#endif
        return GL_DOUBLE;
#endif
    default:
        qWarning() << Q_FUNC_INFO << "unsupported:" << QString::number(type, 16);
    }

    return GL_INVALID_VALUE;
}

GLint QGraphicsContext::tupleSizeFromType(GLint type)
{
    switch (type) {
    case GL_FLOAT:
#ifndef QT_OPENGL_ES_2 // Otherwise compile error as Qt defines GL_DOUBLE as GL_FLOAT when using ES2
    case GL_DOUBLE:
#endif
    case GL_UNSIGNED_BYTE:
    case GL_UNSIGNED_INT:
        break; // fall through

    case GL_FLOAT_VEC2:
#ifdef GL_DOUBLE_VEC2 // For compiling on pre GL 4.1 systems.
    case GL_DOUBLE_VEC2:
#endif
        return 2;

    case GL_FLOAT_VEC3:
#ifdef GL_DOUBLE_VEC3 // For compiling on pre GL 4.1 systems.
    case GL_DOUBLE_VEC3:
#endif
        return 3;

    case GL_FLOAT_VEC4:
#ifdef GL_DOUBLE_VEC4 // For compiling on pre GL 4.1 systems.
    case GL_DOUBLE_VEC4:
#endif
        return 4;

    default:
        qWarning() << Q_FUNC_INFO << "unsupported:" << QString::number(type, 16);
    }

    return 1;
}

GLuint QGraphicsContext::byteSizeFromType(GLint type)
{
    switch (type) {
    case GL_FLOAT:          return sizeof(float);
#ifndef QT_OPENGL_ES_2 // Otherwise compile error as Qt defines GL_DOUBLE as GL_FLOAT when using ES2
    case GL_DOUBLE:         return sizeof(double);
#endif
    case GL_UNSIGNED_BYTE:  return sizeof(unsigned char);
    case GL_UNSIGNED_INT:   return sizeof(GLuint);

    case GL_FLOAT_VEC2:     return sizeof(float) * 2;
    case GL_FLOAT_VEC3:     return sizeof(float) * 3;
    case GL_FLOAT_VEC4:     return sizeof(float) * 4;
#ifdef GL_DOUBLE_VEC3 // Required to compile on pre GL 4.1 systems
    case GL_DOUBLE_VEC2:    return sizeof(double) * 2;
    case GL_DOUBLE_VEC3:    return sizeof(double) * 3;
    case GL_DOUBLE_VEC4:    return sizeof(double) * 4;
#endif
    default:
        qWarning() << Q_FUNC_INFO << "unsupported:" << QString::number(type, 16);
    }

    return 0;
}

} // Render
} // Qt3D of namespace

QT_END_NAMESPACE
