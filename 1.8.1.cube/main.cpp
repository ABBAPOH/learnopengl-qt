#include <QGuiApplication>
#include <QTimer>

#include "window.h"

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);

    QSurfaceFormat fmt;
    // NOTE: default depth buffer size is -1
    fmt.setDepthBufferSize(24);
    fmt.setVersion(3, 2);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(fmt);

    Window w;
    QTimer::singleShot(0, &w, &Window::show);

    return a.exec();
}
