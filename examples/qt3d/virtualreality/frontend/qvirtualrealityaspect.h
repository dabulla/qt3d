#ifndef QT3DVIRTUALREALITY_QVIRTUALREALITYASPECT_H
#define QT3DVIRTUALREALITY_QVIRTUALREALITYASPECT_H

#include <Qt3DCore/qabstractaspect.h>
#include <qt3dvr_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QRenderAspect;
}

namespace Qt3DVirtualReality {

class QVirtualRealityAspectPrivate;
class QHeadMountedDisplay;
//class QVirtualRealityApi;
class QVirtualRealityApiBackend;

//TO DO: might needs access to RenderAspect. UpdateWorldTransformJob must depend on QueryTrackedObjectsJob

class QT3DVR_EXPORT QVirtualRealityAspect : public Qt3DCore::QAbstractAspect
{
    Q_OBJECT
public:
    explicit QVirtualRealityAspect(QObject *parent = nullptr);
    ~QVirtualRealityAspect();

    void setHeadmountedDisplay(QHeadMountedDisplay *hmd);
    //void setVirtualRealityApi(QVirtualRealityApi *api);
    void setVirtualRealityApiBackend(QVirtualRealityApiBackend *apiBackend);
private:
    QVariant executeCommand(const QStringList &args) Q_DECL_OVERRIDE;
    QVector<Qt3DCore::QAspectJobPtr> jobsToExecute(qint64 time) Q_DECL_OVERRIDE;
    void onRegistered() Q_DECL_OVERRIDE;
    void onUnregistered() Q_DECL_OVERRIDE;

protected:
    explicit QVirtualRealityAspect(QVirtualRealityAspectPrivate &dd, QObject *parent);
    Q_DECLARE_PRIVATE(QVirtualRealityAspect)
};

} // namespace Qt3DVirtualReality

QT_END_NAMESPACE

#endif // QT3DVIRTUALREALITY_QVIRTUALREALITYASPECT_H
