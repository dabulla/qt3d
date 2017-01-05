#include "virtualrealityapiovr.h"

#include "Extras/OVR_Math.h"
#include "OVR_CAPI_GL.h"

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

VirtualRealityApiOvr::VirtualRealityApiOvr()
    : m_bothEyesTemp(nullptr)
    , m_sensorSampleTime(0.0)
    , m_frameIndex(0)
    , m_swapChain(nullptr)
{
}

VirtualRealityApiOvr::~VirtualRealityApiOvr()
{
    if(m_swapChain != nullptr)
        delete m_swapChain;
}

bool VirtualRealityApiOvr::isHmdPresent() const
{
    return true;
}

bool VirtualRealityApiOvr::supportsSetSurface() const
{
    return false;
}

void VirtualRealityApiOvr::initialize()
{
    qDebug() << "VRRender Init:" << m_frameIndex;
    ovrResult result = ovr_Initialize(nullptr);
    if (!OVR_SUCCESS(result))
    {
        qDebug() << "Failed to initialize libOVR.";
        return;
    }
    result = ovr_Create(&m_session, &m_luid);
    if (!OVR_SUCCESS(result))
    {
        qDebug() << "Failed to create session libOVR.";
        return;
    }

    if (memcmp(&m_luid, &GetDefaultAdapterLuid(), sizeof(m_luid))) // If luid that the Rift is on is not the default adapter LUID...
    {
        qDebug() << "OpenGL supports only the default graphics adapter.";
        return;
    }

    m_hmdDesc = ovr_GetHmdDesc(m_session);
    ovr_SetTrackingOriginType(m_session, ovrTrackingOrigin_FloorLevel);
    m_swapChain = new OvrSwapChain(m_session, getRenderTargetSize());
}

bool VirtualRealityApiOvr::bindFrambufferObject()
{
    m_swapChain->bindCurrentChainIndexFramebuffer();
    return true;
}

void VirtualRealityApiOvr::swapToHeadset()
{
    qDebug() << "Render to Headset:" << m_frameIndex;
    m_swapChain->commit();

    ovrLayerEyeFov ld;
    ld.Header.Type  = ovrLayerType_EyeFov;
    ld.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;   // Because OpenGL.

    ld.ColorTexture[ovrEye_Left] = m_swapChain->ovrTextureChain();
    int halfWidth = m_swapChain->size().width()/2;
    int height = m_swapChain->size().height();
    ld.Viewport[ovrEye_Left]     = Recti(0, 0, halfWidth, height);
    ld.Fov[ovrEye_Left]          = m_hmdDesc.DefaultEyeFov[ovrEye_Left];
    ld.RenderPose[ovrEye_Left]   = m_eyeRenderPose[ovrEye_Left];

    ld.ColorTexture[ovrEye_Right] = m_swapChain->ovrTextureChain();
    ld.Viewport[ovrEye_Right]     = Recti(halfWidth, 0, halfWidth, height);
    ld.Fov[ovrEye_Right]          = m_hmdDesc.DefaultEyeFov[ovrEye_Right];
    ld.RenderPose[ovrEye_Right]   = m_eyeRenderPose[ovrEye_Right];

    ld.SensorSampleTime  = m_sensorSampleTime;

    ovrLayerHeader* layers = &ld.Header;
    ovrResult result = ovr_SubmitFrame(m_session, m_frameIndex, nullptr, &layers, 1);

    if (!OVR_SUCCESS(result))
    {
        ovrErrorInfo inf;
        ovr_GetLastErrorInfo(&inf);
        qDebug() << inf.ErrorString;
        return;
    }
    m_frameIndex++;
}

