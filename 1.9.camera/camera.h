#ifndef CAMERA_H
#define CAMERA_H

#include <QtCore/QObject>
#include <QtGui/QMatrix4x4>

class QWindow;
class QKeyEvent;
class QFocusEvent;
class QMouseEvent;
class QWheelEvent;

class Camera : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Camera)
    Q_PROPERTY(QWindowPointer window READ window WRITE setWindow NOTIFY windowChanged)
    Q_PROPERTY(float cameraSpeed READ cameraSpeed WRITE setCameraSpeed NOTIFY cameraSpeedChanged)
    Q_PROPERTY(float sensitivity READ sensitivity WRITE setSensitivity NOTIFY sensitivityChanged)

public:
    using QObjectPointer = QObject *; // should be std::observer_ptr<QObject>
    using QWindowPointer = QWindow *; // should be std::observer_ptr<QWindow>

    explicit Camera(QObjectPointer parent = nullptr);
    Camera(Camera &&) = delete;
    ~Camera() override = default;

    Camera &operator=(Camera &&) = delete;

    QWindowPointer window() const noexcept { return m_window; }
    void setWindow(QWindowPointer window);

    QMatrix4x4 view() const { return m_view; }
    QMatrix4x4 projection() const { return m_projection; }

    float cameraSpeed() const noexcept { return m_cameraSpeed; }
    void setCameraSpeed(float speed);

    float sensitivity() const noexcept { return m_sensitivity; }
    void setSensitivity(float sensitivity);

    QVector3D position() const noexcept { return m_cameraPos; }
    QVector3D front() const noexcept { return m_cameraFront; }

    bool eventFilter(QObject *watched, QEvent *event) override;

signals:
    void windowChanged(QWindowPointer window);
    void cameraSpeedChanged(float);
    void sensitivityChanged(float);

protected:
    void timerEvent(QTimerEvent *event) override;

private:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void updateMatrixes();

private:
    enum MoveKey {
        Forward,
        Backward,
        Left,
        Right,
        Up,
        Down,
        KeyCount
    };

    QWindowPointer m_window {nullptr};
    QMatrix4x4 m_view;
    QMatrix4x4 m_projection;
    float m_cameraSpeed = 0.05f;
    float m_sensitivity = 0.075f;

    int m_timer {0};

    bool m_keyPressed[KeyCount] {false, false, false, false};
    float m_yaw = -90.0f;
    float m_pitch = 0.0f;
    float m_fov {45.0};

    QVector3D m_cameraPos {0.0f, 0.0f,  3.0f};
    QVector3D m_cameraFront {0.0f, 0.0f, -1.0f};

    bool m_blockMove {false};
};

#endif // CAMERA_H
