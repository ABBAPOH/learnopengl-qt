#ifndef WINDOW_H
#define WINDOW_H

#include <basewindow.h>

#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QVector3D>

#include <memory>
#include <vector>

class Window : public BaseWindow
{
public:
    Window();
    ~Window() override;

protected:
    void initializeScene() override;
    void paintScene() override;
    void timerEvent(QTimerEvent *event) override;

private:
    void initializeGeometry();
    void initializeShaders();
    void initializeTextures();
    void initializeMatrixes();

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
