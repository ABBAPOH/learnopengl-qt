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
    void initializeGeometry();
    void initializeShaders();
    void initializeTextures();

private:
    QOpenGLFunctions_3_3_Core *m_funcs {nullptr};
    std::unique_ptr<Camera> m_camera;
    QOpenGLBuffer m_cubeVbo {QOpenGLBuffer::VertexBuffer};
    QOpenGLBuffer m_planeVbo {QOpenGLBuffer::VertexBuffer};
    QOpenGLBuffer m_grassVbo {QOpenGLBuffer::VertexBuffer};
    QOpenGLVertexArrayObject m_cubeVao;
    QOpenGLVertexArrayObject m_planeVao;
    QOpenGLVertexArrayObject m_grassVao;
    std::unique_ptr<QOpenGLShaderProgram> m_program;
    std::unique_ptr<QOpenGLTexture> m_cubeTexture;
    std::unique_ptr<QOpenGLTexture> m_floorTexture;
    std::unique_ptr<QOpenGLTexture> m_grassTexture;
};

#endif // WINDOW_H
