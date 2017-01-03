#include "qvrcamera.h"
#include <Qt3DCore/QTransform>

QVRCamera::QVRCamera(QNode *parent)
    :m_leftCamera(new Qt3DCore::QEntity(parent)),
     m_rightCamera(new Qt3DCore::QEntity(parent)), // TODO: Delete?
     m_leftCameraLens(new Qt3DRender::QCameraLens(parent)),//m_leftCamera)),
     m_rightCameraLens(new Qt3DRender::QCameraLens(parent)),//m_rightCamera)),
     m_leftTransform(new Qt3DCore::QTransform(parent)),//m_leftCamera)),
     m_rightTransform(new Qt3DCore::QTransform(parent))//m_rightCamera))
{
    m_leftCamera->addComponent(m_leftCameraLens);
    m_rightCamera->addComponent(m_rightCameraLens);
    m_leftCamera->addComponent(m_leftTransform);
    m_rightCamera->addComponent(m_rightTransform);

//    m_leftCamera->setAspectRatio(9.0/16.0);
//    m_rightCamera->setAspectRatio(9.0/16.0);
//    m_leftCamera->setNearPlane(0.1);
//    m_rightCamera->setNearPlane(0.1);
//    m_leftCamera->setFarPlane(1000.0f);
//    m_rightCamera->setFarPlane(1000.0f);
//    m_leftCamera->setUpVector(QVector3D(0.0f,1.0f,0.0f));
//    m_rightCamera->setUpVector(QVector3D(0.0f,1.0f,0.0f));
//    m_leftCamera->setFieldOfView(100.0);
//    m_rightCamera->setFieldOfView(100.0);
//    //m_leftCamera->set
    Q_EMIT leftCameraChanged(m_leftCamera);
    Q_EMIT rightCameraChanged(m_rightCamera);
}

Qt3DCore::QEntity *QVRCamera::leftCamera()
{
    return m_leftCamera;
}

Qt3DCore::QEntity *QVRCamera::rightCamera()
{
    return m_rightCamera;
}

void QVRCamera::setProjections(const QMatrix4x4 &leftProjection, const QMatrix4x4 &rightProjection)
{
    m_leftCameraLens->setProjectionMatrix(leftProjection);
    m_rightCameraLens->setProjectionMatrix(rightProjection);
}

void QVRCamera::update(const QMatrix4x4 &viewLeft, const QMatrix4x4 &viewRight)
{
//    Vector3f finalUp      = rot.Transform(Vector3f(0, 1, 0));
//    Vector3f finalForward = rot.Transform(Vector3f(0, 0, -1));
//    Matrix4f view         = Matrix4f::LookAtRH(pos, pos + finalForward, finalUp);
    //QMatrix4x4 viewLeft;
    //viewLeft.lookAt(leftPos, leftPos + leftOrient.rotatedVector(QVector3D(0.0,0.0,1.0)), leftOrient.rotatedVector(QVector3D(0.0,1.0,0.0)));
    m_leftTransform->setMatrix(viewLeft);
    //QMatrix4x4 viewRight;
    //viewRight.lookAt(rightPos, rightPos + rightOrient.rotatedVector(QVector3D(0.0,0.0,1.0)), rightOrient.rotatedVector(QVector3D(0.0,1.0,0.0)));
    m_rightTransform->setMatrix(viewRight);


//    Matrix4f   view = Matrix4f(orientation.Inverted()) * Matrix4f::Translation(-WorldEyePos);
//    QMatrix4x4 matL1, matL2;
//    matL1.rotate(leftOrient.inverted());
//    matL2.translate(-leftPos);
//    QMatrix4x4 matR1, matR2;
//    matR1.rotate(rightOrient.inverted());
//    matR2.translate(-rightPos);
//    m_leftTransform->setMatrix(matL1*matL2);
//    m_rightTransform->setMatrix(matR1*matR2);

//    m_leftTransform->setMatrix(leftOrient.inverted()*-leftPos);
//    m_leftTransform->setMatrix(leftOrient.inverted()*-leftPos);
//    m_leftTransform->setRotation(leftOrient.inverted());
//    m_leftTransform->setTranslation(-leftPos);
//    m_rightTransform->setRotation(rightOrient.inverted());
//    m_rightTransform->setTranslation(-rightPos);
}

float QVRCamera::disparity() const
{
    return m_disparity;
}

//QMatrix4x4 QVRCamera::headPosPredicted() const
//{
//    return m_headPosPredicted;
//}

//QMatrix4x4 QVRCamera::headOrientationPredicted() const
//{
//    return m_headOrientationPredicted;
//}

QVector3D QVRCamera::offset() const
{
    return m_offset;
}

//QMatrix4x4 QVRCamera::viewMatrixPredicted() const
//{
//    return m_viewMatrixPredicted;
//}

float QVRCamera::playerHeight() const
{
    return m_playerHeight;
}

QRectF QVRCamera::leftNormalizedViewportRect() const
{
    return m_leftNormalizedViewportRect;
}

QRectF QVRCamera::rightNormalizedViewportRect() const
{
    return m_rightNormalizedViewportRect;
}

QQuaternion QVRCamera::offsetOrientation() const
{
    return m_offsetOrientation;
}

void QVRCamera::setOffset(QVector3D offset)
{
    if (m_offset == offset)
        return;

    m_offset = offset;
    Q_EMIT offsetChanged(offset);
}

void QVRCamera::setLeftNormalizedViewportRect(QRectF leftNormalizedViewportRect)
{
    if (m_leftNormalizedViewportRect == leftNormalizedViewportRect)
        return;

    m_leftNormalizedViewportRect = leftNormalizedViewportRect;
    Q_EMIT leftNormalizedViewportRectChanged(leftNormalizedViewportRect);
}

void QVRCamera::setRightNormalizedViewportRect(QRectF rightNormalizedViewportRect)
{
    if (m_rightNormalizedViewportRect == rightNormalizedViewportRect)
        return;

    m_rightNormalizedViewportRect = rightNormalizedViewportRect;
    Q_EMIT rightNormalizedViewportRectChanged(rightNormalizedViewportRect);
}

void QVRCamera::setOffsetOrientation(QQuaternion offsetOrientation)
{
    if (m_offsetOrientation == offsetOrientation)
        return;

    m_offsetOrientation = offsetOrientation;
    Q_EMIT offsetOrientationChanged(offsetOrientation);
}
