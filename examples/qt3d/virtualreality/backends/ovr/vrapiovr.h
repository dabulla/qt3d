#ifndef VRAPIOVR_H
#define VRAPIOVR_H

#include "../../qvirtualrealityapibackend.h"
#include "OVR_CAPI_GL.h"
class QSurfaceFormat;
class QVrRendertarget;

class VirtualRealityApiOvr : public Qt3DVirtualReality::QVirtualRealityApiBackend
{
public:
    VirtualRealityApiOvr();
    bool isHmdPresent() const;
    bool supportsSetSurface() const;

    GLuint createSurface(int hmdId, const QSize &size, const QSurfaceFormat &format);
    GLuint setSurface(int hmdId, GLuint textureId);

    void swapToHeadset();

    qreal refreshRate(int hmdId) const;
    QMatrix4x4 headPose(int hmdId);
private:
    ovrSession m_session;
    ovrGraphicsLuid m_luid;
    ovrHmdDesc m_hmdDesc;
    ovrPosef   m_eyeRenderPose[2];
    QVrRendertarget *m_bothEyesTemp;
    double m_sensorSampleTime;
    long long m_frameIndex;
};

#endif
