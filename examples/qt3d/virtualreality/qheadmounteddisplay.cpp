#include "qheadmounteddisplay.h"
#include <Qt3DQuick/QQmlAspectEngine>
#include <Qt3DRender/qcamera.h>
#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DRender/qrendersurfaceselector.h>
#include <Qt3DRender/private/qrendersurfaceselector_p.h>
#include <Qt3DInput/qinputaspect.h>
#include <Qt3DInput/qinputsettings.h>
#include <Qt3DLogic/qlogicaspect.h>

#include <QQmlContext>
#include <qqmlincubator.h>
#include <QGuiApplication>
#include <QScreen>

#include <QtGui/qopenglcontext.h>
#include <QSurface>
#include "qvirtualrealityapibackend.h"
#include <Qt3DRender/QRenderAspect>
#include <Qt3DRender/private/qrenderaspect_p.h>
#include "qvrcamera.h"

QT_BEGIN_NAMESPACE

namespace Qt3DVirtualReality {

namespace {

class Qt3DQuickVirtualRealityIncubationController : public QObject, public QQmlIncubationController
{
    Q_OBJECT
public:
    explicit Qt3DQuickVirtualRealityIncubationController(QHeadMountedDisplay *hmd)
        : QObject(hmd)
        , m_incubationTime(std::max(1, int(1000 / hmd->refreshRate()) / 3))
    {
        startTimer(hmd->refreshRate());
    }

    void timerEvent(QTimerEvent *) Q_DECL_OVERRIDE
    {
        incubateFor(m_incubationTime);
    }

private:
    const int m_incubationTime;
};

} // anonymous

QHeadMountedDisplay::QHeadMountedDisplay(int hmdId, const QHeadMountedDisplayFormat &formathmd, QVirtualRealityApi *api, QVirtualRealityApiBackend *apibackend)
    : QObject()
    , m_engine(nullptr)
    , m_renderAspect(nullptr)
    , m_inputAspect(nullptr)
    , m_logicAspect(nullptr)
    , m_initialized(false)
    , m_incubationController(nullptr)
    , m_api(api)
    , m_apibackend(apibackend)
    , m_hmdId(hmdId)
    , m_fbo(nullptr)
    , m_context(nullptr)
    , m_surface(new QOffscreenSurface)
    , m_rootItem(nullptr)
{
    //QSurface::setSurfaceType(QSurface::OpenGLSurface);

    QSurfaceFormat format;
#ifdef QT_OPENGL_ES_2
    format.setRenderableType(QSurfaceFormat::OpenGLES);
#else
    if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL) {
        format.setVersion(4, 3);
        format.setProfile(QSurfaceFormat::CoreProfile);
    }
#endif
    format.setDepthBufferSize(24);
    format.setSamples(4);
    format.setStencilBufferSize(8);
    m_surface->setFormat(format);
    QSurfaceFormat::setDefaultFormat(format);
    m_surface->create();

    m_engine.reset(new Qt3DCore::Quick::QQmlAspectEngine);
    m_renderAspect = new Qt3DRender::QRenderAspect(Qt3DRender::QRenderAspect::Synchronous);
    m_inputAspect = new Qt3DInput::QInputAspect;
    m_logicAspect = new Qt3DLogic::QLogicAspect;

    m_engine->aspectEngine()->registerAspect(m_renderAspect);
    m_engine->aspectEngine()->registerAspect(m_inputAspect);
    m_engine->aspectEngine()->registerAspect(m_logicAspect);
    connect(this, &QHeadMountedDisplay::requestRun, this, &QHeadMountedDisplay::run, Qt::QueuedConnection);
}

QHeadMountedDisplay::~QHeadMountedDisplay()
{
    if(m_surface)
        delete m_surface;
}

void QHeadMountedDisplay::registerAspect(Qt3DCore::QAbstractAspect *aspect)
{
    m_engine->aspectEngine()->registerAspect(aspect);
}

void QHeadMountedDisplay::registerAspect(const QString &name)
{
    m_engine->aspectEngine()->registerAspect(name);
}

void QHeadMountedDisplay::setSource(const QUrl &source)
{
    m_source = source;
    if(!m_initialized)
    {
        // Connect to the QQmlAspectEngine's statusChanged signal so that when the QML is loaded
        // and th eobjects hav ebeen instantiated, but before we set them on the QAspectEngine we
        // can swoop in and set the window surface and camera on the framegraph and ensure the camera
        // respects the window's aspect ratio
        connect(m_engine.data(), &Qt3DCore::Quick::QQmlAspectEngine::sceneCreated,
                this, &QHeadMountedDisplay::onSceneCreated);


        qmlRegisterType<QVRCamera>("vr", 2, 0, "VrCamera");
        m_engine->setSource(m_source);

        // Set the QQmlIncubationController on the window
        // to benefit from asynchronous incubation
        if (!m_incubationController)
            m_incubationController = new Qt3DQuickVirtualRealityIncubationController(this);

        m_engine->qmlEngine()->setIncubationController(m_incubationController);

        m_initialized = true;
    }
    else
    {
        qDebug() << "source already set, not yet implemented."; // TO DO
    }
}

