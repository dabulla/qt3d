#ifndef QT3DHEADMOUNTEDDISPLAY_H
#define QT3DHEADMOUNTEDDISPLAY_H

#include "qvirtualrealityapi.h"

#include <QOffscreenSurface>
#include <QtCore/qpointer.h>
#include <QScopedPointer>
#include <QUrl>

QT_BEGIN_NAMESPACE

class QQmlIncubationController;

namespace Qt3DCore {
class QAbstractAspect;
namespace Quick {
class QQmlAspectEngine;
}
}

namespace Qt3DRender {
class QRenderAspect;
class QCamera;
}

namespace Qt3DInput {
class QInputAspect;
}

namespace Qt3DLogic {
class QLogicAspect;
}

namespace Qt3DVirtualReality {

class QVirtualRealityApiBackend;

class QHeadMountedDisplay : public QOffscreenSurface {
    Q_OBJECT

public:
    QHeadMountedDisplay(int hmdId, const QHeadMountedDisplayFormat &formathmd, QVirtualRealityApi *api, QVirtualRealityApiBackend *apibackend);
    ~QHeadMountedDisplay();

    void registerAspect(Qt3DCore::QAbstractAspect *aspect);
    void registerAspect(const QString &name);

    void setSource(const QUrl &source);
    Qt3DCore::Quick::QQmlAspectEngine *engine() const;

    // Hmd specific
    qreal refreshRate();
    qreal superSamplingFactor();
    QSize size();

private:
    void onSceneCreated(QObject *rootObject);
    void setWindowSurface(QObject *rootObject);

    QScopedPointer<Qt3DCore::Quick::QQmlAspectEngine> m_engine;

    // Aspects
    Qt3DRender::QRenderAspect *m_renderAspect;
    Qt3DInput::QInputAspect *m_inputAspect;
    Qt3DLogic::QLogicAspect *m_logicAspect;

    QUrl m_source;
    bool m_initialized;
    QPointer<Qt3DRender::QCamera> m_camera;
    QQmlIncubationController *m_incubationController;
    QVirtualRealityApi *m_api;
    QVirtualRealityApiBackend *m_apibackend;
    int m_hmdId;
};

} // Qt3DVirtualReality

QT_END_NAMESPACE

#endif
