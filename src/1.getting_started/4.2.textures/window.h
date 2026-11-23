#ifndef WINDOW_H
#define WINDOW_H

#include <basewindow.h>

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>

#include <memory>

class Window : public BaseWindow
{
public:
    ~Window() override;

protected:
    void initializeScene() override;
    void paintScene() override;

private:
    void initializeGeometry();
    void initializeShaders();
    void initializeTextures();

    QOpenGLBuffer m_vbo {QOpenGLBuffer::VertexBuffer};
    QOpenGLBuffer m_ibo {QOpenGLBuffer::IndexBuffer};
    QOpenGLVertexArrayObject m_vao;
    std::unique_ptr<QOpenGLShaderProgram> m_program;
    std::unique_ptr<QOpenGLTexture> m_texture1;
    std::unique_ptr<QOpenGLTexture> m_texture2;
};

#endif // WINDOW_H
