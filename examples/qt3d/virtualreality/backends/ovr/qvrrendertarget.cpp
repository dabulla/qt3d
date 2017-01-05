#include "qvrrendertarget.h"
#include <QOpenGLFunctions_3_2_Core>
#include <QOpenGLFramebufferObject>
#include <QDebug>

QVrRendertarget::QVrRendertarget( ovrSession session, ovrSizei size)
    :m_session(session),
     m_texSize(size),
     m_oVrTextureChain(0)
{
    m_funcs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_2_Core>();
    if(!m_funcs)
    {
        qDebug() << "Could not get OpenGLFunctions 3.2";
        return;
    }
    ovrTextureSwapChainDesc desc = {};
    desc.Type = ovrTexture_2D;
    desc.ArraySize = 1;
    desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
    desc.Width = m_texSize.w;
    desc.Height = m_texSize.h;
    desc.MipLevels = 1;
    desc.SampleCount = 1;
    desc.StaticImage = ovrFalse;

    // We have to use the internally created texture from this method.
    // No way to use a native QOpenGLTexture for rendering, without copying it.
    ovrResult result = ovr_CreateTextureSwapChainGL(m_session, &desc, &m_oVrTextureChain);

    if(!OVR_SUCCESS(result))
    {
        ovrErrorInfo inf;
        ovr_GetLastErrorInfo(&inf);
        qDebug() << inf.ErrorString;
    }
    int length = 0;
    ovr_GetTextureSwapChainLength(m_session, m_oVrTextureChain, &length);

    if(OVR_SUCCESS(result))
    {
        for (int i = 0; i < length; ++i)
        {
            GLuint chainTexId;
            ovr_GetTextureSwapChainBufferGL(m_session, m_oVrTextureChain, i, &chainTexId);
            //m_texId = chainTexId; //TODO
            glBindTexture(GL_TEXTURE_2D, chainTexId);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
    }

    if (!m_oVrTextureChain)
    {
        ovrErrorInfo inf;
        ovr_GetLastErrorInfo(&inf);
        qDebug() << inf.ErrorString;
    }

    //Init DepthBuffer

//    glGenTextures(1, &m_texIdDepthBuffer);
//    glBindTexture(GL_TEXTURE_2D, m_texIdDepthBuffer);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

//    GLenum internalFormat = GL_DEPTH_COMPONENT24;
//    GLenum type = GL_UNSIGNED_INT;
//    //TODO: Correct Init
////    if (GLE_ARB_depth_buffer_float)
////    {
//        internalFormat = GL_DEPTH_COMPONENT32F;
//        type = GL_FLOAT;
////    }

//    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, size.w, size.h, 0, GL_DEPTH_COMPONENT, type, NULL);
////    m_qfbo = new QOpenGLFramebufferObject(m_texSize.w, m_texSize.h);
////    m_qfbo->bind();
//    m_funcs->glGenFramebuffers(1, &m_fboId);
//    m_funcs->glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);
//    int sampleCount = 1;
//    GLenum texTarget = (sampleCount > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

//    GLuint chainTexId;
//    ovr_GetTextureSwapChainBufferGL(m_session, m_oVrTextureChain, 0, &chainTexId);
//    m_funcs->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texTarget, chainTexId, 0);
//    m_funcs->glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

QVrRendertarget::~QVrRendertarget()
{
    if (m_oVrTextureChain)
    {
        ovr_DestroyTextureSwapChain(m_session, m_oVrTextureChain);
        m_oVrTextureChain = nullptr;
    }
//    if (m_texId)
//    {
//        glDeleteTextures(1, &m_texId);
//        m_texId = 0;
//    }
}

Sizei QVrRendertarget::size() const
{
    return m_texSize;
}
GLuint QVrRendertarget::texId()
{
    GLuint curTexId = 0;
    if (m_oVrTextureChain)
    {
        int curIndex;
        ovr_GetTextureSwapChainCurrentIndex(m_session, m_oVrTextureChain, &curIndex);
        ovr_GetTextureSwapChainBufferGL(m_session, m_oVrTextureChain, curIndex, &curTexId);
        qDebug() << "CurTextId: " << curTexId;
        //m_texId = curTexId;
    }
    return curTexId;
}

//void QVrRendertarget::SetAndClearRenderSurface()
//{
//    GLuint curTexId;
//    if (m_oVrTextureChain)
//    {
//        int curIndex;
//        ovr_GetTextureSwapChainCurrentIndex(m_session, m_oVrTextureChain, &curIndex);
//        ovr_GetTextureSwapChainBufferGL(m_session, m_oVrTextureChain, curIndex, &curTexId);
//        qDebug() << "CurTextId: " << curTexId << std::endl;
//        //m_texId = curTexId;
//    }

////    m_funcs->glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);
////    m_funcs->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, curTexId, 0);
////    m_funcs->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_texIdDepthBuffer, 0);

////    glViewport(0, 0, m_texSize.w, m_texSize.h);
////    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
////    glEnable(GL_FRAMEBUFFER_SRGB);
//}

//void QVrRendertarget::UnsetRenderSurface()
//{
////    m_funcs->glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);
////    m_funcs->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
////    m_funcs->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
//}

void QVrRendertarget::Commit()
{
    if (m_oVrTextureChain)
    {
        ovr_CommitTextureSwapChain(m_session, m_oVrTextureChain);
    }
}
