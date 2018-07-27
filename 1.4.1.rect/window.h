#ifndef WINDOW_H
#define WINDOW_H

#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLFunctions_3_3_Core>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLWindow>

#include <memory>

class Window : public QOpenGLWindow
{
public:
    Window();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    void initializeGeometry();
    void initializeShaders();

private:
    QOpenGLFunctions_3_3_Core *m_funcs {nullptr};
    QOpenGLBuffer m_vbo {QOpenGLBuffer::VertexBuffer};
    QOpenGLBuffer m_ibo {QOpenGLBuffer::IndexBuffer};
    QOpenGLVertexArrayObject m_vao;
    std::unique_ptr<QOpenGLShaderProgram> m_program;
};

#endif // WINDOW_H
