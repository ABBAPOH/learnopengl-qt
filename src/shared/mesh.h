#ifndef MESH_H
#define MESH_H

#include <QtOpenGL/QOpenGLBuffer>
#include <QtOpenGL/QOpenGLFunctions_3_3_Core>
#include <QtOpenGL/QOpenGLShaderProgram>
#include <QtOpenGL/QOpenGLVertexArrayObject>

#include <QtGui/QVector2D>
#include <QtGui/QVector3D>

#include <memory>
#include <vector>

struct Vertex {
    QVector3D position;
    QVector3D normal;
    QVector2D texCoords;
};

struct Texture {
    unsigned int id{};
    QString type;
    QString path;
};

class Mesh {
public:
    // mesh data
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Mesh(QOpenGLFunctions_3_3_Core *funcs, std::vector<Vertex> vertices,
         std::vector<unsigned int> indices, std::vector<Texture> textures);
    Mesh(Mesh &&) noexcept = default;
    Mesh &operator=(Mesh &&) noexcept = default;
    ~Mesh() = default;

    void draw(QOpenGLShaderProgram &shaderProgram);

private:
    void setupMesh();

    QOpenGLFunctions_3_3_Core *m_funcs{nullptr};
    QOpenGLBuffer m_vbo{QOpenGLBuffer::VertexBuffer};
    QOpenGLBuffer m_ebo{QOpenGLBuffer::IndexBuffer};
    std::unique_ptr<QOpenGLVertexArrayObject> m_vao;
};

#endif // MESH_H
