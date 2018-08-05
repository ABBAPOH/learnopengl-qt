#include "camera.h"

#include <QtGui/QCursor>
#include <QtGui/QKeyEvent>
#include <QtGui/QWindow>

#include <QtCore/QDebug>

#include <cmath>

constexpr float radians(float angle)
{
    return angle * float(M_PI) / 180.0;
}

Camera::Camera(QObjectPointer parent)
    : QObject(parent)
{
    m_timer = startTimer(25);
}

void Camera::setWindow(QWindowPointer window)
{
    if (window == m_window) {
        return;
    }

    if (m_window) {
        m_window->setCursor(QCursor());
        m_window->removeEventFilter(this);
    }

    m_window = window;

    if (m_window) {
        QCursor cursor;
        cursor.setShape(Qt::BlankCursor);
        m_window->setCursor(cursor);
        m_window->installEventFilter(this);
        updateMatrixes();
    }

    emit windowChanged(m_window);
}

void Camera::setCameraSpeed(float speed)
{
    if (qFuzzyCompare(m_cameraSpeed, speed)) {
        return;
    }

    m_cameraSpeed = speed;
    emit cameraSpeedChanged(speed);
}

void Camera::setSensitivity(float sensitivity)
{
    if (qFuzzyCompare(m_sensitivity, sensitivity)) {
        return;
    }

    m_sensitivity = sensitivity;
    emit sensitivityChanged(sensitivity);
}

bool Camera::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_window) {
        switch (event->type()) {
        case QEvent::KeyPress:
            keyPressEvent(static_cast<QKeyEvent *>(event));
            break;
        case QEvent::KeyRelease:
            keyReleaseEvent(static_cast<QKeyEvent *>(event));
            break;
        case QEvent::FocusIn:
            focusInEvent(static_cast<QFocusEvent *>(event));
            break;
        case QEvent::FocusOut:
            focusOutEvent(static_cast<QFocusEvent *>(event));
            break;
        case QEvent::MouseMove:
            mouseMoveEvent(static_cast<QMouseEvent *>(event));
            break;
        case QEvent::Wheel:
            wheelEvent(static_cast<QWheelEvent *>(event));
            break;
        default:
            break;
        }
    }
    return false;
}

void Camera::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_timer) {
        updateMatrixes();
        if (m_window) {
            m_window->requestUpdate();
        }
    }
    QObject::timerEvent(event);
}

void Camera::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_W) {
        m_keyPressed[Forward] = true;
    } else if (event->key() == Qt::Key_S) {
        m_keyPressed[Backward] = true;
    } else if (event->key() == Qt::Key_A) {
        m_keyPressed[Left] = true;
    } else if (event->key() == Qt::Key_D) {
        m_keyPressed[Right] = true;
    } else if (event->key() == Qt::Key_Space) {
        m_keyPressed[Up] = true;
    } else if (event->key() == Qt::Key_C) {
        m_keyPressed[Down] = true;
    }
}

void Camera::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_W) {
        m_keyPressed[Forward] = false;
    } else if (event->key() == Qt::Key_S) {
        m_keyPressed[Backward] = false;
    } else if (event->key() == Qt::Key_A) {
        m_keyPressed[Left] = false;
    } else if (event->key() == Qt::Key_D) {
        m_keyPressed[Right] = false;
    } else if (event->key() == Qt::Key_Space) {
        m_keyPressed[Up] = false;
    } else if (event->key() == Qt::Key_C) {
        m_keyPressed[Down] = false;
    }
}

void Camera::focusInEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    if (m_window) {
        m_window->setMouseGrabEnabled(true);
        m_blockMove = true;
        QCursor::setPos(m_window->geometry().center());
    }
}

void Camera::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    if (m_window) {
        m_window->setMouseGrabEnabled(false);
    }
}

void Camera::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_blockMove) {
        if (!m_window) {
            return;
        }

        const auto size = m_window->geometry().size();
        const auto center = QPointF(size.width() / 2.0, size.height() / 2.0);
        const auto delta = event->localPos() - center;
        const auto deltax = delta.x() * m_sensitivity;
        const auto deltay = delta.y() * m_sensitivity;
        m_yaw += deltax;
        m_pitch = qBound(-89.0, m_pitch - deltay, 89.0);

        QVector3D front {
            cos(radians(m_pitch)) * cos(radians(m_yaw)),
            sin(radians(m_pitch)),
            cos(radians(m_pitch)) * sin(radians(m_yaw))
        };
        m_cameraFront = front.normalized();

        m_blockMove = true;
        QCursor::setPos(m_window->geometry().center());
    } else {
        m_blockMove = false;
    }
}

void Camera::wheelEvent(QWheelEvent *event)
{
    const auto sensitivity = 0.1;
    m_fov = qBound(1.0, m_fov - event->delta() * sensitivity, 45.0);
    updateMatrixes();
}

void Camera::updateMatrixes()
{
    m_view = QMatrix4x4();
    m_projection = QMatrix4x4();

    if (!m_window) {
        return;
    }

    QVector3D cameraUp {0.0f, 1.0f,  0.0f};

    if (m_keyPressed[Forward]) {
        m_cameraPos += m_cameraSpeed * m_cameraFront;
    }
    if (m_keyPressed[Backward]) {
        m_cameraPos -= m_cameraSpeed * m_cameraFront;
    }
    if (m_keyPressed[Left]) {
        m_cameraPos -= QVector3D::crossProduct(m_cameraFront, cameraUp).normalized() * m_cameraSpeed;
    }
    if (m_keyPressed[Right]) {
        m_cameraPos += QVector3D::crossProduct(m_cameraFront, cameraUp).normalized() * m_cameraSpeed;
    }
    if (m_keyPressed[Up]) {
        m_cameraPos += m_cameraSpeed * cameraUp;
    }
    if (m_keyPressed[Down]) {
        m_cameraPos -= m_cameraSpeed * cameraUp;
    }

    m_view.lookAt(m_cameraPos, m_cameraPos + m_cameraFront, {0.0f, 1.0f, 0.0f});
    m_projection.perspective(m_fov, 1.0 * m_window->width() / m_window->height(), 0.1, 100.0);
}
