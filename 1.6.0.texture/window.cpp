#include "window.h"

#include <QDebug>
#include <QCoreApplication>
#include <QTime>

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

    m_funcs->glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    m_funcs->glClear(GL_COLOR_BUFFER_BIT);

    m_program->bind();

    m_funcs->glActiveTexture(GL_TEXTURE0);
    m_funcs->glBindTexture(GL_TEXTURE_2D, m_texture);
    m_program->setUniformValue("ourTexture", 0);

    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_funcs->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // release resources
    m_program->release();
    m_funcs->glBindTexture(GL_TEXTURE_2D, 0);
}

void Window::initializeGeometry()
{
    // setup vertex data

    GLfloat vertices[] = {
        // Positions          // Colors             // Texture coords
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // Top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // Bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // Bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // Top left
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
    m_funcs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), nullptr);

    // Colors attribute
    m_funcs->glEnableVertexAttribArray(1);
    m_funcs->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(3* sizeof(GLfloat)));

    // Texture attribute
    m_funcs->glEnableVertexAttribArray(2);
    m_funcs->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(6 * sizeof(GLfloat)));
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
    QImage image(":/container.jpg");
    if (image.isNull()) {
        qCritical() << "Can't load image";
        close();
        return;
    }
    image = image.convertToFormat(QImage::Format_RGBA8888);

    m_funcs->glGenTextures(1, &m_texture);
    m_funcs->glBindTexture(GL_TEXTURE_2D, m_texture);
    m_funcs->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
    m_funcs->glGenerateMipmap(GL_TEXTURE_2D);
    m_funcs->glBindTexture(GL_TEXTURE_2D, 0);
}
