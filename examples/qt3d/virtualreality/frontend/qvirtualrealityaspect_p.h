#ifndef QT3DVIRTUALREALITY_QVIRTUALREALITYASPECT_P_H
#define QT3DVIRTUALREALITY_QVIRTUALREALITYASPECT_P_H

#include "qvirtualrealityaspect.h"
#include <Qt3DCore/private/qabstractaspect_p.h>
//#include <QtCore/qsharedpointer.h>

QT_BEGIN_NAMESPACE

namespace Qt3DVirtualReality {

class QVirtualRealityAspectPrivate : public Qt3DCore::QAbstractAspectPrivate
{
    QVirtualRealityAspectPrivate();

    Q_DECLARE_PUBLIC(QVirtualRealityAspect)

    void onEngineAboutToShutdown() Q_DECL_OVERRIDE;
    void registerBackendTypes();

    qint64 m_time;
    bool m_initialized;
//    QScopedPointer<Logic::Manager> m_manager;
//    QScopedPointer<Logic::Executor> m_executor;
//    QSharedPointer<Logic::CallbackJob> m_callbackJob;
};

} // namespace Qt3DLogic

QT_END_NAMESPACE

#endif // QT3DLOGIC_QLOGICASPECT_P_H
