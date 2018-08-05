#include "window.h"

#include <QtCore/QDebug>

Window::Window()
{
    resize(640, 480);
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
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_funcs->glDrawArrays(GL_TRIANGLES, 0, 3);
    m_program->release();
}

void Window::initializeGeometry()
{
    // setup vertex data

    GLfloat vertices[] = {
        // Positions           // Colors
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // Bottom right corner
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // Bottom left corner
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // Top corner
    };

    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    m_vbo.create();
    m_vbo.bind();
    m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vbo.allocate(vertices, sizeof(vertices));

    // Vertexes attribute
    m_funcs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid *>0);
    m_funcs->glEnableVertexAttribArray(0);
    // Colors attribute
    m_funcs->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(3* sizeof(GLfloat)));
    m_funcs->glEnableVertexAttribArray(1);
}

void Window::initializeShaders()
{
    m_program = std::make_unique<QOpenGLShaderProgram>();
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, QStringLiteral(":/vshader.glsl"));
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, QStringLiteral(":/fshader.glsl"));
    m_program->link();
}
