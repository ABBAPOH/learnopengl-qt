#ifndef WINDOW_H
#define WINDOW_H

#include <camerawindow.h>

#include <QOpenGLShaderProgram>

#include <memory>

class Model;

class Window : public CameraWindow
{
public:
    Window();
    ~Window() override;

protected:
    void initializeScene() override;
    void paintScene() override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void initializeShaders();
    void initializeModel();

private:
    std::unique_ptr<QOpenGLShaderProgram> m_program;
    std::unique_ptr<Model> m_model;
};

#endif // WINDOW_H
