#ifndef WINDOW_H
#define WINDOW_H

#include <QtGui/QOpenGLWindow>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLShaderProgram>

#include <QOpenGLFunctions_3_3_Core>

class Window : public QOpenGLWindow
{
public:
    Window();

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
    GLuint createTexture(QStringView path);

private:
    QOpenGLFunctions_3_3_Core *m_funcs {nullptr};
    QOpenGLBuffer m_vbo {QOpenGLBuffer::VertexBuffer};
    QOpenGLBuffer m_ibo {QOpenGLBuffer::IndexBuffer};
    QOpenGLVertexArrayObject m_vao;
    std::unique_ptr<QOpenGLShaderProgram> m_program;
    GLuint m_texture1 {0};
    GLuint m_texture2 {0};
    int m_timer {0};

    QMatrix4x4 m_model;
    QMatrix4x4 m_view;
    QMatrix4x4 m_projection;
};

#endif // WINDOW_H