Qt3DCore::Quick::QQmlAspectEngine *QHeadMountedDisplay::engine() const
{
    return m_engine.data();
}

qreal QHeadMountedDisplay::refreshRate()
{
    Q_ASSERT(m_apibackend);
    return m_apibackend->refreshRate(m_hmdId);
}

void QHeadMountedDisplay::onSceneCreated(QObject *rootObject)
{
    Q_ASSERT(rootObject);

    setWindowSurface(rootObject);

    m_rootItem = rootObject;//qobject_cast<QQuickItem *>(rootObject);
//    if (m_cameraAspectRatioMode == AutomaticAspectRatio) {
//        // Set aspect ratio of first camera to match the window
//        QList<Qt3DRender::QCamera *> cameras
//                = rootObject->findChildren<Qt3DRender::QCamera *>();
//        if (cameras.isEmpty()) {
//            qWarning() << "No camera found";
//        } else {
//            m_camera = cameras.first();
//            setCameraAspectModeHelper();
//        }
//    }

    // Set ourselves up as a source of input events for the input aspect
    Qt3DInput::QInputSettings *inputSettings = rootObject->findChild<Qt3DInput::QInputSettings *>();
    if (inputSettings) {
        inputSettings->setEventSource(this);
    } else {
        qWarning() << "No Input Settings found, keyboard and mouse events won't be handled";
    }
}

void QHeadMountedDisplay::run() {

    QOpenGLContext::currentContext()->makeCurrent(m_surface);
    m_fbo->bind();
    Q_ASSERT(false);//TO DO: wrong thread for m_fbo->bind(). need QSGNode. What about createTextureFromId?
//        if (m_aspectEngine->rootEntity() != m_item->entity())
//            scheduleRootEntityChange();
    //TODO: QVrSelector. This is the object with all parameters then
    QVRCamera *vrCamera(nullptr);
    if(m_rootItem)
        vrCamera = m_rootItem->findChild<QVRCamera *>();
    QMatrix4x4 leftEye;
    QMatrix4x4 rightEye;
    m_apibackend->getEyeMatrices(leftEye, rightEye);
    if(vrCamera != nullptr)
        vrCamera->update(leftEye, rightEye);
    static_cast<Qt3DRender::QRenderAspectPrivate*>(Qt3DRender::QRenderAspectPrivate::get(m_renderAspect))->renderSynchronous();
    //m_fbo->bindDefault();

    m_apibackend->swapToHeadset();
    emit requestRun();
}

void QHeadMountedDisplay::setWindowSurface(QObject *rootObject)
{
    if(!(m_context = QOpenGLContext::currentContext()))
    {
        m_context = new QOpenGLContext();
        m_context->create();
        m_context->makeCurrent(m_surface);
        Q_ASSERT(QOpenGLContext::currentContext() != nullptr);
        static_cast<Qt3DRender::QRenderAspectPrivate*>(Qt3DRender::QRenderAspectPrivate::get(m_renderAspect))->renderInitialize(m_context);
    }
    Qt3DRender::QRenderSurfaceSelector *surfaceSelector = Qt3DRender::QRenderSurfaceSelectorPrivate::find(rootObject);
    if (surfaceSelector)
        surfaceSelector->setSurface(m_surface);
    if (m_fbo) {
        delete m_fbo;
        m_fbo = nullptr;
    }
    const QSize texSize(m_apibackend->getRenderSurfaceSize());

    //TODO: At the moment FBO is recreated each frame
    if (!m_fbo) {
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        format.setTextureTarget(GL_TEXTURE_2D);
        format.setSamples(0);
        format.setMipmap(false);

        QSurfaceFormat notUsedTodo;
        GLuint texId = m_apibackend->createSurface(m_hmdId, texSize, notUsedTodo);

        m_fbo = new QOpenGLFramebufferObject(texSize, format, texId );
        //m_quickWindow->setRenderTarget(m_fbo);
    }
//    if(!sizeOkay)
//    {
//        // Size of m_window can differ from m_quickWindow and rendertarget size
//        m_quickWindow->resize(texSize);
//        m_quickWindow->contentItem()->setWidth(texSize.width());
//        m_quickWindow->contentItem()->setHeight(texSize.height());
//        m_quickWindow->setGeometry(0, 0, texSize.width(), texSize.height());
//    }
}

} // Qt3DVirtualReality

QT_END_NAMESPACE

#include "qheadmounteddisplay.moc"
