#include "window.h"

#include "camera.h"
#include "model.h"

#if QT_VERSION >= 0x060000
#include <QtOpenGL/QOpenGLVersionFunctionsFactory>
#endif

#include <QtGui/QKeyEvent>

#include <QtCore/QDebug>

Window::Window()
    : m_camera(std::make_unique<Camera>())
{
    resize(800, 600);

    m_camera->setWindow(this);
}

Window::~Window()
{
    makeCurrent();
    m_model.reset();
    doneCurrent();
}

void Window::initializeGL()
{
    if (!context()) {
        qCritical() << "Can't get OGL context";
        close();
        return;
    }

#if QT_VERSION >= 0x060000
    m_funcs = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_3_Core>(context());
#else
    m_funcs = context()->versionFunctions<QOpenGLFunctions_3_3_Core>();
#endif
    if (!m_funcs) {
        qCritical() << "Can't get OGL 3.3";
        close();
        return;
    }

    m_funcs->initializeOpenGLFunctions();
    m_funcs->glEnable(GL_DEPTH_TEST);

    initializeShaders();
    initializeModel();
}

void Window::resizeGL(int w, int h)
{
    if (!m_funcs)
        return;

    m_funcs->glViewport(0, 0, w, h);
}

void Window::paintGL()
{
    if (!m_funcs || !m_model)
        return;

    m_funcs->glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    m_funcs->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_program->bind();

    m_program->setUniformValue("projection", m_camera->projection());
    m_program->setUniformValue("view", m_camera->view());

    QMatrix4x4 model;
    model.translate(0.0f, 0.0f, 0.0f);
    model.scale(1.0f);
    m_program->setUniformValue("model", model);

    m_model->draw(*m_program);

    m_program->release();
}

void Window::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    } else if (event->key() == Qt::Key_F) {
        toggleFullScreen();
    }

    QOpenGLWindow::keyPressEvent(event);
}

void Window::toggleFullScreen()
{
    if (windowState() != Qt::WindowState::WindowFullScreen)
        showFullScreen();
    else
        showNormal();
}

void Window::initializeShaders()
{
    m_program = std::make_unique<QOpenGLShaderProgram>();
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, QStringLiteral(":/vshader.glsl"));
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, QStringLiteral(":/fshader.glsl"));
    m_program->link();
}

void Window::initializeModel()
{
    m_model = std::make_unique<Model>(m_funcs, QStringLiteral(":/objects/backpack/backpack.obj"));
}
