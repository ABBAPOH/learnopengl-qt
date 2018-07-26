#include "window.h"

#include <QDebug>
#include <QCoreApplication>
#include <QTime>
#include <QKeyEvent>

#include <math.h>

constexpr float radians(float angle)
{
    return angle * float(M_PI) / 180.0;
}

Window::Window()
{
    resize(640, 480);

    auto cursor = QCursor();
    cursor.setShape(Qt::BlankCursor);
    setCursor(cursor);

    m_timer = startTimer(25);
}

Window::~Window()
{
    makeCurrent();
    m_texture1.reset();
    m_texture2.reset();
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

    initializeGeometry();
    initializeShaders();
    initializeTextures();
    updateMatrixes();
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
    m_funcs->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_program->bind();

    m_funcs->glActiveTexture(GL_TEXTURE0);
    m_texture1->bind();
    m_program->setUniformValue("ourTexture1", 0);

    m_funcs->glActiveTexture(GL_TEXTURE1);
    m_texture2->bind();
    m_program->setUniformValue("ourTexture2", 1);

    for (int i = 0; i < 10; i++) {
        QMatrix4x4 model;
        model.translate(m_cubePositions[i]);
        model.rotate(20.0 * i, {1.0f, 0.3f, 0.5f});

        m_program->setUniformValue("model", model);
        m_program->setUniformValue("view", m_view);
        m_program->setUniformValue("projection", m_projection);

        QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
        m_funcs->glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // release resources
    m_program->release();
    m_texture2->release();
    m_funcs->glActiveTexture(GL_TEXTURE0);
    m_texture1->release();
}

void Window::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_timer) {
        updateMatrixes();
        update();
    }
    QOpenGLWindow::timerEvent(event);
}

void Window::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
        close();
    else if (event->key() == Qt::Key_W)
        keyPressed[Forward] = true;
    else if (event->key() == Qt::Key_S)
        keyPressed[Backward] = true;
    else if (event->key() == Qt::Key_A)
        keyPressed[Left] = true;
    else if (event->key() == Qt::Key_D)
        keyPressed[Right] = true;
    else
        QWindow::keyPressEvent(event);
}

void Window::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_W)
        keyPressed[Forward] = false;
    else if (event->key() == Qt::Key_S)
        keyPressed[Backward] = false;
    else if (event->key() == Qt::Key_A)
        keyPressed[Left] = false;
    else if (event->key() == Qt::Key_D)
        keyPressed[Right] = false;
    else
        QWindow::keyReleaseEvent(event);
}

void Window::focusInEvent(QFocusEvent *event)
{
    setMouseGrabEnabled(true);
    m_blockMove = true;
    QCursor::setPos(geometry().center());
    QWindow::focusInEvent(event);
}

void Window::focusOutEvent(QFocusEvent *event)
{
    setMouseGrabEnabled(false);
    QWindow::focusOutEvent(event);
}

void Window::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_blockMove) {
        const auto sensitivity = 0.075f;
        const auto size = geometry().size();
        const auto center = QPointF(size.width() / 2.0, size.height() / 2.0);
        const auto delta = event->localPos() - center;
        const auto deltax = delta.x() * sensitivity;
        const auto deltay = delta.y() * sensitivity;
        m_yaw += deltax;
        m_pitch = qBound(-89.0, m_pitch - deltay, 89.0);

        QVector3D front {
            cos(radians(m_pitch)) * cos(radians(m_yaw)),
            sin(radians(m_pitch)),
            cos(radians(m_pitch)) * sin(radians(m_yaw))
        };
        m_cameraFront = front.normalized();

        m_blockMove = true;
        QCursor::setPos(geometry().center());
    } else {
        m_blockMove = false;
    }

    QWindow::mouseMoveEvent(event);
}

void Window::initializeGeometry()
{
    // setup vertex data

    GLfloat vertices[] = {
        // Позиции            // Текстурные координаты
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

    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    m_vbo.create();
    m_vbo.bind();
    m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vbo.allocate(vertices, sizeof(vertices));

    // Атрибут с координатами
    m_funcs->glEnableVertexAttribArray(0);
    m_funcs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);

    // Атрибут с текстурой
    m_funcs->glEnableVertexAttribArray(1);
    m_funcs->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    m_vbo.release();

    m_cubePositions = {
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
    m_texture1 = std::make_unique<QOpenGLTexture>(QImage(":/container.jpg"));
    m_texture2 = std::make_unique<QOpenGLTexture>(QImage(":/awesomeface.png"));
}

void Window::updateMatrixes()
{
    m_view = QMatrix4x4();
    m_projection = QMatrix4x4();

    const auto time = QTime::currentTime();
    const auto angle = (time.second() * 1000 + time.msec()) / 25 % 360;
    const auto radius = 15.0f;
    const auto camX = sinf(angle * float(M_PI) / 180) * radius;
    const auto camZ = cosf(angle * float(M_PI) / 180) * radius;

//    m_view.lookAt({camX, 0.0f, camZ}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});

    QVector3D cameraUp {0.0f, 1.0f,  0.0f};
    const auto cameraSpeed = 0.05f;

    if (keyPressed[Forward])
        m_cameraPos += cameraSpeed * m_cameraFront;
    if (keyPressed[Backward])
        m_cameraPos -= cameraSpeed * m_cameraFront;
    if (keyPressed[Left])
        m_cameraPos -= QVector3D::crossProduct(m_cameraFront, cameraUp).normalized() * cameraSpeed;
    if (keyPressed[Right])
        m_cameraPos += QVector3D::crossProduct(m_cameraFront, cameraUp).normalized() * cameraSpeed;

    m_view.lookAt(m_cameraPos, m_cameraPos + m_cameraFront, {0.0f, 1.0f, 0.0f});
    m_projection.perspective(45.0, 1.0 * width() / height(), 0.1, 100.0);
}
