#ifndef WINDOW_H
#define WINDOW_H

#include <camerawindow.h>

#include <QVector3D>

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
    void keyPressEvent(QKeyEvent *event) override;

private:
    void initializeCubeGeometry();
    void initializeLampGeometry();
    void initializeShaders();
    void initializeTextures();
    void paintCube();
    void paintLamp();

private:
    QOpenGLBuffer m_vbo {QOpenGLBuffer::VertexBuffer};
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_lampVbo {QOpenGLBuffer::VertexBuffer};
    QOpenGLVertexArrayObject m_lampVao;
    QVector3D m_lightPos {1.2f, 1.0f, 2.0f};
    std::unique_ptr<QOpenGLShaderProgram> m_program;
    std::unique_ptr<QOpenGLShaderProgram> m_lampProgram;
    std::unique_ptr<QOpenGLTexture> m_texture;
    std::unique_ptr<QOpenGLTexture> m_textureSpecular;
};

#endif // WINDOW_H
