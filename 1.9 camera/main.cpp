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

    std::unique_ptr<QObject> p = std::make_unique<QObject>();


    Window w;
    w.show();

    return QCoreApplication::exec();
}
