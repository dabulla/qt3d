//#include "qvrmanager.h"
//#include <iostream>

////#include "GL/CAPI_GLE.h"
//#include "Extras/OVR_Math.h"
//#include "OVR_CAPI_GL.h"
//#include <assert.h>

//#if defined(_WIN32)
//#include <dxgi.h> // for GetDefaultAdapterLuid
////#pragma comment(lib, "dxgi.lib")
//#endif

//#include <QOpenGLFunctions>

//#include <QQuickWindow>

//#include "qvrrendertarget.h"

//using namespace OVR;

//static ovrGraphicsLuid GetDefaultAdapterLuid()
//{
//    ovrGraphicsLuid luid = ovrGraphicsLuid();

//#if defined(_WIN32)
//    IDXGIFactory* factory = nullptr;

//    if (SUCCEEDED(CreateDXGIFactory(IID_PPV_ARGS(&factory))))
//    {
//        IDXGIAdapter* adapter = nullptr;

//        if (SUCCEEDED(factory->EnumAdapters(0, &adapter)))
//        {
//            DXGI_ADAPTER_DESC desc;

//            adapter->GetDesc(&desc);
//            memcpy(&luid, &desc.AdapterLuid, sizeof(luid));
//            adapter->Release();
//        }

//        factory->Release();
//    }
//#endif

//    return luid;
//}

//class VRManagerPrivate
//{
//public:
//    VRManagerPrivate(VRManager* parent)
//        :m_eyeLeft(NULL),
//         m_eyeRight(NULL),
//         m_frameIndex(0),
//         m_initialized(false),
//         m_parent(parent){}
//    ~VRManagerPrivate()
//    {
////        if(m_eyeLeft)
////            delete m_eyeLeft;
////        if(m_eyeRight)
////            delete m_eyeRight;
//    }
//    ovrSession m_session;
//    ovrGraphicsLuid m_luid;
////    QVrRendertarget *m_eyeLeft;
////    QVrRendertarget *m_eyeRight;
//    ovrHmdDesc m_hmdDesc;
//    long long m_frameIndex;
//    bool m_initialized;
//    VRManager* m_parent;
//    void init();
//};

//VRManager::VRManager(QObject *parent)
//    :m_frameAction(new Qt3DLogic::QFrameAction()),
//      m_window(NULL),
//      m_p(new VRManagerPrivate(this))
//{
//    ovrResult result = ovr_Initialize(nullptr);
//    if (!OVR_SUCCESS(result))
//    {
//        qDebug() << "Failed to initialize libOVR.";
//        return;
//    }

//    result = ovr_Create(&m_p->m_session, &m_p->m_luid);
//    if (!OVR_SUCCESS(result))
//    {
//        qDebug() << "Failed to create session libOVR.";
//        return;
//    }

//    if (memcmp(&m_p->m_luid, &GetDefaultAdapterLuid(), sizeof(m_p->m_luid))) // If luid that the Rift is on is not the default adapter LUID...
//    {
//        qDebug() << "OpenGL supports only the default graphics adapter.";
//        return;
//    }

//    m_p->m_hmdDesc = ovr_GetHmdDesc(m_p->m_session);

//    QObject::connect(m_frameAction, SIGNAL(triggered(float)),
//                     this, SLOT(onTriggered(float)));
//    this->addComponent(m_frameAction);
//}

//void VRManagerPrivate::init()
//{
//    //    QQuickWindow *qquickwindow = m_parent->window();

//    //    if(qquickwindow == NULL)
//    //    {
//    //        //TODO: would be nice to get this working...
//    //        QObject *attached = qmlAttachedPropertiesObject<QQuickWindow>(this->m_parent, false);
//    //        if (attached)
//    //        {
//    //            QObject * obj = qvariant_cast<QObject *>(attached->property("window"));
//    //            qquickwindow = qobject_cast<QQuickWindow *>(obj);
//    //        }
//    //    }

