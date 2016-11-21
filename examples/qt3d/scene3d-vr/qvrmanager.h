//#ifndef QVRMANAGER
//#define QVRMANAGER

//#include <QObject>
//#include <Qt3DRender/QRenderTarget>
//#include <Qt3DCore/QEntity>
//#include <Qt3DLogic/QFrameAction>
//#include <QQuickWindow>

//class VRManagerPrivate;

//class VRManager : public Qt3DCore::QEntity
//{
//    Q_OBJECT
//    Q_PROPERTY(Qt3DRender::QRenderTarget *target READ target NOTIFY targetChanged)
//    Q_PROPERTY(QQuickWindow *window READ window WRITE setWindow NOTIFY windowChanged)
//public:
//    VRManager(QObject *parent = NULL);
//    virtual ~VRManager();
//    Qt3DRender::QRenderTarget * target() const
//    {
//        return m_target;
//    }
//    QQuickWindow *window() const
//    {
//        return m_window;
//    }

//public slots:
//    void setWindow(QQuickWindow *window);

//signals:
//    void targetChanged(Qt3DRender::QRenderTarget * target);
//    void windowChanged(QQuickWindow *window);

//private Q_SLOTS:
//    void onTriggered(float dt);
//    void onFrame();
//private:
//    Qt3DRender::QRenderTarget * m_target;
//    Qt3DLogic::QFrameAction * m_frameAction;
//    class VRManagerPrivate * m_p;
//    QQuickWindow *m_window;
//};
//#endif
