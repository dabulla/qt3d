#include "qvirtualrealityaspect.h"
#include "qvirtualrealityaspect_p.h"

using namespace Qt3DCore;

namespace Qt3DVirtualReality {

/*!
  Constructs a new QVirtualRealityAspect instance with \a parent.
*/
QVirtualRealityAspect::QVirtualRealityAspect(QObject *parent)
    : QVirtualRealityAspect(*new QVirtualRealityAspectPrivate(), parent) {}

QVirtualRealityAspect::~QVirtualRealityAspect()
{

}

QVariant QVirtualRealityAspect::executeCommand(const QStringList &args)
{
    return QVariant();
}

QVector<Qt3DCore::QAspectJobPtr> QVirtualRealityAspect::jobsToExecute(qint64 time)
{
    return QVector<Qt3DCore::QAspectJobPtr>();
}

void QVirtualRealityAspect::onRegistered()
{
}

void QVirtualRealityAspect::onUnregistered()
{
}

QVirtualRealityAspect::QVirtualRealityAspect(QVirtualRealityAspectPrivate &dd, QObject *parent)
    : QAbstractAspect(dd, parent)
{

}

void QVirtualRealityAspectPrivate::registerBackendTypes()
{
    Q_Q(QVirtualRealityAspect);
    //q->registerBackendType<QQueryTrackedObjectsJob>();

}

Qt3DVirtualReality::QVirtualRealityAspectPrivate::QVirtualRealityAspectPrivate()
{

}

void QVirtualRealityAspectPrivate::onEngineAboutToShutdown()
{

}

} // namespace Qt3DVirtualReality

QT_END_NAMESPACE

QT3D_REGISTER_NAMESPACED_ASPECT("virtualreality", QT_PREPEND_NAMESPACE(Qt3DVirtualReality), QVirtualRealityAspect)

