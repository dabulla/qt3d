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
    ovrSizei rtSize;
    if(size.width() == 0 || size.height() == 0) {
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
        rtSize.w = idealTextureSizeLeft.w+idealTextureSizeRight.w;
        rtSize.h = idealTextureSizeLeft.h;
    } else {
        rtSize.w = size.width();
        rtSize.h = size.height();
    }

    m_bothEyesTemp = new QVrRendertarget(m_session, rtSize);
    ovr_SetTrackingOriginType(m_session, ovrTrackingOrigin_FloorLevel);
    return m_bothEyesTemp->texId();
}

GLuint VirtualRealityApiOvr::setSurface(int hmdId, GLuint textureId)
{
    return 0;
}

void VirtualRealityApiOvr::destroySurface(int hmdId, GLuint textureId)
{
    // TO DO
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

void VirtualRealityApiOvr::getEyeMatrices(QMatrix4x4 &leftEye, QMatrix4x4 &rightEye)
{
    ovrEyeRenderDesc eyeRenderDesc[2];
    eyeRenderDesc[0] = ovr_GetRenderDesc(m_session, ovrEye_Left, m_hmdDesc.DefaultEyeFov[0]);
    eyeRenderDesc[1] = ovr_GetRenderDesc(m_session, ovrEye_Right, m_hmdDesc.DefaultEyeFov[1]);
    // Get eye poses, feeding in correct IPD offset
    ovrVector3f               HmdToEyeOffset[2] = { eyeRenderDesc[0].HmdToEyeOffset,
                                                 eyeRenderDesc[1].HmdToEyeOffset };
    ovrPosef eyePoses[2];
    ovr_GetEyePoses(m_session, m_frameIndex, ovrTrue, HmdToEyeOffset, eyePoses, &m_sensorSampleTime);

    ovrVector3f &posLeft = eyePoses[ovrEye_Left].Position;
    ovrVector3f &posRight = eyePoses[ovrEye_Right].Position;
    ovrQuatf orientLeft = eyePoses[ovrEye_Left].Orientation;
    ovrQuatf orientRight = eyePoses[ovrEye_Right].Orientation;
    //orientLeft.y *= -1.0f;
    //orientRight.y *= -1.0f;

    Matrix4f finalRollPitchYaw = Matrix4f(orientLeft);
    Vector3f finalUp = finalRollPitchYaw.Transform(Vector3f(0, 1, 0));
    Vector3f finalForward = finalRollPitchYaw.Transform(Vector3f(0, 0, -1));
    Vector3f shiftedEyePos = Vector3f(0.0,0.0,0.0);//m_p->m_vrCamera->offset().x(), m_p->m_vrCamera->offset().y(), m_p->m_vrCamera->offset().z()) + Vector3f(posLeft) * 100.0f;
    Matrix4f m = Matrix4f::LookAtRH(shiftedEyePos, shiftedEyePos + finalForward, finalUp);
    leftEye = QMatrix4x4(m.M[0][0], m.M[0][1], m.M[0][2], m.M[0][3],
            m.M[1][0], m.M[1][1], m.M[1][2], m.M[1][3],
            m.M[2][0], m.M[2][1], m.M[2][2], m.M[2][3],
            m.M[3][0], m.M[3][1], m.M[3][2], m.M[3][3]);
    finalRollPitchYaw = Matrix4f(orientRight);
    finalUp = finalRollPitchYaw.Transform(Vector3f(0, 1, 0));
    finalForward = finalRollPitchYaw.Transform(Vector3f(0, 0, -1));
    //shiftedEyePos = Vector3f(m_p->m_vrCamera->offset().x(), m_p->m_vrCamera->offset().y(), m_p->m_vrCamera->offset().z()) + Vector3f(posRight) * 100.0f;
    m = Matrix4f::LookAtRH(shiftedEyePos, shiftedEyePos + finalForward, finalUp);
    rightEye = QMatrix4x4(m.M[0][0], m.M[0][1], m.M[0][2], m.M[0][3],
                  m.M[1][0], m.M[1][1], m.M[1][2], m.M[1][3],
                  m.M[2][0], m.M[2][1], m.M[2][2], m.M[2][3],
                  m.M[3][0], m.M[3][1], m.M[3][2], m.M[3][3]);
}

qreal VirtualRealityApiOvr::refreshRate(int hmdId) const
{
    return 90; //TODO
}

QMatrix4x4 VirtualRealityApiOvr::headPose(int hmdId)
{
    return QMatrix4x4();
}

QSize VirtualRealityApiOvr::getRenderSurfaceSize()
{
    if(m_bothEyesTemp == nullptr)
    {
        ovrSizei idealTextureSizeLeft = ovr_GetFovTextureSize(m_session, ovrEye_Left, m_hmdDesc.DefaultEyeFov[ovrEye_Left], 1.0f);
        ovrSizei idealTextureSizeRight = ovr_GetFovTextureSize(m_session, ovrEye_Right, m_hmdDesc.DefaultEyeFov[ovrEye_Right], 1.0f);
        return QSize(idealTextureSizeLeft.w+idealTextureSizeRight.w, idealTextureSizeLeft.h);
    }
    return QSize(m_bothEyesTemp->size().w, m_bothEyesTemp->size().h);
}
