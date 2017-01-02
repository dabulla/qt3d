#ifndef QT3DVIRTUALREALITYAPI_H
#define QT3DVIRTUALREALITYAPI_H

#include "qt3dvr_global.h"

#include <QObject>
#include <qopengl.h>
#include <QSize>

QT_BEGIN_NAMESPACE

namespace Qt3DVirtualReality {

class QHeadMountedDisplay;
class QVirtualRealityApiPrivate;

class QHeadMountedDisplayFormat {
//    void setSize(const QSize& size);
//    const QSize& size();
//    void setRenderingSurface(GLuint textureId);
//    GLuint renderingSurface();
};
/**
 * @brief The QVrApi class
 * TODO: Uses virtual methods internally to decide which concrete implementation was chosen. Alternatively the public interface could use template parameters.
 */
class QT3DVR_EXPORT QVirtualRealityApi : public QObject
{
    Q_OBJECT
public:
    enum Type
    {
        Ovr, // OculusVR
        OSVR, // OpenSourceVR (e.g. Razer)
        OpenVR // HTC Vive / SteamVR
    };

    ///
    /// \brief QVirtualRealityApi() Tries to choose the most specific API for the connected HMD.
    /// 1) Ovr, 2) OpenVR, 3) OSVR
    ///
    QVirtualRealityApi();
    ///
    /// \brief QVirtualRealityApi Initialize with known API of given type
    /// \param type
    ///
    explicit QVirtualRealityApi(QVirtualRealityApi::Type type);
    QVirtualRealityApi(const QVirtualRealityApi &other);
    ~QVirtualRealityApi();

    QVirtualRealityApi &operator=(const QVirtualRealityApi &other);

    ///
    /// \brief type
    /// \return type of API used to connect to the headmounted display
    ///
    QVirtualRealityApi::Type type() const;

    bool isHmdPresent() const;
    static bool isRuntimeInstalled(QVirtualRealityApi::Type type);
    bool supportsSetSurface() const;

    QHeadMountedDisplay *getHmd(int hmdId, const QHeadMountedDisplayFormat &format);
private:
    QVirtualRealityApi(QVirtualRealityApiPrivate &dd, QObject *parent = nullptr);
    Q_DECLARE_PRIVATE(QVirtualRealityApi)
};

}

QT_END_NAMESPACE

#endif
