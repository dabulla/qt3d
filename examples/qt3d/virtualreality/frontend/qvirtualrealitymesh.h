#ifndef QVIRTUALREALITYMESH_H
#define QVIRTUALREALITYMESH_H

#include <qt3dvr_global.h>
#include <Qt3DRender/qgeometryrenderer.h>

QT_BEGIN_NAMESPACE

namespace Qt3DVirtualReality {

class QVirtualRealityMesh : public Qt3DRender::QGeometryRenderer
{
public:
    QVirtualRealityMesh();
};

} // namespace Qt3DVirtualReality

QT_END_NAMESPACE

#endif // QVIRTUALREALITYMESH_H
