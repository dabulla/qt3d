#include "vrrenderer.h"
#include <iostream>

//#include "GL/CAPI_GLE.h"
#include "Extras/OVR_Math.h"
#include "OVR_CAPI_GL.h"
#include <assert.h>
#include <QDebug>

#if defined(_WIN32)
#include <dxgi.h> // for GetDefaultAdapterLuid
//#pragma comment(lib, "dxgi.lib")
#endif

#include <QOpenGLFunctions>

#include <QQuickWindow>

#include "qvrcamera.h"

#include <Qt3DRender/QCameraLens>

#include "qvrrendertarget.h"

using namespace OVR;

static ovrGraphicsLuid GetDefaultAdapterLuid()
{
    ovrGraphicsLuid luid = ovrGraphicsLuid();

#if defined(_WIN32)
    IDXGIFactory* factory = nullptr;

    if (SUCCEEDED(CreateDXGIFactory(IID_PPV_ARGS(&factory))))
    {
        IDXGIAdapter* adapter = nullptr;

        if (SUCCEEDED(factory->EnumAdapters(0, &adapter)))
        {
            DXGI_ADAPTER_DESC desc;

            adapter->GetDesc(&desc);
            memcpy(&luid, &desc.AdapterLuid, sizeof(luid));
            adapter->Release();
        }

        factory->Release();
    }
#endif

    return luid;
}

class VrRendererPrivate
{
public:
    VrRendererPrivate(VrRenderer* parent)
        :m_bothEyes(nullptr),
         m_sensorSampleTime(0.0),
         m_frameIndex(0),
         m_initialized(false),
         m_vrCamera(nullptr),
         m_parent(parent){}
    ~VrRendererPrivate()
    {
        if(m_bothEyes)
            delete m_bothEyes;
    }
    ovrSession m_session;
    ovrGraphicsLuid m_luid;
    ovrHmdDesc m_hmdDesc;
    ovrPosef   m_eyeRenderPose[2];
    QVrRendertarget *m_bothEyes;

    double m_sensorSampleTime;
    long long m_frameIndex;
    bool m_initialized;
    QVRCamera *m_vrCamera;
    VrRenderer* m_parent;
    void init();
};

VrRenderer::VrRenderer()
    :m_p(new VrRendererPrivate(this))
{
    qDebug() << "VRRender Init:" << m_p->m_frameIndex;
    ovrResult result = ovr_Initialize(nullptr);
    if (!OVR_SUCCESS(result))
    {
        qDebug() << "Failed to initialize libOVR.";
        return;
    }

    result = ovr_Create(&m_p->m_session, &m_p->m_luid);
    if (!OVR_SUCCESS(result))
    {
        qDebug() << "Failed to create session libOVR.";
        return;
    }

    if (memcmp(&m_p->m_luid, &GetDefaultAdapterLuid(), sizeof(m_p->m_luid))) // If luid that the Rift is on is not the default adapter LUID...
    {
        qDebug() << "OpenGL supports only the default graphics adapter.";
        return;
    }

    m_p->m_hmdDesc = ovr_GetHmdDesc(m_p->m_session);
}

VrRenderer::~VrRenderer()
{
    qDebug() << "VRRender Destruct:" << m_p->m_frameIndex;
    delete m_p;
}

void VrRenderer::initialize()
{
    if(m_p->m_initialized) return;
    ovrSizei idealTextureSizeLeft = ovr_GetFovTextureSize(m_p->m_session, ovrEye_Left, m_p->m_hmdDesc.DefaultEyeFov[ovrEye_Left], 1.0f);
    ovrSizei idealTextureSizeRight = ovr_GetFovTextureSize(m_p->m_session, ovrEye_Right, m_p->m_hmdDesc.DefaultEyeFov[ovrEye_Right], 1.0f);
    if(idealTextureSizeLeft.h != idealTextureSizeRight.h)
    {
        qDebug() << "WARN: Right and left eye differ in height";
    }
    else
    {
        qDebug() << "ideal Texture Size: " << idealTextureSizeLeft.w << " x " << idealTextureSizeLeft.h;
    }
    ovrSizei rtSize;
    rtSize.w = idealTextureSizeLeft.w+idealTextureSizeRight.w;
    rtSize.h = idealTextureSizeLeft.h;
    m_p->m_bothEyes = new QVrRendertarget(m_p->m_session, rtSize);
    ovr_SetTrackingOriginType(m_p->m_session, ovrTrackingOrigin_FloorLevel);
    m_p->m_initialized = true;
}

