#include "qvirtualrealityapi.h"
#include "qheadmounteddisplay.h"
#include "qvirtualrealityapi_p.h"

Qt3DVirtualReality::QVirtualRealityApi::QVirtualRealityApi()
    : Qt3DVirtualReality::QVirtualRealityApi(*new Qt3DVirtualReality::QVirtualRealityApiPrivate)
{

}

Qt3DVirtualReality::QVirtualRealityApi::QVirtualRealityApi(Qt3DVirtualReality::QVirtualRealityApi::Type type)
    : QVirtualRealityApi()
{
    Q_D(QVirtualRealityApi);
    d->setType(type);
}

Qt3DVirtualReality::QVirtualRealityApi::~QVirtualRealityApi()
{

}

bool Qt3DVirtualReality::QVirtualRealityApi::isRuntimeInstalled(Qt3DVirtualReality::QVirtualRealityApi::Type type)
{
    return true;
}

Qt3DVirtualReality::QHeadMountedDisplay* Qt3DVirtualReality::QVirtualRealityApi::getHmd(int hmdId, const QHeadMountedDisplayFormat &format)
{
    Q_D(const QVirtualRealityApi);
    return new Qt3DVirtualReality::QHeadMountedDisplay(hmdId, format, this, d->m_apibackend);
}

Qt3DVirtualReality::QVirtualRealityApi::QVirtualRealityApi(Qt3DVirtualReality::QVirtualRealityApiPrivate &dd, QObject *parent)
    : QObject(dd, parent)
{

}