void VirtualRealityApiOvr::getEyeMatrices(QMatrix4x4 &leftEye, QMatrix4x4 &rightEye)
{
    ovrEyeRenderDesc eyeRenderDesc[2];
    eyeRenderDesc[0] = ovr_GetRenderDesc(m_session, ovrEye_Left, m_hmdDesc.DefaultEyeFov[0]);
    eyeRenderDesc[1] = ovr_GetRenderDesc(m_session, ovrEye_Right, m_hmdDesc.DefaultEyeFov[1]);
    // Get eye poses, feeding in correct IPD offset
    ovrVector3f               HmdToEyeOffset[2] = { eyeRenderDesc[0].HmdToEyeOffset,
                                                 eyeRenderDesc[1].HmdToEyeOffset };

    ovr_GetEyePoses(m_session, m_frameIndex, ovrTrue, HmdToEyeOffset, m_eyeRenderPose, &m_sensorSampleTime);

    ovrVector3f &posLeft = m_eyeRenderPose[ovrEye_Left].Position;
    ovrVector3f &posRight = m_eyeRenderPose[ovrEye_Right].Position;
    ovrQuatf orientLeft = m_eyeRenderPose[ovrEye_Left].Orientation;
    ovrQuatf orientRight = m_eyeRenderPose[ovrEye_Right].Orientation;
    //orientLeft.y *= -1.0f;
    //orientRight.y *= -1.0f;

    Matrix4f finalRollPitchYaw = Matrix4f(orientLeft);
    Vector3f finalUp = finalRollPitchYaw.Transform(Vector3f(0, 1, 0));
    Vector3f finalForward = finalRollPitchYaw.Transform(Vector3f(0, 0, -1));
    Vector3f shiftedEyePos = Vector3f(0.0,0.0,0.0);//m_p->m_vrCamera->offset().x(), m_p->m_vrCamera->offset().y(), m_p->m_vrCamera->offset().z()) + Vector3f(posLeft) * 100.0f;
    Matrix4f m = Matrix4f::LookAtRH(shiftedEyePos, shiftedEyePos + finalForward, finalUp);
    //TODO: don't copy
    leftEye = QMatrix4x4(m.M[0][0], m.M[0][1], m.M[0][2], m.M[0][3],
            m.M[1][0], m.M[1][1], m.M[1][2], m.M[1][3],
            m.M[2][0], m.M[2][1], m.M[2][2], m.M[2][3],
            m.M[3][0], m.M[3][1], m.M[3][2], m.M[3][3]);
    finalRollPitchYaw = Matrix4f(orientRight);
    finalUp = finalRollPitchYaw.Transform(Vector3f(0, 1, 0));
    finalForward = finalRollPitchYaw.Transform(Vector3f(0, 0, -1));
    //shiftedEyePos = Vector3f(m_p->m_vrCamera->offset().x(), m_p->m_vrCamera->offset().y(), m_p->m_vrCamera->offset().z()) + Vector3f(posRight) * 100.0f;
    m = Matrix4f::LookAtRH(shiftedEyePos, shiftedEyePos + finalForward, finalUp);
    //TODO: don't copy
    rightEye = QMatrix4x4(m.M[0][0], m.M[0][1], m.M[0][2], m.M[0][3],
                  m.M[1][0], m.M[1][1], m.M[1][2], m.M[1][3],
                  m.M[2][0], m.M[2][1], m.M[2][2], m.M[2][3],
            m.M[3][0], m.M[3][1], m.M[3][2], m.M[3][3]);
}

void VirtualRealityApiOvr::getProjectionMatrices(QMatrix4x4 &leftProjection, QMatrix4x4 &rightProjection)
{
    Matrix4f   projLeft   = ovrMatrix4f_Projection(m_hmdDesc.DefaultEyeFov[ovrEye_Left], 0.2f, 1000.0f, ovrProjection_None);
    Matrix4f   projRight  = ovrMatrix4f_Projection(m_hmdDesc.DefaultEyeFov[ovrEye_Right], 0.2f, 1000.0f, ovrProjection_None);
    QMatrix4x4 projL(projLeft.M[0][0], projLeft.M[0][1], projLeft.M[0][2], projLeft.M[0][3],
                     projLeft.M[1][0], projLeft.M[1][1], projLeft.M[1][2], projLeft.M[1][3],
                     projLeft.M[2][0], projLeft.M[2][1], projLeft.M[2][2], projLeft.M[2][3],
                     projLeft.M[3][0], projLeft.M[3][1], projLeft.M[3][2], projLeft.M[3][3]);
    QMatrix4x4 projR(projRight.M[0][0], projRight.M[0][1], projRight.M[0][2], projRight.M[0][3],
                     projRight.M[1][0], projRight.M[1][1], projRight.M[1][2], projRight.M[1][3],
                     projRight.M[2][0], projRight.M[2][1], projRight.M[2][2], projRight.M[2][3],
                     projRight.M[3][0], projRight.M[3][1], projRight.M[3][2], projRight.M[3][3]);
    leftProjection = projL;
    rightProjection = projR;
    //TODO: don't copy
}

qreal VirtualRealityApiOvr::refreshRate(int hmdId) const
{
    return 90; //TODO
}

QMatrix4x4 VirtualRealityApiOvr::headPose(int hmdId)
{
    return QMatrix4x4();
}

QSize VirtualRealityApiOvr::getRenderTargetSize()
{
    if(m_swapChain == nullptr)
    {
        ovrSizei idealTextureSizeLeft = ovr_GetFovTextureSize(m_session, ovrEye_Left, m_hmdDesc.DefaultEyeFov[ovrEye_Left], 1.0f);
        ovrSizei idealTextureSizeRight = ovr_GetFovTextureSize(m_session, ovrEye_Right, m_hmdDesc.DefaultEyeFov[ovrEye_Right], 1.0f);
        return QSize(idealTextureSizeLeft.w+idealTextureSizeRight.w, idealTextureSizeLeft.h);
    }
    return m_swapChain->size();
}