void VrRenderer::beforeRender()
{
    if(!m_p->m_initialized)
    {
        initialize();
    }
//    ovrEyeRenderDesc eyeRenderDesc[2];
//    eyeRenderDesc[0] = ovr_GetRenderDesc(m_p->m_session, ovrEye_Left, m_p->m_hmdDesc.DefaultEyeFov[0]);
//    eyeRenderDesc[1] = ovr_GetRenderDesc(m_p->m_session, ovrEye_Right, m_p->m_hmdDesc.DefaultEyeFov[1]);
//    // Get eye poses, feeding in correct IPD offset
//    ovrVector3f               HmdToEyeOffset[2] = { eyeRenderDesc[0].HmdToEyeOffset,
//                                                    eyeRenderDesc[1].HmdToEyeOffset };

//    ovr_GetEyePoses(m_p->m_session, m_p->m_frameIndex, ovrTrue, HmdToEyeOffset, m_p->m_eyeRenderPose, &m_p->m_sensorSampleTime);

//    if(m_p->m_vrCamera)
//    {
//        ovrVector3f &posLeft = m_p->m_eyeRenderPose[ovrEye_Left].Position;
//        QVector3D headPosLeft(posLeft.x, posLeft.y, posLeft.z);
//        ovrVector3f &posRight = m_p->m_eyeRenderPose[ovrEye_Right].Position;
//        QVector3D headPosRight(posRight.x, posRight.y, posRight.z);
//        ovrQuatf &orientLeft = m_p->m_eyeRenderPose[ovrEye_Left].Orientation;
//        QQuaternion headOrientationLeft(orientLeft.w, orientLeft.x, orientLeft.y, orientLeft.z);
//        ovrQuatf &orientRight = m_p->m_eyeRenderPose[ovrEye_Right].Orientation;
//        QQuaternion headOrientationRight(orientRight.w, orientRight.x, orientRight.y, orientRight.z);
//        m_p->m_vrCamera->update(headPosLeft, headPosRight, headOrientationLeft, headOrientationRight);
//    }
    //m_p->m_bothEyes->SetAndClearRenderSurface();
}

void VrRenderer::renderToHeadset()
{
    qDebug() << "Render to Headset:" << m_p->m_frameIndex;
    m_p->m_bothEyes->Commit();

    ovrLayerEyeFov ld;
    ld.Header.Type  = ovrLayerType_EyeFov;
    ld.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;   // Because OpenGL.

    ld.ColorTexture[ovrEye_Left] = m_p->m_bothEyes->m_oVrTextureChain;
    ld.Viewport[ovrEye_Left]     = Recti(0, 0, m_p->m_bothEyes->size().w/2, m_p->m_bothEyes->size().h);
    ld.Fov[ovrEye_Left]          = m_p->m_hmdDesc.DefaultEyeFov[ovrEye_Left];
    ld.RenderPose[ovrEye_Left]   = m_p->m_eyeRenderPose[ovrEye_Left];

    ld.ColorTexture[ovrEye_Right] = m_p->m_bothEyes->m_oVrTextureChain;
    ld.Viewport[ovrEye_Right]     = Recti(m_p->m_bothEyes->size().w/2, 0, m_p->m_bothEyes->size().w/2, m_p->m_bothEyes->size().h);
    ld.Fov[ovrEye_Right]          = m_p->m_hmdDesc.DefaultEyeFov[ovrEye_Right];
    ld.RenderPose[ovrEye_Right]   = m_p->m_eyeRenderPose[ovrEye_Right];

    ld.SensorSampleTime  = m_p->m_sensorSampleTime;

    ovrLayerHeader* layers = &ld.Header;
    ovrResult result = ovr_SubmitFrame(m_p->m_session, m_p->m_frameIndex, nullptr, &layers, 1);

    if (!OVR_SUCCESS(result))
    {
        ovrErrorInfo inf;
        ovr_GetLastErrorInfo(&inf);
        qDebug() << inf.ErrorString;
        return;
    }
    m_p->m_frameIndex++;
}

