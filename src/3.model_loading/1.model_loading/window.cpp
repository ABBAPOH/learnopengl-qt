#include "window.h"

#include "model.h"

#include <QtGui/QKeyEvent>

Window::Window()
{
    resize(800, 600);
}

Window::~Window()
{
    makeCurrent();
    m_model.reset();
    doneCurrent();
}

void Window::initializeScene()
{
    initializeShaders();
    initializeModel();
}

void Window::paintScene()
{
    if (!m_model)
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
    if (event->key() == Qt::Key_F)
        toggleFullScreen();
    BaseWindow::keyPressEvent(event);
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
