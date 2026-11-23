#ifndef WINDOW_H
#define WINDOW_H

#include <camerawindow.h>

#include <QVector3D>

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>

#include <memory>
#include <vector>

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
    QOpenGLBuffer m_vbo {QOpenGLBuffer::VertexBuffer};
    QOpenGLVertexArrayObject m_vao;
    std::unique_ptr<QOpenGLShaderProgram> m_program;
    std::unique_ptr<QOpenGLTexture> m_texture1;
    std::unique_ptr<QOpenGLTexture> m_texture2;
    std::vector<QVector3D> m_cubePositions;
};

#endif // WINDOW_H
