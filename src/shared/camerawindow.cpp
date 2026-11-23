#include "camerawindow.h"

CameraWindow::CameraWindow()
    : m_camera(std::make_unique<Camera>())
{
    m_camera->setWindow(this);
}

void CameraWindow::configureGL()
{
    m_funcs->glEnable(GL_DEPTH_TEST);
}
