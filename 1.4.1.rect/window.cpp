#include "window.h"

#include <QtCore/QDebug>

const char* const vertexShaderSource =
"#version 330 core \n"

"layout (location = 0) in vec3 position;\n"

"void main()\n"
"{\n"
"    gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
"}";

const char *const fragmentShaderSource =
"#version 330 core\n"

"out vec4 color;\n"

"void main()\n"
"{\n"
"    color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}"
;

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
    m_funcs->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    m_program->release();
}

void Window::initializeGeometry()
{
    // setup vertex data

    GLfloat vertices[] = {
        0.5f,  0.5f, 0.0f,  // Top right corner
        0.5f, -0.5f, 0.0f,  // Bottom right corner
        -0.5f, -0.5f, 0.0f,  // Bottom left corner
        -0.5f,  0.5f, 0.0f   // Top left corner
    };
    GLuint indices[] = {  // Zero-based indexation
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

    m_funcs->glEnableVertexAttribArray(0);
    m_funcs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
}

void Window::initializeShaders()
{
    m_program = std::make_unique<QOpenGLShaderProgram>();
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();
}
