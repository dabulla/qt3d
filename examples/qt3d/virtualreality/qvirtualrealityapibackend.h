#ifndef QT3DVIRTUALREALITYAPIBACKEND_H
#define QT3DVIRTUALREALITYAPIBACKEND_H

#include "qt3dvr_global.h"
#include <QOpenGLFramebufferObject>
#include <QMatrix4x4>
#include <qopengl.h>
#include <QSize>
#include <QSurfaceFormat>

QT_BEGIN_NAMESPACE

namespace Qt3DVirtualReality {

class QVirtualRealityApiBackend
{
public:
    virtual bool isHmdPresent() const = 0;
    virtual bool supportsSetSurface() const = 0;

    // TO DO: Rename, so this does not confuse with QSurface
    virtual GLuint createSurface(int hmdId, const QSize &size, const QSurfaceFormat &format) = 0;
    virtual GLuint setSurface(int hmdId, GLuint textureId) = 0;
    virtual void destroySurface(int hmdId, GLuint textureId) = 0;

    virtual qreal refreshRate(int hmdId) const = 0;
    virtual QMatrix4x4 headPose(int hmdId) = 0;

    //TO DO: introduce getRecomendedSize()
    virtual QSize getRenderSurfaceSize() = 0;

    virtual void swapToHeadset() = 0;

    virtual void getEyeMatrices(QMatrix4x4 &leftEye, QMatrix4x4 &rightEye) = 0;
};

}

QT_END_NAMESPACE

#endif
