#include "basewindow.h"

#include <QtOpenGL/QOpenGLVersionFunctionsFactory>

#include <QtGui/QKeyEvent>

#include <QtCore/QDebug>

BaseWindow::BaseWindow()
{
    resize(640, 480);
}

void BaseWindow::initializeGL()
{
    if (!context()) {
        qCritical() << "Can't get OGL context";
        close();
        return;
    }

    m_funcs = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_3_Core>(context());
    if (!m_funcs) {
        qCritical() << "Can't get OGL 3.2";
        close();
        return;
    }
    m_funcs->initializeOpenGLFunctions();

    configureGL();
    initializeScene();
}

void BaseWindow::configureGL()
{
}

void BaseWindow::resizeGL(int w, int h)
{
    if (!m_funcs)
        return;

    m_funcs->glViewport(0, 0, w, h);
}

void BaseWindow::paintGL()
{
    if (!m_funcs)
        return;

    paintScene();
}

void BaseWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    }
    QOpenGLWindow::keyPressEvent(event);
}

void BaseWindow::toggleFullScreen()
{
    if (windowState() != Qt::WindowState::WindowFullScreen)
        showFullScreen();
    else
        showNormal();
}
