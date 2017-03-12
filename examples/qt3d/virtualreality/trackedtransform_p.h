#ifndef TRACKEDTRANSFORM_P_H
#define TRACKEDTRANSFORM_P_H

#include <Qt3DCore/qbackendnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DVirtualReality {

class TrackedTransform : public Qt3DCore::QBackendNode
{
public:
    TrackedTransform();
    
    // QBackendNode interface
protected:
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
    {
    }
    
private:
    void initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change)
    {
    }
};

} //namespace Qt3DVirtualReality

QT_END_NAMESPACE

#endif // TRACKEDTRANSFORM_P_H
