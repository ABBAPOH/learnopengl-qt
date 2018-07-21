#ifndef WINDOW_H
#define WINDOW_H

#include <QtGui/QOpenGLWindow>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLTexture>

#include <QOpenGLFunctions_3_3_Core>

class Window : public QOpenGLWindow
{
public:
    Window();
    ~Window() override;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void timerEvent(QTimerEvent *event) override;

private:
    void initializeGeometry();
    void initializeShaders();
    void initializeTextures();
    void initializeMatrixes();

private:
    QOpenGLFunctions_3_3_Core *m_funcs {nullptr};
    QOpenGLBuffer m_vbo {QOpenGLBuffer::VertexBuffer};
    QOpenGLVertexArrayObject m_vao;
    std::unique_ptr<QOpenGLShaderProgram> m_program;
    std::unique_ptr<QOpenGLTexture> m_texture1;
    std::unique_ptr<QOpenGLTexture> m_texture2;
    int m_timer {0};

    QMatrix4x4 m_view;
    QMatrix4x4 m_projection;

    std::vector<QVector3D> m_cubePositions;
};

#endif // WINDOW_H
