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
    GLuint createTexture(QStringView path);

private:
    QOpenGLFunctions_3_3_Core *_funcs {nullptr};
    QOpenGLBuffer _vbo {QOpenGLBuffer::VertexBuffer};
    QOpenGLBuffer _ibo {QOpenGLBuffer::IndexBuffer};
    QOpenGLVertexArrayObject _vao;
    std::unique_ptr<QOpenGLShaderProgram> _program;
    GLuint _texture1 {0};
    GLuint _texture2 {0};
    int m_timer {0};
};

#endif // WINDOW_H