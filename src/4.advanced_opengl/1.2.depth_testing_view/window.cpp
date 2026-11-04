#include "window.h"
#include <camera.h>

#include <QtOpenGL/QOpenGLVersionFunctionsFactory>

#include <QtGui/QKeyEvent>

#include <QtCore/QDebug>

Window::Window() :
    m_camera(std::make_unique<Camera>())
{
    resize(640, 480);

    m_camera->setWindow(this);
}

Window::~Window()
{
    makeCurrent();
    m_cubeTexture.reset();
    m_floorTexture.reset();
    doneCurrent();
}

void Window::initializeGL()
{
    if (!context()) {
        qCritical() << "Can't get OGL context";
        close();
        return;
    }

    m_funcs = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_3_Core>(context());
    if (!m_funcs) {
        qCritical() << "Can't get OGL 3.2";
        close();
        return;
    }

    m_funcs->initializeOpenGLFunctions();
    m_funcs->glEnable(GL_DEPTH_TEST);
    m_funcs->glDepthFunc(GL_LESS);

    initializeGeometry();
    initializeShaders();
    initializeTextures();
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

    m_funcs->glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    m_funcs->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_program->bind();
    m_program->setUniformValue("view", m_camera->view());
    m_program->setUniformValue("projection", m_camera->projection());

    m_funcs->glActiveTexture(GL_TEXTURE0);
    m_program->setUniformValue("ourTexture1", 0);

    m_cubeTexture->bind();
    {
        QOpenGLVertexArrayObject::Binder vaoBinder(&m_cubeVao);

        QMatrix4x4 model;
        model.translate(-1.0f, 0.0f, -1.0f);
        m_program->setUniformValue("model", model);
        m_funcs->glDrawArrays(GL_TRIANGLES, 0, 36);

        model.setToIdentity();
        model.translate(2.0f, 0.0f, 0.0f);
        m_program->setUniformValue("model", model);
        m_funcs->glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    m_cubeTexture->release();

    m_floorTexture->bind();
    {
        QOpenGLVertexArrayObject::Binder vaoBinder(&m_planeVao);
        m_program->setUniformValue("model", QMatrix4x4());
        m_funcs->glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    // release resources
    m_program->release();
    m_floorTexture->release();
}

void Window::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    }
    QOpenGLWindow::keyPressEvent(event);
}

void Window::initializeGeometry()
{
    // setup vertex data

    GLfloat vertices[] = {
        // Positions            // Texture coords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    // texture coords above 1, together with GL_REPEAT, make the floor texture tile
    GLfloat planeVertices[] = {
        5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
       -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
       -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

        5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
       -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
        5.0f, -0.5f, -5.0f,  2.0f, 2.0f
    };

    const auto setupMesh = [this](QOpenGLVertexArrayObject &vao, QOpenGLBuffer &vbo, const void *data, int size) {
        vao.create();
        QOpenGLVertexArrayObject::Binder vaoBinder(&vao);

        vbo.create();
        vbo.bind();
        vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
        vbo.allocate(data, size);

        m_funcs->glEnableVertexAttribArray(0);
        m_funcs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);

        m_funcs->glEnableVertexAttribArray(1);
        m_funcs->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(3 * sizeof(GLfloat)));
        vbo.release();
    };

    setupMesh(m_cubeVao, m_cubeVbo, vertices, sizeof(vertices));
    setupMesh(m_planeVao, m_planeVbo, planeVertices, sizeof(planeVertices));
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
    const auto loadTexture = [](const QString &path) {
        auto texture = std::make_unique<QOpenGLTexture>(QImage(path));
        texture->setWrapMode(QOpenGLTexture::Repeat);
        texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
        texture->setMagnificationFilter(QOpenGLTexture::Linear);
        return texture;
    };

    m_cubeTexture = loadTexture(QStringLiteral(":/marble.jpg"));
    m_floorTexture = loadTexture(QStringLiteral(":/metal.png"));
}
