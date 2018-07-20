#include "window.h"

#include <QDebug>
#include <QCoreApplication>
#include <QTime>

const char* const vertexShaderSource = "\n"
"#version 330 core \n"

"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 color;\n"
"layout (location = 2) in vec2 texCoord;\n"

"out vec3 ourColor;\n"
"out vec2 TexCoord;\n"

"void main()\n"
"{\n"
"    gl_Position = vec4(position, 1.0);\n"
"    ourColor = color;\n"
"    TexCoord = vec2(texCoord.x, 1.0f - texCoord.y);\n"
"}";

const char *const fragmentShaderSource =
"#version 330 core\n"

"in vec3 ourColor;\n"
"in vec2 TexCoord;\n"

"out vec4 color;\n"

"uniform sampler2D ourTexture1;\n"
"uniform sampler2D ourTexture2;\n"

"void main()\n"
"{\n"
"    color = mix(texture(ourTexture1, TexCoord), texture(ourTexture2, TexCoord), 0.2);\n"
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
    initializeTextures();
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

    _funcs->glActiveTexture(GL_TEXTURE0);
    _funcs->glBindTexture(GL_TEXTURE_2D, _texture1);
    _funcs->glUniform1i(_program->uniformLocation("ourTexture1"), 0);

    _funcs->glActiveTexture(GL_TEXTURE1);
    _funcs->glBindTexture(GL_TEXTURE_2D, _texture2);
    _funcs->glUniform1i(_program->uniformLocation("ourTexture2"), 1);

    QOpenGLVertexArrayObject::Binder vaoBinder(&_vao);
    _funcs->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    _program->release();
    _funcs->glBindTexture(GL_TEXTURE_2D, 0);
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

    // Атрибут с координатами
    _funcs->glEnableVertexAttribArray(0);
    _funcs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), nullptr);

    // Атрибут с цветом
    _funcs->glEnableVertexAttribArray(1);
    _funcs->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3* sizeof(GLfloat)));

    // Атрибут с текстурой
    _funcs->glEnableVertexAttribArray(2);
    _funcs->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
}

void Window::initializeShaders()
{
    _program = std::make_unique<QOpenGLShaderProgram>();
    _program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    _program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    _program->link();
}

void Window::initializeTextures()
{
    // texture 1
    QImage image1(":/container.jpg");
    if (image1.isNull()) {
        qCritical() << "Can't load image";
        close();
        return;
    }
    image1 = image1.convertToFormat(QImage::Format_RGB888);

    _funcs->glGenTextures(1, &_texture1);
    _funcs->glBindTexture(GL_TEXTURE_2D, _texture1);
    _funcs->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image1.width(), image1.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, image1.bits());
    _funcs->glGenerateMipmap(GL_TEXTURE_2D);
    _funcs->glBindTexture(GL_TEXTURE_2D, 0);

    // texture 2
    QImage image2(":/awesomeface.png");
    if (image2.isNull()) {
        qCritical() << "Can't load image";
        close();
        return;
    }
    image2 = image2.convertToFormat(QImage::Format_RGB888);

    _funcs->glGenTextures(1, &_texture2);
    _funcs->glBindTexture(GL_TEXTURE_2D, _texture2);
    _funcs->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image2.width(), image2.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, image2.bits());
    _funcs->glGenerateMipmap(GL_TEXTURE_2D);
    _funcs->glBindTexture(GL_TEXTURE_2D, 0);
}
