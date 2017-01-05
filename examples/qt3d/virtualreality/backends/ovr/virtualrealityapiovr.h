#ifndef VIRTUALREALITYAPIOVR_H
#define VIRTUALREALITYAPIOVR_H

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

    void initialize();
    void createSurface(int hmdId, const QSize &size, const QSurfaceFormat &format);
    GLuint currentTextureId();
    GLuint setSurface(int hmdId, GLuint textureId);
    void destroySurface(int hmdId, GLuint textureId);

    qreal refreshRate(int hmdId) const;
    QMatrix4x4 headPose(int hmdId);
    QSize getRenderSurfaceSize();

    void swapToHeadset();

    void getEyeMatrices(QMatrix4x4 &leftEye, QMatrix4x4 &rightEye);

    void getProjectionMatrices(QMatrix4x4 &leftProjection, QMatrix4x4 &rightProjection);
private:
    ovrSession m_session;
    ovrGraphicsLuid m_luid;
    //TO DO: decouple using hmdId
    ovrHmdDesc m_hmdDesc;
    ovrPosef   m_eyeRenderPose[2];
    QVrRendertarget *m_bothEyesTemp;
    double m_sensorSampleTime;
    long long m_frameIndex;
};

#endif
