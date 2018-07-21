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
    m_funcs->glBindTexture(GL_TEXTURE_2D, m_texture1);
    m_program->setUniformValue("ourTexture1", 0);

    m_funcs->glActiveTexture(GL_TEXTURE1);
    m_funcs->glBindTexture(GL_TEXTURE_2D, m_texture2);
    m_program->setUniformValue("ourTexture2", 1);

    m_program->setUniformValue("model", m_model);
    m_program->setUniformValue("view", m_view);
    m_program->setUniformValue("projection", m_projection);

    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_funcs->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // release resources
    m_program->release();
    m_funcs->glBindTexture(GL_TEXTURE_2D, 0);
    m_funcs->glActiveTexture(GL_TEXTURE0);
    m_funcs->glBindTexture(GL_TEXTURE_2D, 0);
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

    // Атрибут с координатами
    m_funcs->glEnableVertexAttribArray(0);
    m_funcs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), nullptr);

    // Атрибут с цветом
    m_funcs->glEnableVertexAttribArray(1);
    m_funcs->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3* sizeof(GLfloat)));

    // Атрибут с текстурой
    m_funcs->glEnableVertexAttribArray(2);
    m_funcs->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
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
    m_texture1 = createTexture(u":/container.jpg");
    m_texture2 = createTexture(u":/awesomeface.png");
}

void Window::initializeMatrixes()
{
    m_model.rotate(-55.0, {1.0, 0.0, 0.0});
    m_view.translate({0.0, 0.0, -3.0});
    m_projection.perspective(45.0, 1.0 * width() / height(), 0.1, 100.0);
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
    m_funcs->glGenTextures(1, &result);
    m_funcs->glBindTexture(GL_TEXTURE_2D, result);
    m_funcs->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
    m_funcs->glGenerateMipmap(GL_TEXTURE_2D);
    m_funcs->glBindTexture(GL_TEXTURE_2D, 0);

    return result;
}
