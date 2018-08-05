#include "window.h"

#include <QDebug>
#include <QCoreApplication>
#include <QTime>

Window::Window()
{
    resize(640, 480);

    m_timer = startTimer(40);
}

Window::~Window()
{
    makeCurrent();
    m_texture1.reset();
    m_texture2.reset();
    doneCurrent();
}

void Window::initializeGL()
{
    if (!context()) {
        qCritical() << "Can't get OGL context";
        close();
        return;
    }

    m_funcs = context()->versionFunctions<QOpenGLFunctions_3_3_Core>();
    if (!m_funcs) {
        qCritical() << "Can't get OGL 3.2";
        close();
        return;
    }

    m_funcs->initializeOpenGLFunctions();
    m_funcs->glEnable(GL_DEPTH_TEST);

    initializeGeometry();
    initializeShaders();
    initializeTextures();
    initializeMatrixes();
}

void Window::resizeGL(int w, int h)
{
    if (!m_funcs)
        return;

    m_funcs->glViewport(0, 0, w, h);
}

void Window::paintGL()
{
    if (!m_funcs)
        return;

    m_funcs->glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    m_funcs->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_program->bind();

    m_funcs->glActiveTexture(GL_TEXTURE0);
    m_texture1->bind();
    m_program->setUniformValue("ourTexture1", 0);

    m_funcs->glActiveTexture(GL_TEXTURE1);
    m_texture2->bind();
    m_program->setUniformValue("ourTexture2", 1);

    for (int i = 0; i < 10; i++) {
        QMatrix4x4 model;
        model.translate(m_cubePositions[i]);
        model.rotate(20.0 * i, {1.0f, 0.3f, 0.5f});

        m_program->setUniformValue("model", model);
        m_program->setUniformValue("view", m_view);
        m_program->setUniformValue("projection", m_projection);

        QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
        m_funcs->glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // release resources
    m_program->release();
    m_texture2->release();
    m_funcs->glActiveTexture(GL_TEXTURE0);
    m_texture1->release();
}

void Window::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_timer) {
        update();
    }
    QOpenGLWindow::timerEvent(event);
}

void Window::initializeGeometry()
{
    // setup vertex data

    GLfloat vertices[] = {
        // Positions            // Texture coords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    m_vbo.create();
    m_vbo.bind();
    m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vbo.allocate(vertices, sizeof(vertices));

    // Vertexes attribute
    m_funcs->glEnableVertexAttribArray(0);
    m_funcs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);

    // Texture attribute
    m_funcs->glEnableVertexAttribArray(1);
    m_funcs->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(3 * sizeof(GLfloat)));
    m_vbo.release();

    m_cubePositions = {
        {0.0f,  0.0f,  0.0f},
        {2.0f,  5.0f, -15.0f},
        {-1.5f, -2.2f, -2.5f},
        {-3.8f, -2.0f, -12.3f},
        {2.4f, -0.4f, -3.5f},
        {-1.7f,  3.0f, -7.5f},
        {1.3f, -2.0f, -2.5f},
        {1.5f,  2.0f, -2.5f},
        {1.5f,  0.2f, -1.5f},
        {-1.3f,  1.0f, -1.5f}
    };
}

void Window::initializeShaders()
{
    m_program = std::make_unique<QOpenGLShaderProgram>();
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, QStringLiteral(":/vshader.glsl"));
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, QStringLiteral(":/fshader.glsl"));
    m_program->link();
}

void Window::initializeTextures()
{
    m_texture1 = std::make_unique<QOpenGLTexture>(QImage(":/container.jpg"));
    m_texture2 = std::make_unique<QOpenGLTexture>(QImage(":/awesomeface.png"));
}

void Window::initializeMatrixes()
{
    m_view.translate({0.0, 0.0, -3.0});
    m_projection.perspective(45.0, 1.0 * width() / height(), 0.1, 100.0);
}
