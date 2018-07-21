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
    _funcs->glDrawArrays(GL_TRIANGLES, 0, 3);
    _program->release();
}

void Window::initializeGeometry()
{
    // setup vertex data

    GLfloat vertices[] = {
        // Позиции           // Цвета
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // Нижний правый угол
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // Нижний левый угол
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // Верхний угол
    };

    _vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&_vao);

    _vbo.create();
    _vbo.bind();
    _vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    _vbo.allocate(vertices, sizeof(vertices));

    // Атрибут с координатами
    _funcs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    _funcs->glEnableVertexAttribArray(0);
    // Атрибут с цветом
    _funcs->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3* sizeof(GLfloat)));
    _funcs->glEnableVertexAttribArray(1);
}

void Window::initializeShaders()
{
    _program = std::make_unique<QOpenGLShaderProgram>();
    _program->addShaderFromSourceFile(QOpenGLShader::Vertex, QStringLiteral(":/vshader.glsl"));
    _program->addShaderFromSourceFile(QOpenGLShader::Fragment, QStringLiteral(":/fshader.glsl"));
    _program->link();
}
