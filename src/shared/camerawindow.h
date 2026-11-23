#ifndef CAMERAWINDOW_H
#define CAMERAWINDOW_H

#include "basewindow.h"
#include "camera.h"

#include <memory>

class CameraWindow : public BaseWindow
{
public:
    CameraWindow();

protected:
    void configureGL() override;

    std::unique_ptr<Camera> m_camera;
};

#endif // CAMERAWINDOW_H
