#ifndef WINDOW_H
#define WINDOW_H

#include <camerawindow.h>

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>

#include <memory>

class Window : public CameraWindow
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

private:
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
