#ifndef WINDOW_H
#define WINDOW_H

#include <QOpenGLBuffer>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWindow>

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
    void toggleFullScreen();
    void initializeCubeGeometry();
    void initializeLampGeometry();
    void initializeShaders();
    void initializeTextures();
    void paintCube();
    void paintLamps();

private:
    QOpenGLFunctions_3_3_Core *m_funcs {nullptr};
    std::unique_ptr<Camera> m_camera;
    QOpenGLBuffer m_vbo {QOpenGLBuffer::VertexBuffer};
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_lampVbo {QOpenGLBuffer::VertexBuffer};
    QOpenGLVertexArrayObject m_lampVao;
//    QVector3D m_lightPos {1.2f, 1.0f, 2.0f};
    QVector3D m_lightPositions[4] = {
        { 0.7f,  0.2f,  2.0f},
        { 2.3f, -3.3f, -4.0f},
        {-4.0f,  2.0f, -12.0f},
        { 0.0f,  0.0f, -3.0f}
    };
    std::unique_ptr<QOpenGLShaderProgram> m_program;
    std::unique_ptr<QOpenGLShaderProgram> m_lampProgram;
    std::unique_ptr<QOpenGLTexture> m_texture;
    std::unique_ptr<QOpenGLTexture> m_textureSpecular;
};

#endif // WINDOW_H
