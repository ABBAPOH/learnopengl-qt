#include "window.h"
#include "camera.h"

#include <QtGui/QKeyEvent>

#include <QtCore/QDebug>

#include <cmath>

namespace {

// setup vertex data

constexpr const GLfloat vertices[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

constexpr QVector3D cubePositions[] = {
    {0.0f,  0.0f,  0.0f},
    {2.0f,  5.0f, -15.0f},
    {-1.5f, -2.2f, -2.5f},
    {-3.8f, -2.0f, -12.3f},
    {2.4f, -0.4f, -3.5f},
    {-1.7f,  3.0f, -7.5f},
    {1.3f, -2.0f, -2.5f},
    {1.5f,  2.0f, -2.5f},
    {1.5f,  0.2f, -1.5f},
    {-1.3f,  1.0f, -1.5f}
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
    m_texture->destroy();
    m_textureSpecular->destroy();
    doneCurrent();
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
    m_funcs->glEnable(GL_DEPTH_TEST);

    initializeCubeGeometry();
    initializeLampGeometry();
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

    paintCube();
    paintLamps();
}

void Window::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    } else if (event->key() == Qt::Key_F) {
        toggleFullScreen();
    }

    QWindow::keyPressEvent(event);
}

void Window::toggleFullScreen()
{
    if (windowState() != Qt::WindowState::WindowFullScreen)
        showFullScreen();
    else
        showNormal();
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
    m_funcs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), nullptr);

    // Normals attribute
    m_funcs->glEnableVertexAttribArray(1);
    m_funcs->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(3 * sizeof(GLfloat)));

    m_funcs->glEnableVertexAttribArray(2);
    m_funcs->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(6 * sizeof(GLfloat)));

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
    m_funcs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), nullptr);
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

void Window::initializeTextures()
{
    m_texture = std::make_unique<QOpenGLTexture>(QImage(":/container2.png").mirrored());
    m_textureSpecular = std::make_unique<QOpenGLTexture>(QImage(":/container2_specular.png").mirrored());
}

void Window::paintCube()
{
    m_program->bind();

    m_program->setUniformValue("view", m_camera->view());
    m_program->setUniformValue("projection", m_camera->projection());

    m_program->setUniformValue("objectColor", QVector3D(1.0f, 0.5f, 0.31f));
    m_program->setUniformValue("viewPos", m_camera->position());

    m_funcs->glActiveTexture(GL_TEXTURE0);
    m_texture->bind();
    m_program->setUniformValue("material.diffuse", 0);

    m_funcs->glActiveTexture(GL_TEXTURE1);
    m_textureSpecular->bind();
    m_program->setUniformValue("material.specular", 1);

    m_program->setUniformValue("material.specular", QVector3D(0.5f, 0.5f, 0.5f));
    m_program->setUniformValue("material.shininess", 32.0f);

    // direct light
    m_program->setUniformValue("dirLight.ambient", QVector3D(0.2f, 0.2f, 0.2f));
    m_program->setUniformValue("dirLight.diffuse", QVector3D(0.5f, 0.5f, 0.5f));
    m_program->setUniformValue("dirLight.specular", QVector3D(1.0f, 1.0f, 1.0f));
    m_program->setUniformValue("dirLight.direction", QVector3D(-0.2f, -1.0f, -0.3f));

    // point lights
    int lightIndex = 0;
    for (const auto &lightPos: m_lightPositions) {
        const auto prefix = QByteArray("pointLights[") + QByteArray::number(lightIndex) + "].";
        m_program->setUniformValue((prefix + "position").constData(), lightPos);

        m_program->setUniformValue((prefix + "ambient").constData(), QVector3D(0.2f, 0.2f, 0.2f));
        m_program->setUniformValue((prefix + "diffuse").constData(), QVector3D(0.5f, 0.5f, 0.5f));
        m_program->setUniformValue((prefix + "specular").constData(), QVector3D(1.0f, 1.0f, 1.0f));

        m_program->setUniformValue((prefix + "constant").constData(), 1.0f);
        m_program->setUniformValue((prefix + "linear").constData(), 0.09f);
        m_program->setUniformValue((prefix + "quadratic").constData(), 0.032f);

        lightIndex++;
    }

    // spot light
    m_program->setUniformValue("spotLight.position", m_camera->position());
    m_program->setUniformValue("spotLight.direction", m_camera->front());
    m_program->setUniformValue("spotLight.cutoff", cos(radians(12.5f)));
    m_program->setUniformValue("spotLight.outerCutoff", cos(radians(17.5f)));

    m_program->setUniformValue("spotLight.ambient", QVector3D(0.2f, 0.2f, 0.2f));
    m_program->setUniformValue("spotLight.diffuse", QVector3D(0.5f, 0.5f, 0.5f));
    m_program->setUniformValue("spotLight.specular", QVector3D(1.0f, 1.0f, 1.0f));

    m_program->setUniformValue("spotLight.constant", 1.0f);
    m_program->setUniformValue("spotLight.linear", 0.09f);
    m_program->setUniformValue("spotLight.quadratic", 0.032f);

    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    for(unsigned int i = 0; i < 10; i++)
    {
        QMatrix4x4 model;
        const auto angle = 20.0f * i;
        model.translate(cubePositions[i]);
        model.rotate(angle, {1.0f, 0.3f, 0.5f});

        m_program->setUniformValue("model", model);
        m_funcs->glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // release resources
    m_texture->release();
    m_textureSpecular->release();
    m_program->release();
}

void Window::paintLamps()
{
    m_lampProgram->bind();

    m_lampProgram->setUniformValue("view", m_camera->view());
    m_lampProgram->setUniformValue("projection", m_camera->projection());

    for (const auto &lightPos: m_lightPositions) {
        QMatrix4x4 model;
        model.translate(lightPos);
        model.scale({0.2f, 0.2f, 0.2f});
        m_lampProgram->setUniformValue("model", model);
        QOpenGLVertexArrayObject::Binder vaoBinder(&m_lampVao);
        m_funcs->glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // release resources
    m_lampProgram->release();
}
