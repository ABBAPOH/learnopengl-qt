#ifndef WINDOW_H
#define WINDOW_H

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWindow>

class Window : public QOpenGLWindow
{
public:
    Window();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    QOpenGLFunctions_3_3_Core *m_funcs {nullptr};
};

#endif // WINDOW_H
