#ifndef VRAPIOVR_H
#define VRAPIOVR_H

#include "../../qvirtualrealityapibackend.h"
class QSurfaceFormat;

class VirtualRealityApiOvr : public Qt3DVirtualReality::QVirtualRealityApiBackend
{
    bool isHmdPresent() const;
    bool supportsSetSurface() const;

    GLuint createSurface(int hmdId, const QSize &size, const QSurfaceFormat &format);
    GLuint setSurface(int hmdId, GLuint textureId);

    qreal refreshRate(int hmdId) const;
    QMatrix4x4 headPose(int hmdId);
};

#endif
