#ifndef WINDOW_H
#define WINDOW_H

#include <camerawindow.h>

#include <QVector3D>

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#include <memory>

class Window : public CameraWindow
{
protected:
    void initializeScene() override;
    void paintScene() override;

private:
    void initializeCubeGeometry();
    void initializeLampGeometry();
    void initializeShaders();
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
};

#endif // WINDOW_H
