#ifndef QT3DVIRTUALREALITY_QVIRTUALREALITYASPECT_P_H
#define QT3DVIRTUALREALITY_QVIRTUALREALITYASPECT_P_H

#include "qvirtualrealityaspect.h"
#include <Qt3DCore/private/qabstractaspect_p.h>
#include <QtCore/qsharedpointer.h>
#include "querytrackedobjectsjob_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DVirtualReality {

class QHeadMountedDisplay;
class QVirtualRealityApi;
class QVirtualRealityApiBackend;

class QVirtualRealityAspectPrivate : public Qt3DCore::QAbstractAspectPrivate
{
    QVirtualRealityAspectPrivate();

    Q_DECLARE_PUBLIC(QVirtualRealityAspect)

    void onEngineAboutToShutdown() Q_DECL_OVERRIDE;
    void registerBackendTypes();

    qint64 m_time;
    bool m_initialized;
    QSharedPointer<QueryTrackedObjectsJob> m_queryTrackedObjectsJob;


    QHeadMountedDisplay *m_hmd;
    //QVirtualRealityApi *m_api;
    QVirtualRealityApiBackend *m_apibackend;
};

} // namespace Qt3DLogic

QT_END_NAMESPACE

#endif // QT3DVIRTUALREALITY_QVIRTUALREALITYASPECT_P_H
