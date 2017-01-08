#ifndef VIRTUALREALITYAPIOPENVR_H
#define VIRTUALREALITYAPIOPENVR_H

#include "../../qvirtualrealityapibackend.h"
#include "openvr.h"
class QSurfaceFormat;

class VirtualRealityApiOpenVR : public Qt3DVirtualReality::QVirtualRealityApiBackend
{
public:
    static bool isRuntimeInstalled();
    VirtualRealityApiOpenVR();
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
    void getTrackedObject(QMatrix4x4 &transform);
    int getTrackedObjectType();
    void getTrackedObjectVertices(QVector<float> &vertices);
    void getTrackedObjectTexture(QOpenGLTexture *texture);

    void getMirrorTexture(QOpenGLTexture *outMirrorTexture);

private:
    QOpenGLFramebufferObject *m_fbo;
    double m_sensorSampleTime;
    long long m_frameIndex;
    vr::IVRSystem *m_hmd;
    std::string m_driver;
    std::string m_display;
    vr::TrackedDevicePose_t m_trackedDevicePose[ vr::k_unMaxTrackedDeviceCount ];
    QMatrix4x4 m_devicePose[ vr::k_unMaxTrackedDeviceCount ];
    bool m_showTrackedDevice[ vr::k_unMaxTrackedDeviceCount ];

    int m_trackedControllerCount;
    int m_trackedControllerCountLast;
    int m_validPoseCount;
    int m_validPoseCountLast;
    std::string m_poseClasses;                              // what classes we saw poses for this frame
    char m_devClassChar[ vr::k_unMaxTrackedDeviceCount ];   // for each device, a character representing its class

    QMatrix4x4 m_hmdPose;
    QMatrix4x4 m_eyePosLeft;
    QMatrix4x4 m_eyePosRight;

    QMatrix4x4 getHmdMatrixProjectionEye(vr::Hmd_Eye nEye);
    QMatrix4x4 getHmdMatrixPoseEye(vr::Hmd_Eye nEye);
    QMatrix4x4 getCurrentViewMatrix(vr::Hmd_Eye nEye);
    void updateHmdMatrixPose();
    QMatrix4x4 convertSteamVrMatrixToQMatrix4x4(const vr::HmdMatrix34_t &matPose);
    void processVrEvent(const vr::VREvent_t &event);
    void setupCameras();
    bool m_poseNewEnough; //TO DO: openvr in example only updates poses once a frame
};

#endif
