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
    void timerEvent(QTimerEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void initializeGeometry();
    void initializeShaders();
    void initializeTextures();
    void updateMatrixes();

private:
    QOpenGLFunctions_3_3_Core *m_funcs {nullptr};
    QOpenGLBuffer m_vbo {QOpenGLBuffer::VertexBuffer};
    QOpenGLVertexArrayObject m_vao;
    std::unique_ptr<QOpenGLShaderProgram> m_program;
    std::unique_ptr<QOpenGLTexture> m_texture1;
    std::unique_ptr<QOpenGLTexture> m_texture2;
    int m_timer {0};

    enum MoveKey {
        Forward,
        Backward,
        Left,
        Right,
        KeyCount
    };

    bool keyPressed[KeyCount] {false, false, false, false};

    QVector3D m_cameraFront {0.0f, 0.0f, -1.0f};
    QVector3D m_cameraPos {0.0f, 0.0f,  3.0f};
    QMatrix4x4 m_view;
    QMatrix4x4 m_projection;

    std::vector<QVector3D> m_cubePositions;

    float m_yaw = -90.0f;
    float m_pitch = 0.0f;
    bool m_blockMove {false};
    float m_fov {45.0};
};

#endif // WINDOW_H
