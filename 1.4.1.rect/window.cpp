#include "window.h"

#include <QDebug>
#include <QCoreApplication>

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

    _funcs = context()->versionFunctions<QOpenGLFunctions_3_3_Core>();
    if (!_funcs) {
        qCritical() << "Can't get OGL 3.2";
        close();
        return;
    }
    _funcs->initializeOpenGLFunctions();

    initializeGeometry();
    initializeShaders();
}

void Window::resizeGL(int w, int h)
{
    if (!_funcs)
        return;

    _funcs->glViewport(0, 0, w, h);
}

void Window::paintGL()
{
    if (!_funcs)
        return;

    _funcs->glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    _funcs->glClear(GL_COLOR_BUFFER_BIT);

    _program->bind();
    QOpenGLVertexArrayObject::Binder vaoBinder(&_vao);
    _funcs->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    _program->release();
}

void Window::initializeGeometry()
{
    // setup vertex data

    GLfloat vertices[] = {
        0.5f,  0.5f, 0.0f,  // Верхний правый угол
        0.5f, -0.5f, 0.0f,  // Нижний правый угол
        -0.5f, -0.5f, 0.0f,  // Нижний левый угол
        -0.5f,  0.5f, 0.0f   // Верхний левый угол
    };
    GLuint indices[] = {  // Помните, что мы начинаем с 0!
        0, 1, 3,   // Первый треугольник
        1, 2, 3    // Второй треугольник
    };

    _vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&_vao);

    _vbo.create();
    _vbo.bind();
    _vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    _vbo.allocate(vertices, sizeof(vertices));

    _ibo.create();
    _ibo.bind();
    _ibo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    _ibo.allocate(indices, sizeof(indices));

    _funcs->glEnableVertexAttribArray(0);
    _funcs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
}

void Window::initializeShaders()
{
    _program = std::make_unique<QOpenGLShaderProgram>();
    _program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    _program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    _program->link();
}
