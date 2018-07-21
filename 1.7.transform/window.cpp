#include "window.h"

#include <QDebug>
#include <QCoreApplication>
#include <QTime>

Window::Window()
{
    resize(640, 480);

    m_timer = startTimer(40);
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

    auto time = QTime::currentTime();
    QMatrix4x4 transform;
    transform.translate({0.5, -0.5, 0.0});
    transform.rotate(6 * ((time.second() * 1000 + time.msec()) / 25 % 240), {0.0, 0.0, 1.0});

    _funcs->glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    _funcs->glClear(GL_COLOR_BUFFER_BIT);

    _program->bind();

    _funcs->glActiveTexture(GL_TEXTURE0);
    _funcs->glBindTexture(GL_TEXTURE_2D, _texture1);
    _program->setUniformValue("ourTexture1", 0);

    _funcs->glActiveTexture(GL_TEXTURE1);
    _funcs->glBindTexture(GL_TEXTURE_2D, _texture2);
    _program->setUniformValue("ourTexture2", 1);

    _program->setUniformValue("transform", transform);

    QOpenGLVertexArrayObject::Binder vaoBinder(&_vao);
    _funcs->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    _program->release();
    _funcs->glBindTexture(GL_TEXTURE_2D, 0);
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
    _program->addShaderFromSourceFile(QOpenGLShader::Vertex, QStringLiteral(":/vshader.glsl"));
    _program->addShaderFromSourceFile(QOpenGLShader::Fragment, QStringLiteral(":/fshader.glsl"));
    _program->link();
}

void Window::initializeTextures()
{
    _texture1 = createTexture(u":/container.jpg");
    _texture2 = createTexture(u":/awesomeface.png");
}

GLuint Window::createTexture(QStringView path)
{
    QImage image(path.toString());
    if (image.isNull()) {
        qCritical() << "Can't load image" << path;
        close();
        return 0;
    }
    image = image.convertToFormat(QImage::Format_RGBA8888);

    GLuint result {0};
    _funcs->glGenTextures(1, &result);
    _funcs->glBindTexture(GL_TEXTURE_2D, result);
    _funcs->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
    _funcs->glGenerateMipmap(GL_TEXTURE_2D);
    _funcs->glBindTexture(GL_TEXTURE_2D, 0);

    return result;
}
