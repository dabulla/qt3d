#include "vrapiovr.h"

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
    :m_bothEyesTemp(nullptr),
     m_sensorSampleTime(0.0),
     m_frameIndex(0)
{

}

bool VirtualRealityApiOvr::isHmdPresent() const
{
    return true;
}

bool VirtualRealityApiOvr::supportsSetSurface() const
{
    return false;
}

GLuint VirtualRealityApiOvr::createSurface(int hmdId, const QSize &size, const QSurfaceFormat &format)
{
        qDebug() << "VRRender Init:" << m_frameIndex;
        ovrResult result = ovr_Initialize(nullptr);
        if (!OVR_SUCCESS(result))
        {
            qDebug() << "Failed to initialize libOVR.";
            return 0;
        }
        result = ovr_Create(&m_session, &m_luid);
        if (!OVR_SUCCESS(result))
        {
            qDebug() << "Failed to create session libOVR.";
            return 0;
        }

        if (memcmp(&m_luid, &GetDefaultAdapterLuid(), sizeof(m_luid))) // If luid that the Rift is on is not the default adapter LUID...
        {
            qDebug() << "OpenGL supports only the default graphics adapter.";
            return 0;
        }

        m_hmdDesc = ovr_GetHmdDesc(m_session);

        ovrSizei idealTextureSizeLeft = ovr_GetFovTextureSize(m_session, ovrEye_Left, m_hmdDesc.DefaultEyeFov[ovrEye_Left], 1.0f);
        ovrSizei idealTextureSizeRight = ovr_GetFovTextureSize(m_session, ovrEye_Right, m_hmdDesc.DefaultEyeFov[ovrEye_Right], 1.0f);
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
        m_bothEyesTemp = new QVrRendertarget(m_session, rtSize);
        ovr_SetTrackingOriginType(m_session, ovrTrackingOrigin_FloorLevel);

}

GLuint VirtualRealityApiOvr::setSurface(int hmdId, GLuint textureId)
{
    return 0;
}

void VirtualRealityApiOvr::swapToHeadset()
{
    qDebug() << "Render to Headset:" << m_frameIndex;
    m_bothEyesTemp->Commit();

    ovrLayerEyeFov ld;
    ld.Header.Type  = ovrLayerType_EyeFov;
    ld.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;   // Because OpenGL.

    ld.ColorTexture[ovrEye_Left] = m_bothEyesTemp->m_oVrTextureChain;
    ld.Viewport[ovrEye_Left]     = Recti(0, 0, m_bothEyesTemp->size().w/2, m_bothEyesTemp->size().h);
    ld.Fov[ovrEye_Left]          = m_hmdDesc.DefaultEyeFov[ovrEye_Left];
    ld.RenderPose[ovrEye_Left]   = m_eyeRenderPose[ovrEye_Left];

    ld.ColorTexture[ovrEye_Right] = m_bothEyesTemp->m_oVrTextureChain;
    ld.Viewport[ovrEye_Right]     = Recti(m_bothEyesTemp->size().w/2, 0, m_bothEyesTemp->size().w/2, m_bothEyesTemp->size().h);
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

qreal VirtualRealityApiOvr::refreshRate(int hmdId) const
{
    return 90; //TODO
}

QMatrix4x4 VirtualRealityApiOvr::headPose(int hmdId)
{
    return QMatrix4x4();
}
