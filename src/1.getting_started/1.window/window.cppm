module;

#include <QtOpenGL/QOpenGLWindow>
#include <QtOpenGL/QOpenGLFunctions_3_3_Core>

export module window;

export class Window : public QOpenGLWindow
{
    Q_OBJECT
public:
    Window();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    QOpenGLFunctions_3_3_Core *m_funcs {nullptr};
};
