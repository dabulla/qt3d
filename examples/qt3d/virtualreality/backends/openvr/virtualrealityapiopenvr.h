#ifndef VIRTUALREALITYAPIOPENVR_H
#define VIRTUALREALITYAPIOPENVR_H

#include "../../qvirtualrealityapibackend.h"
#include "openvr.h"
class QSurfaceFormat;
class QVrRendertarget;

class VirtualRealityApiOpenVR : public Qt3DVirtualReality::QVirtualRealityApiBackend
{
public:
    VirtualRealityApiOpenVR();
    bool isHmdPresent() const;

    void initialize();
    void shutdown();
    bool bindFrambufferObject();

    qreal refreshRate(int hmdId) const;
    QMatrix4x4 headPose(int hmdId);
    QSize getRenderTargetSize();

    void swapToHeadset();

    void getEyeMatrices(QMatrix4x4 &leftEye, QMatrix4x4 &rightEye);

    void getProjectionMatrices(QMatrix4x4 &leftProjection, QMatrix4x4 &rightProjection);
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
