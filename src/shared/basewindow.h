#ifndef BASEWINDOW_H
#define BASEWINDOW_H

#include <QtOpenGL/QOpenGLFunctions_3_3_Core>
#include <QtOpenGL/QOpenGLWindow>

class QKeyEvent;

class BaseWindow : public QOpenGLWindow
{
public:
    BaseWindow();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void keyPressEvent(QKeyEvent *event) override;

    virtual void configureGL();
    virtual void initializeScene() = 0;
    virtual void paintScene() = 0;

    void toggleFullScreen();

    QOpenGLFunctions_3_3_Core *m_funcs {nullptr};
};

#endif // BASEWINDOW_H
