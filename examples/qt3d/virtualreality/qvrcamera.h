#ifndef QVRCAMERA
#define QVRCAMERA
#include <QObject>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCameraLens>

class QVRCamera : public Qt3DCore::QEntity
{
    Q_OBJECT
    Q_PROPERTY(Qt3DCore::QEntity * leftCamera READ leftCamera NOTIFY leftCameraChanged)
    Q_PROPERTY(Qt3DCore::QEntity * rightCamera READ rightCamera NOTIFY rightCameraChanged)
    Q_PROPERTY(float disparity READ disparity NOTIFY disparityChanged)
//    Q_PROPERTY(QVector3D headPosPredicted READ headPosPredicted NOTIFY headPosPredictedChanged)
//    Q_PROPERTY(QQuaternion headOrientationPredicted READ headOrientationPredicted NOTIFY headOrientationPredictedChanged)
    Q_PROPERTY(QVector3D offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(QQuaternion offsetOrientation READ offsetOrientation WRITE setOffsetOrientation NOTIFY offsetOrientationChanged)
//    Q_PROPERTY(QMatrix4x4 viewMatrixPredicted READ viewMatrixPredicted NOTIFY viewMatrixPredictedChanged)
    Q_PROPERTY(float playerHeight READ playerHeight NOTIFY playerHeightChanged)
    Q_PROPERTY(QRectF leftNormalizedViewportRect READ leftNormalizedViewportRect WRITE setLeftNormalizedViewportRect NOTIFY leftNormalizedViewportRectChanged)
    Q_PROPERTY(QRectF rightNormalizedViewportRect READ rightNormalizedViewportRect WRITE setRightNormalizedViewportRect NOTIFY rightNormalizedViewportRectChanged)
public:
    QVRCamera(QNode *parent = nullptr);

    Qt3DCore::QEntity * leftCamera();
    Qt3DCore::QEntity * rightCamera();

    void setProjections(const QMatrix4x4& leftProjection, const QMatrix4x4& rightProjection);
    void update(const QMatrix4x4 &viewLeft, const QMatrix4x4 &viewRight);
    float disparity() const;

//    QMatrix4x4 headPosPredicted() const;
//    QMatrix4x4 headOrientationPredicted() const;
    QVector3D offset() const;
//    QMatrix4x4 viewMatrixPredicted() const;
    float playerHeight() const;
    QRectF leftNormalizedViewportRect() const;
    QRectF rightNormalizedViewportRect() const;
    QQuaternion offsetOrientation() const;

public Q_SLOTS:

    void setOffset(QVector3D offset);
    void setLeftNormalizedViewportRect(QRectF leftNormalizedViewportRect);
    void setRightNormalizedViewportRect(QRectF rightNormalizedViewportRect);
    void setOffsetOrientation(QQuaternion offsetOrientation);

Q_SIGNALS:

    void disparityChanged(float disparity);
//    void headPosPredictedChanged(QVector3D headPosPredicted);
//    void headOrientationPredictedChanged(QMatrix4x4 headOrientationPredicted);
    void offsetChanged(QVector3D offset);
//    void viewMatrixPredictedChanged(QMatrix4x4 viewMatrixPredicted);
    void playerHeightChanged(float playerHeight);
    void leftNormalizedViewportRectChanged(QRectF leftNormalizedViewportRect);
    void rightNormalizedViewportRectChanged(QRectF rightNormalizedViewportRect);
    void leftCameraChanged(Qt3DCore::QEntity * leftCamera);
    void rightCameraChanged(Qt3DCore::QEntity * rightCamera);
    void offsetOrientationChanged(QQuaternion offsetOrientation);

private:
    Qt3DRender::QCameraLens *m_leftCameraLens;
    Qt3DRender::QCameraLens *m_rightCameraLens;
    Qt3DCore::QTransform *m_leftTransform;
    Qt3DCore::QTransform *m_rightTransform;
    Qt3DCore::QEntity *m_leftCamera;
    Qt3DCore::QEntity *m_rightCamera;
    float m_disparity;
//    QVector3D m_headPosPredicted;
//    QQuaternion m_headOrientationPredicted;
    QVector3D m_offset;
//    QMatrix4x4 m_viewMatrixPredicted;
    float m_playerHeight;
    QRectF m_leftNormalizedViewportRect;
    QRectF m_rightNormalizedViewportRect;
    QQuaternion m_offsetOrientation;
};
#endif
