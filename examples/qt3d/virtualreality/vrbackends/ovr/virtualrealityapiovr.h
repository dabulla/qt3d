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
    bool bindFrambufferObject(int hmdId);

    qreal refreshRate(int hmdId) const;
    QMatrix4x4 headPose(int hmdId);
    QSize getRenderTargetSize();

    int timeUntilNextFrame();

    void swapToHeadset();

    void getEyeMatrices(QMatrix4x4 &leftEye, QMatrix4x4 &rightEye);

    void getProjectionMatrices(QMatrix4x4 &leftProjection, QMatrix4x4 &rightProjection);

    int numberOfTrackedObjects();
    void getTrackedObject(int id,QMatrix4x4 &transform);
    TrackedObjectType getTrackedObjectType(int id);
    void getTrackedObjectVertices(int id, QVector<float> &vertices);
    void getTrackedObjectTexture(int id, QOpenGLTexture *texture);

    void getMirrorTexture(QOpenGLTexture *outMirrorTexture);

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
