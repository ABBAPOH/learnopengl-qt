#ifndef WINDOW_H
#define WINDOW_H

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLWindow>

#include <memory>

class Camera;
class Model;

class Window : public QOpenGLWindow
{
public:
    Window();
    ~Window() override;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void toggleFullScreen();
    void initializeShaders();
    void initializeModel();

private:
    QOpenGLFunctions_3_3_Core *m_funcs {nullptr};
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<QOpenGLShaderProgram> m_program;
    std::unique_ptr<Model> m_model;
};

#endif // WINDOW_H
