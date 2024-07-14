#include "window.h"
#include <camera.h>

#if QT_VERSION >= 0x060000
#include <QtOpenGL/QOpenGLVersionFunctionsFactory>
#endif

#include <QtGui/QKeyEvent>

#include <QtCore/QDebug>
#include <QtCore/QTime>

#include <cmath>

namespace {

// setup vertex data

constexpr const GLfloat vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
    0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
    0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

} // namespace

Window::Window() :
    m_camera(std::make_unique<Camera>())
{
    resize(640, 480);

    m_camera->setWindow(this);
}

Window::~Window()
{
    makeCurrent();
    doneCurrent();
}

void Window::initializeGL()
{
    if (!context()) {
        qCritical() << "Can't get OGL context";
        close();
        return;
    }

#if QT_VERSION >= 0x060000
    m_funcs = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_3_Core>(context());
#else
    m_funcs = context()->versionFunctions<QOpenGLFunctions_3_3_Core>();
#endif
    if (!m_funcs) {
        qCritical() << "Can't get OGL 3.2";
        close();
        return;
    }

    m_funcs->initializeOpenGLFunctions();
    m_funcs->glEnable(GL_DEPTH_TEST);

    initializeCubeGeometry();
    initializeLampGeometry();
    initializeShaders();
}

void Window::resizeGL(int w, int h)
{
    if (!m_funcs) {
        return;
    }

    m_funcs->glViewport(0, 0, w, h);
}

void Window::paintGL()
{
    if (!m_funcs) {
        return;
    }

    m_funcs->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    paintCube();
    paintLamp();
}

void Window::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    }
    QOpenGLWindow::keyPressEvent(event);
}

void Window::initializeCubeGeometry()
{
    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    m_vbo.create();
    m_vbo.bind();
    m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vbo.allocate(vertices, sizeof(vertices));

    // Vertexes attribute
    m_funcs->glEnableVertexAttribArray(0);
    m_funcs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);

    // Normals attribute
    m_funcs->glEnableVertexAttribArray(1);
    m_funcs->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(3 * sizeof(GLfloat)));
    m_vbo.release();
}

void Window::initializeLampGeometry()
{
    m_lampVao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_lampVao);

    m_lampVbo.create();
    m_lampVbo.bind();
    m_lampVbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_lampVbo.allocate(vertices, sizeof(vertices));

    // Vertexes attribute
    m_funcs->glEnableVertexAttribArray(0);
    m_funcs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
}

void Window::initializeShaders()
{
    m_program = std::make_unique<QOpenGLShaderProgram>();
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, QStringLiteral(":/vshader.glsl"));
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, QStringLiteral(":/fshader.glsl"));
    m_program->link();

    m_lampProgram = std::make_unique<QOpenGLShaderProgram>();
    m_lampProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, QStringLiteral(":/vlamp.glsl"));
    m_lampProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, QStringLiteral(":/flamp.glsl"));
    m_lampProgram->link();
}

void Window::paintCube()
{
    m_program->bind();

    m_program->setUniformValue("view", m_camera->view());
    m_program->setUniformValue("projection", m_camera->projection());

    m_program->setUniformValue("objectColor", QVector3D(1.0f, 0.5f, 0.31f));
    m_program->setUniformValue("viewPos", m_camera->position());

    m_program->setUniformValue("material.ambient",  QVector3D(1.0f, 0.5f, 0.31f));
    m_program->setUniformValue("material.diffuse",  QVector3D(1.0f, 0.5f, 0.31f));
    m_program->setUniformValue("material.specular", QVector3D(0.5f, 0.5f, 0.5f));
    m_program->setUniformValue("material.shininess", 32.0f);

    const auto time = QTime::currentTime();
    const auto angle = radians(((time.second() * 1000 + time.msec()) % 10000) / 10000.0 * 360.0);
    const auto lightColor = QVector3D(sin(angle * 2.0f) / 2.0 + 0.5,
                                      sin(angle * 0.7f) / 2.0 + 0.5,
                                      sin(angle * 1.3f) / 2.0 + 0.5);
    const auto diffuseColor = lightColor * QVector3D(0.5f, 0.5f, 0.5f); // decrease the influence
    const auto ambientColor = diffuseColor * QVector3D(0.2f, 0.2f, 0.2f); // low influence
    m_program->setUniformValue("light.ambient", ambientColor);
    m_program->setUniformValue("light.diffuse", diffuseColor);
    m_program->setUniformValue("light.specular", QVector3D(1.0f, 1.0f, 1.0f));
    m_program->setUniformValue("light.position", m_lightPos);

    m_program->setUniformValue("model", QMatrix4x4());

    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_funcs->glDrawArrays(GL_TRIANGLES, 0, 36);

    // release resources
    m_program->release();
}

void Window::paintLamp()
{
    m_lampProgram->bind();

    m_lampProgram->setUniformValue("view", m_camera->view());
    m_lampProgram->setUniformValue("projection", m_camera->projection());

    QMatrix4x4 model;
    model.translate(m_lightPos);
    model.scale({0.2f, 0.2f, 0.2f});
    m_lampProgram->setUniformValue("model", model);
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_lampVao);
    m_funcs->glDrawArrays(GL_TRIANGLES, 0, 36);

    // release resources
    m_lampProgram->release();
}