void VrRenderer::setVrCamera(QVRCamera *cam)
{
    m_p->m_vrCamera = cam;
    //m_p->m_vrCamera->leftCamera()->setProjectionType(Qt3DRender::QCameraLens::CustomProjection);
    //m_p->m_vrCamera->rightCamera()->setProjectionType(Qt3DRender::QCameraLens::CustomProjection);
    Matrix4f   projLeft   = ovrMatrix4f_Projection(m_p->m_hmdDesc.DefaultEyeFov[ovrEye_Left], 0.2f, 1000.0f, ovrProjection_None);
    Matrix4f   projRight  = ovrMatrix4f_Projection(m_p->m_hmdDesc.DefaultEyeFov[ovrEye_Right], 0.2f, 1000.0f, ovrProjection_None);
    QMatrix4x4 projL(projLeft.M[0][0], projLeft.M[0][1], projLeft.M[0][2], projLeft.M[0][3],
                     projLeft.M[1][0], projLeft.M[1][1], projLeft.M[1][2], projLeft.M[1][3],
                     projLeft.M[2][0], projLeft.M[2][1], projLeft.M[2][2], projLeft.M[2][3],
                     projLeft.M[3][0], projLeft.M[3][1], projLeft.M[3][2], projLeft.M[3][3]);
    QMatrix4x4 projR(projRight.M[0][0], projRight.M[0][1], projRight.M[0][2], projRight.M[0][3],
                     projRight.M[1][0], projRight.M[1][1], projRight.M[1][2], projRight.M[1][3],
                     projRight.M[2][0], projRight.M[2][1], projRight.M[2][2], projRight.M[2][3],
                     projRight.M[3][0], projRight.M[3][1], projRight.M[3][2], projRight.M[3][3]);
    m_p->m_vrCamera->setProjections(projL, projR);
    m_p->m_vrCamera->setLeftNormalizedViewportRect(QRectF(0.0f, 0.0f, 0.5f, 1.0f));
    m_p->m_vrCamera->setRightNormalizedViewportRect(QRectF(0.5f, 0.0f, 0.5f, 1.0f));
}

