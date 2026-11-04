#include "window.h"

#include <QtOpenGL/QOpenGLVersionFunctionsFactory>

#include <QtGui/QKeyEvent>
#include <QtCore/QDebug>

Window::Window()
{
    resize(640, 480);
}

void Window::initializeGL()
{
    if (!context()) {
        qCritical() << "Can't get OGL context";
        close();
        return;
    }

    m_funcs = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_3_Core>(context());
    if (!m_funcs) {
        qCritical() << "Can't get OGL 3.3";
        close();
        return;
    }
    m_funcs->initializeOpenGLFunctions();

    qInfo() << "real OGL version" << reinterpret_cast<const char *>(m_funcs->glGetString(GL_VERSION));
}

void Window::resizeGL(int w, int h)
{
    if (!m_funcs)
        return;

    m_funcs->glViewport(0, 0, w, h);
}

void Window::paintGL()
{
    if (!m_funcs)
        return;

    m_funcs->glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    m_funcs->glClear(GL_COLOR_BUFFER_BIT);
}

void Window::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    }
    QOpenGLWindow::keyPressEvent(event);
}