//    // Make eye render buffers
//    ovrSizei idealTextureSize = ovr_GetFovTextureSize(m_session, ovrEye_Left, m_hmdDesc.DefaultEyeFov[ovrEye_Left], 1);
//    m_eyeLeft = new QVrRendertarget(m_session, idealTextureSize);
//    idealTextureSize = ovr_GetFovTextureSize(m_session, ovrEye_Right, m_hmdDesc.DefaultEyeFov[ovrEye_Right], 1);
//    m_eyeRight = new QVrRendertarget(m_session, idealTextureSize);

//    ovr_SetTrackingOriginType(m_session, ovrTrackingOrigin_FloorLevel);


//    m_initialized = true;
//}

//VRManager::~VRManager()
//{
//    delete m_p;
//}

//void VRManager::setWindow(QQuickWindow *window)
//{
//    if (m_window == window)
//        return;

//    if(m_window)
//    {
//        QObject::disconnect(m_window, &QQuickWindow::frameSwapped, this, &VRManager::onFrame);
//    }
//    m_window = window;

//    if(m_window)
//    {
//        QObject::connect(m_window, &QQuickWindow::frameSwapped, this, &VRManager::onFrame, Qt::DirectConnection);
//    }

//    Q_EMIT windowChanged(window);
//}

//void VRManager::onTriggered(float dt)
//{
//}

//void VRManager::onFrame()
//{
//    if(!m_p->m_initialized)
//    {
//        m_p->init();
//    }
//    ovrEyeRenderDesc eyeRenderDesc[2];
//    eyeRenderDesc[0] = ovr_GetRenderDesc(m_p->m_session, ovrEye_Left, m_p->m_hmdDesc.DefaultEyeFov[0]);
//    eyeRenderDesc[1] = ovr_GetRenderDesc(m_p->m_session, ovrEye_Right, m_p->m_hmdDesc.DefaultEyeFov[1]);
//    // Get eye poses, feeding in correct IPD offset
//    ovrPosef                  EyeRenderPose[2];
//    ovrVector3f               HmdToEyeOffset[2] = { eyeRenderDesc[0].HmdToEyeOffset,
//                                                    eyeRenderDesc[1].HmdToEyeOffset };

//    double sensorSampleTime;    // sensorSampleTime is fed into the layer later
//    ovr_GetEyePoses(m_p->m_session, m_p->m_frameIndex, ovrTrue, HmdToEyeOffset, EyeRenderPose, &sensorSampleTime);

//    m_p->m_eyeLeft->SetAndClearRenderSurface();
//    m_p->m_eyeLeft->UnsetRenderSurface();
//    m_p->m_eyeLeft->Commit();

//    m_p->m_eyeRight->SetAndClearRenderSurface();
//    m_p->m_eyeRight->UnsetRenderSurface();
//    m_p->m_eyeRight->Commit();

//    ovrLayerEyeFov ld;
//    ld.Header.Type  = ovrLayerType_EyeFov;
//    ld.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;   // Because OpenGL.

//    ld.ColorTexture[ovrEye_Left] = m_p->m_eyeLeft->m_oVrTextureChain;
//    ld.Viewport[ovrEye_Left]     = Recti(m_p->m_eyeLeft->size());
//    ld.Fov[ovrEye_Left]          = m_p->m_hmdDesc.DefaultEyeFov[ovrEye_Left];
//    ld.RenderPose[ovrEye_Left]   = EyeRenderPose[ovrEye_Left];
//    ld.SensorSampleTime  = sensorSampleTime;

//    ld.ColorTexture[ovrEye_Right] = m_p->m_eyeRight->m_oVrTextureChain;
//    ld.Viewport[ovrEye_Right]     = Recti(m_p->m_eyeRight->size());
//    ld.Fov[ovrEye_Right]          = m_p->m_hmdDesc.DefaultEyeFov[ovrEye_Right];
//    ld.RenderPose[ovrEye_Right]   = EyeRenderPose[ovrEye_Right];
//    ld.SensorSampleTime  = sensorSampleTime;

//    ovrLayerHeader* layers = &ld.Header;
//    ovrResult result = ovr_SubmitFrame(m_p->m_session, m_p->m_frameIndex, nullptr, &layers, 1);
//}