void VrRenderer::sync()
{
    ovrEyeRenderDesc eyeRenderDesc[2];
    eyeRenderDesc[0] = ovr_GetRenderDesc(m_p->m_session, ovrEye_Left, m_p->m_hmdDesc.DefaultEyeFov[0]);
    eyeRenderDesc[1] = ovr_GetRenderDesc(m_p->m_session, ovrEye_Right, m_p->m_hmdDesc.DefaultEyeFov[1]);
    // Get eye poses, feeding in correct IPD offset
    ovrVector3f               HmdToEyeOffset[2] = { eyeRenderDesc[0].HmdToEyeOffset,
                                                    eyeRenderDesc[1].HmdToEyeOffset };

    ovr_GetEyePoses(m_p->m_session, m_p->m_frameIndex, ovrTrue, HmdToEyeOffset, m_p->m_eyeRenderPose, &m_p->m_sensorSampleTime);

    if(m_p->m_vrCamera)
    {
        ovrVector3f &posLeft = m_p->m_eyeRenderPose[ovrEye_Left].Position;
        ovrVector3f &posRight = m_p->m_eyeRenderPose[ovrEye_Right].Position;
        ovrQuatf orientLeft = m_p->m_eyeRenderPose[ovrEye_Left].Orientation;
        ovrQuatf orientRight = m_p->m_eyeRenderPose[ovrEye_Right].Orientation;
        //orientLeft.y *= -1.0f;
        //orientRight.y *= -1.0f;

        Matrix4f finalRollPitchYaw = Matrix4f(orientLeft);
        Vector3f finalUp = finalRollPitchYaw.Transform(Vector3f(0, 1, 0));
        Vector3f finalForward = finalRollPitchYaw.Transform(Vector3f(0, 0, -1));
        Vector3f shiftedEyePos = Vector3f(m_p->m_vrCamera->offset().x(), m_p->m_vrCamera->offset().y(), m_p->m_vrCamera->offset().z()) + Vector3f(posLeft) * 100.0f;
        Matrix4f m = Matrix4f::LookAtRH(shiftedEyePos, shiftedEyePos + finalForward, finalUp);
        QMatrix4x4 mLeft(m.M[0][0], m.M[0][1], m.M[0][2], m.M[0][3],
                         m.M[1][0], m.M[1][1], m.M[1][2], m.M[1][3],
                         m.M[2][0], m.M[2][1], m.M[2][2], m.M[2][3],
                         m.M[3][0], m.M[3][1], m.M[3][2], m.M[3][3]);

        finalRollPitchYaw = Matrix4f(orientRight);
        finalUp = finalRollPitchYaw.Transform(Vector3f(0, 1, 0));
        finalForward = finalRollPitchYaw.Transform(Vector3f(0, 0, -1));
        shiftedEyePos = Vector3f(m_p->m_vrCamera->offset().x(), m_p->m_vrCamera->offset().y(), m_p->m_vrCamera->offset().z()) + Vector3f(posRight) * 100.0f;
        m = Matrix4f::LookAtRH(shiftedEyePos, shiftedEyePos + finalForward, finalUp);
        QMatrix4x4 mRight(m.M[0][0], m.M[0][1], m.M[0][2], m.M[0][3],
                         m.M[1][0], m.M[1][1], m.M[1][2], m.M[1][3],
                         m.M[2][0], m.M[2][1], m.M[2][2], m.M[2][3],
                         m.M[3][0], m.M[3][1], m.M[3][2], m.M[3][3]);
//        mLeft = mLeft.transposed();
//        mRight = mRight.transposed();
        m_p->m_vrCamera->update(mLeft, mRight);
//        Matrix4f   viewLeft = Matrix4f(orientLeft.Inverted()) * Matrix4f::Translation(-posLeft);
//        Matrix4f   viewRight = Matrix4f(orientRight.Inverted()) * Matrix4f::Translation(-posRight);
//        viewLeft = Matrix4f::Translation(eyeRenderDesc[ovrEye_Left].) * viewLeft;
//        ovrVector3f &posLeft = m_p->m_eyeRenderPose[ovrEye_Left].Position;
//        QVector3D headPosLeft(posLeft.x, posLeft.y, posLeft.z);
//        ovrVector3f &posRight = m_p->m_eyeRenderPose[ovrEye_Right].Position;
//        QVector3D headPosRight(posRight.x, posRight.y, posRight.z);
//        headPosLeft *= 100.0f;
//        headPosRight *= 100.0f;
//        ovrQuatf &orientLeft = m_p->m_eyeRenderPose[ovrEye_Left].Orientation;
//        QQuaternion headOrientationLeft(orientLeft.w, orientLeft.x, orientLeft.y, orientLeft.z);
//        ovrQuatf &orientRight = m_p->m_eyeRenderPose[ovrEye_Right].Orientation;
//        QQuaternion headOrientationRight(orientRight.w, orientRight.x, orientRight.y, orientRight.z);
//        m_p->m_vrCamera->update(headPosLeft, headPosRight, headOrientationLeft, headOrientationRight);
    }
}

GLuint VrRenderer::getTexId() const
{
    return m_p->m_bothEyes->texId();
}

//GLuint VrRenderer::getTexIdRight() const
//{
//    return m_p->m_bothEyes->texId();
//}

QSize VrRenderer::getSize() const
{
    return QSize(m_p->m_bothEyes->size().w, m_p->m_bothEyes->size().h);
}

//QSize VrRenderer::getSizeRight() const
//{
//    return QSize(m_p->m_bothEyes->size().w, m_p->m_bothEyes->size().h);
//}
