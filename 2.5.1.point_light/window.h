#ifndef WINDOW_H
#define WINDOW_H

#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLFunctions_3_3_Core>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLWindow>

#include <memory>

class Camera;

class Window : public QOpenGLWindow
{
public:
    Window();
    ~Window() override;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void initializeCubeGeometry();
    void initializeLampGeometry();
    void initializeShaders();
    void initializeTextures();
    void paintCube();
    void paintLamp();

private:
    QOpenGLFunctions_3_3_Core *m_funcs {nullptr};
    std::unique_ptr<Camera> m_camera;
    QOpenGLBuffer m_vbo {QOpenGLBuffer::VertexBuffer};
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_lampVbo {QOpenGLBuffer::VertexBuffer};
    QOpenGLVertexArrayObject m_lampVao;
    QVector3D m_lightPos {1.2f, 1.0f, 2.0f};
    std::unique_ptr<QOpenGLShaderProgram> m_program;
    std::unique_ptr<QOpenGLShaderProgram> m_lampProgram;
    std::unique_ptr<QOpenGLTexture> m_texture;
    std::unique_ptr<QOpenGLTexture> m_textureSpecular;
};

#endif // WINDOW_H
