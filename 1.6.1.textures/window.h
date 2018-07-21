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

private:
    void initializeGeometry();
    void initializeShaders();
    void initializeTextures();

private:
    QOpenGLFunctions_3_3_Core *_funcs {nullptr};
    QOpenGLBuffer _vbo {QOpenGLBuffer::VertexBuffer};
    QOpenGLBuffer _ibo {QOpenGLBuffer::IndexBuffer};
    QOpenGLVertexArrayObject _vao;
    std::unique_ptr<QOpenGLShaderProgram> _program;
    std::unique_ptr<QOpenGLTexture> _texture1;
    std::unique_ptr<QOpenGLTexture> _texture2;
};

#endif // WINDOW_H
