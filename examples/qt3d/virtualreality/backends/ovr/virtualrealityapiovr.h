#ifndef VIRTUALREALITYAPIOVR_H
#define VIRTUALREALITYAPIOVR_H

#include "../../qvirtualrealityapibackend.h"
#include "OVR_CAPI_GL.h"

class OvrSwapChain;

class VirtualRealityApiOvr : public Qt3DVirtualReality::QVirtualRealityApiBackend
{
public:
    static bool isRuntimeInstalled();
    VirtualRealityApiOvr();
    ~VirtualRealityApiOvr();
    bool isHmdPresent();

    void initialize();
    void shutdown();
    bool bindFrambufferObject();

    qreal refreshRate(int hmdId) const;
    QMatrix4x4 headPose(int hmdId);
    QSize getRenderTargetSize();

    int timeUntilNextFrame();

    void swapToHeadset();

    void getEyeMatrices(QMatrix4x4 &leftEye, QMatrix4x4 &rightEye);

    void getProjectionMatrices(QMatrix4x4 &leftProjection, QMatrix4x4 &rightProjection);
private:
    bool m_sessionStarted;
    ovrSession m_session;
    ovrGraphicsLuid m_luid;
    //TO DO: decouple using hmdId
    ovrHmdDesc m_hmdDesc;
    ovrPosef   m_eyeRenderPose[2];
    double m_sensorSampleTime;
    long long m_frameIndex;
    OvrSwapChain *m_swapChain;

    bool initializeIfHmdIsPresent();
};

#endif
