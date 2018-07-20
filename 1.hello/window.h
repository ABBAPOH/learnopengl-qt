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

private:
    QOpenGLFunctions_3_3_Core *_funcs {nullptr};
    QOpenGLBuffer _vbo {QOpenGLBuffer::VertexBuffer};
    QOpenGLBuffer _ibo {QOpenGLBuffer::IndexBuffer};
    QOpenGLVertexArrayObject _vao;
    std::unique_ptr<QOpenGLShaderProgram> _program;
    int vertexColorLocation;
};

#endif // WINDOW_H
