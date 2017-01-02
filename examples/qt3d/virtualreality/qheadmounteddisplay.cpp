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
    : QOffscreenSurface()
    , m_engine(nullptr)
    , m_renderAspect(nullptr)
    , m_inputAspect(nullptr)
    , m_logicAspect(nullptr)
    , m_initialized(false)
    , m_incubationController(nullptr)
    , m_api(api)
    , m_apibackend(apibackend)
    , m_hmdId(hmdId)
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
    setFormat(format);
    QSurfaceFormat::setDefaultFormat(format);
    create();

    m_engine.reset(new Qt3DCore::Quick::QQmlAspectEngine);
    m_renderAspect = new Qt3DRender::QRenderAspect;
    m_inputAspect = new Qt3DInput::QInputAspect;
    m_logicAspect = new Qt3DLogic::QLogicAspect;

    m_engine->aspectEngine()->registerAspect(m_renderAspect);
    m_engine->aspectEngine()->registerAspect(m_inputAspect);
    m_engine->aspectEngine()->registerAspect(m_logicAspect);


}

QHeadMountedDisplay::~QHeadMountedDisplay()
{
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
    return m_apibackend->refreshRate(m_hmdId);
}

void QHeadMountedDisplay::onSceneCreated(QObject *rootObject)
{
    Q_ASSERT(rootObject);

    setWindowSurface(rootObject);

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

void QHeadMountedDisplay::setWindowSurface(QObject *rootObject)
{
    Qt3DRender::QRenderSurfaceSelector *surfaceSelector = Qt3DRender::QRenderSurfaceSelectorPrivate::find(rootObject);
    if (surfaceSelector)
        surfaceSelector->setSurface(this);
}

} // Qt3DVirtualReality

QT_END_NAMESPACE

#include "qheadmounteddisplay.moc"
