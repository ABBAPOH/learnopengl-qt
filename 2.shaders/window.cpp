#include "window.h"

#include <QDebug>
#include <QCoreApplication>
#include <QTime>

const char* const vertexShaderSource = "\n"
"#version 330 core \n"

"in vec3 position;\n"
"layout (location = 1) in vec3 color;\n"

"out vec3 ourColor;\n"

"void main()\n"
"{\n"
"    gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
"    ourColor = color;\n"
"}";

const char *const fragmentShaderSource =
"#version 330 core\n"

"in vec3 ourColor;\n"
"out vec4 color;\n"

"void main()\n"
"{\n"
"    color = vec4(ourColor, 1.0f);\n"
"}"
;

Window::Window()
{
    resize(640, 480);
}

void Window::initializeGL()
{
    if (!context()) {
        throw std::runtime_error("Can't get OGL context");
    }
    _funcs = context()->versionFunctions<QOpenGLFunctions_3_3_Core>();
    if (!_funcs) {
        qCritical() << "Can't get OGL 3.2";
        qApp->quit();
        return;
    }
    _funcs->initializeOpenGLFunctions();
    qInfo() << "real OGL version" << (char *)_funcs->glGetString(GL_VERSION);

    { // setup vertex data

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

    _program = std::make_unique<QOpenGLShaderProgram>();
    _program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    _program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    _program->link();

    vertexColorLocation = _program->uniformLocation("ourColor");
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
    _program->setUniformValue(vertexColorLocation, QVector4D(0.0f, QTime::currentTime().second() / 60.0f, 0.0f, 1.0f));
    QOpenGLVertexArrayObject::Binder vaoBinder(&_vao);
    _funcs->glDrawArrays(GL_TRIANGLES, 0, 3);
    _program->release();
}
