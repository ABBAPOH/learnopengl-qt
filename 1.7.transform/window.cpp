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

    initializeGeometry();
    initializeShaders();
    initializeTextures();
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

    auto time = QTime::currentTime();
    QMatrix4x4 transform;
    transform.translate({0.5, -0.5, 0.0});
    transform.rotate(6 * ((time.second() * 1000 + time.msec()) / 25 % 240), {0.0, 0.0, 1.0});

    m_funcs->glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    m_funcs->glClear(GL_COLOR_BUFFER_BIT);

    m_program->bind();

    m_funcs->glActiveTexture(GL_TEXTURE0);
    m_texture1->bind();
    m_program->setUniformValue("ourTexture1", 0);

    m_funcs->glActiveTexture(GL_TEXTURE1);
    m_texture2->bind();
    m_program->setUniformValue("ourTexture2", 1);

    m_program->setUniformValue("transform", transform);

    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_funcs->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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
        // Позиции          // Цвета             // Текстурные координаты
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // Верхний правый
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // Нижний правый
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // Нижний левый
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // Верхний левый
    };

    GLuint indices[] = {  // Помните, что мы начинаем с 0!
                          0, 1, 3,   // Первый треугольник
                          1, 2, 3    // Второй треугольник
                       };

    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    m_vbo.create();
    m_vbo.bind();
    m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vbo.allocate(vertices, sizeof(vertices));

    m_ibo.create();
    m_ibo.bind();
    m_ibo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_ibo.allocate(indices, sizeof(indices));

    // Атрибут с координатами
    m_funcs->glEnableVertexAttribArray(0);
    m_funcs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), nullptr);

    // Атрибут с цветом
    m_funcs->glEnableVertexAttribArray(1);
    m_funcs->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3* sizeof(GLfloat)));

    // Атрибут с текстурой
    m_funcs->glEnableVertexAttribArray(2);
    m_funcs->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
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

