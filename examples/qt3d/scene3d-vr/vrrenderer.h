#ifndef VRRENDERER_H
#define VRRENDERER_H

#include <QOpenGLFunctions>
#include <Qt3DRender/QCamera>

class QVRCamera;

class VrRenderer
{
public:
    VrRenderer();
    ~VrRenderer();

    void initialize();
    void beforeRender();
    void renderToHeadset();

    void setVrCamera(QVRCamera *cam);

    void sync();
    QSize getSize() const;
    GLuint getTexId() const;
private:
    class VrRendererPrivate * m_p;
};
#endif
