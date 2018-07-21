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

    qInfo() << "real OGL version" << (char *)_funcs->glGetString(GL_VERSION);

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
    _funcs->glDrawArrays(GL_TRIANGLES, 0, 3);
    _program->release();
}

void Window::initializeGeometry()
{
    // setup vertex data

    GLfloat vertices[] = {
        // Позиции
        0.5f, -0.5f, 0.0f,  // Нижний правый угол
        -0.5f, -0.5f, 0.0f, // Нижний левый угол
        0.0f,  0.5f, 0.0f,  // Верхний угол
    };

    _vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&_vao);

    _vbo.create();
    _vbo.bind();
    _vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    _vbo.allocate(vertices, sizeof(vertices));

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
