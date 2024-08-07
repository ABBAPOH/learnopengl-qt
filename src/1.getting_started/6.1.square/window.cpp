#include "window.h"

#if QT_VERSION >= 0x060000
#include <QtOpenGL/QOpenGLVersionFunctionsFactory>
#endif

#include <QtGui/QKeyEvent>
#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>
#include <QtCore/QTime>

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

#if QT_VERSION >= 0x060000
    m_funcs = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_3_Core>(context());
#else
    m_funcs = context()->versionFunctions<QOpenGLFunctions_3_3_Core>();
#endif
    if (!m_funcs) {
        qCritical() << "Can't get OGL 3.2";
        close();
        return;
    }

    m_funcs->initializeOpenGLFunctions();

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
    m_funcs->glClear(GL_COLOR_BUFFER_BIT);

    m_program->bind();

    m_funcs->glActiveTexture(GL_TEXTURE0);
    m_texture1->bind();
    m_program->setUniformValue("ourTexture1", 0);

    m_funcs->glActiveTexture(GL_TEXTURE1);
    m_texture2->bind();
    m_program->setUniformValue("ourTexture2", 1);

    m_program->setUniformValue("model", m_model);
    m_program->setUniformValue("view", m_view);
    m_program->setUniformValue("projection", m_projection);

    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_funcs->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // release resources
    m_program->release();
    m_texture2->release();
    m_funcs->glActiveTexture(GL_TEXTURE0);
    m_texture1->release();
}

void Window::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    }
    QOpenGLWindow::keyPressEvent(event);
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
        // Positions          // Texture coords
        0.5f,  0.5f, 0.0f,  1.0f, 1.0f,   // Top right
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f,   // Bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,   // Bottom left
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f    // Top left
    };

    GLuint indices[] = {
        // Zero-based indexation
        0, 1, 3,   // First triangle
        1, 2, 3    // Second triangle
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

    // Vertexes attribute
    m_funcs->glEnableVertexAttribArray(0);
    m_funcs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);

    // Texture attribute
    m_funcs->glEnableVertexAttribArray(1);
    m_funcs->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(3 * sizeof(GLfloat)));
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
    m_model.rotate(-55.0, {1.0, 0.0, 0.0});
    m_view.translate({0.0, 0.0, -3.0});
    m_projection.perspective(45.0, 1.0 * width() / height(), 0.1, 100.0);
}
