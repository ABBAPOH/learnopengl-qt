#include "mesh.h"

#include <cstddef>

Mesh::Mesh(QOpenGLFunctions_3_3_Core *funcs, std::vector<Vertex> vertices,
           std::vector<unsigned int> indices, std::vector<Texture> textures)
    : vertices(std::move(vertices))
    , indices(std::move(indices))
    , textures(std::move(textures))
    , m_funcs(funcs)
{
    setupMesh();
}

void Mesh::setupMesh()
{
    m_vao = std::make_unique<QOpenGLVertexArrayObject>();
    m_vao->create();
    QOpenGLVertexArrayObject::Binder vaoBinder(m_vao.get());

    m_vbo.create();
    m_vbo.bind();
    m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vbo.allocate(vertices.data(),
                   static_cast<int>(vertices.size() * sizeof(Vertex)));

    m_ebo.create();
    m_ebo.bind();
    m_ebo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_ebo.allocate(indices.data(),
                   static_cast<int>(indices.size() * sizeof(unsigned int)));

    // vertex positions
    m_funcs->glEnableVertexAttribArray(0);
    m_funcs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    // vertex normals
    m_funcs->glEnableVertexAttribArray(1);
    m_funcs->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                   reinterpret_cast<void *>(offsetof(Vertex, normal)));
    // vertex texture coords
    m_funcs->glEnableVertexAttribArray(2);
    m_funcs->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                   reinterpret_cast<void *>(offsetof(Vertex, texCoords)));
}

void Mesh::draw(QOpenGLShaderProgram &shaderProgram)
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for (unsigned int i = 0; i < textures.size(); ++i) {
        m_funcs->glActiveTexture(GL_TEXTURE0 + i);
        QString number;
        const QString &name = textures[i].type;
        if (name == QLatin1String("texture_diffuse"))
            number = QString::number(diffuseNr++);
        else if (name == QLatin1String("texture_specular"))
            number = QString::number(specularNr++);

        shaderProgram.setUniformValue(
            QStringLiteral("%1%2").arg(name, number).toUtf8().constData(),
            static_cast<GLint>(i));
        m_funcs->glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    m_funcs->glActiveTexture(GL_TEXTURE0);

    QOpenGLVertexArrayObject::Binder vaoBinder(m_vao.get());
    m_funcs->glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()),
                            GL_UNSIGNED_INT, nullptr);
}
