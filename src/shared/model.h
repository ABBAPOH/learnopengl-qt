#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <QtOpenGL/QOpenGLTexture>

#include <memory>
#include <vector>

class Model
{
public:
    Model(QOpenGLFunctions_3_3_Core *funcs, const QString &path);
    void draw(QOpenGLShaderProgram &shaderProgram);

private:
    // model data
    QOpenGLFunctions_3_3_Core *m_funcs {nullptr};
    std::vector<Texture> textures_loaded;
    std::vector<Mesh> meshes;
    std::vector<std::unique_ptr<QOpenGLTexture>> m_ownedTextures;
    QString directory;

    void loadModel(const QString &path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                              const QString &typeName);
    unsigned int textureFromFile(const QString &path);
};

#endif // MODEL_H
