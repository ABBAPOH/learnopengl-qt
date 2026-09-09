#include "model.h"
#include "qtassimpio.h"

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtGui/QImage>

Model::Model(QOpenGLFunctions_3_3_Core *funcs, const QString &path)
    : m_funcs(funcs)
{
    loadModel(path);
}

void Model::draw(QOpenGLShaderProgram &shaderProgram)
{
    for (auto &mesh : meshes)
        mesh.draw(shaderProgram);
}

void Model::loadModel(const QString &path)
{
    Assimp::Importer importer;
    importer.SetIOHandler(new QtIOSystem);

    const aiScene *scene = importer.ReadFile(
        path.toUtf8().constData(),
        aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        qWarning() << "ERROR::ASSIMP::" << importer.GetErrorString();
        return;
    }

    directory = QFileInfo(path).path();
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
        processNode(node->mChildren[i], scene);
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        Vertex vertex;

        vertex.position = {
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        };

        if (mesh->HasNormals()) {
            vertex.normal = {
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            };
        }

        if (mesh->mTextureCoords[0]) {
            vertex.texCoords = {
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            };
        } else {
            vertex.texCoords = {0.0f, 0.0f};
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        const aiFace &face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j)
            indices.push_back(face.mIndices[j]);
    }

    if (mesh->mMaterialIndex >= 0) {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = loadMaterialTextures(
            material, aiTextureType_DIFFUSE, QStringLiteral("texture_diffuse"));
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture> specularMaps = loadMaterialTextures(
            material, aiTextureType_SPECULAR, QStringLiteral("texture_specular"));
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(m_funcs, std::move(vertices), std::move(indices), std::move(textures));
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                                 const QString &typeName)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i) {
        aiString str;
        mat->GetTexture(type, i, &str);
        const QString texturePath = QString::fromUtf8(str.C_Str());

        bool skip = false;
        for (const Texture &loaded : textures_loaded) {
            if (loaded.path == texturePath) {
                textures.push_back(loaded);
                skip = true;
                break;
            }
        }

        if (!skip) {
            Texture texture;
            texture.id = textureFromFile(texturePath);
            texture.type = typeName;
            texture.path = texturePath;
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }
    return textures;
}

unsigned int Model::textureFromFile(const QString &path)
{
    const QString filename = directory + QLatin1Char('/') + path;

    QImage image(filename);
    if (image.isNull()) {
        qWarning() << "Texture failed to load at path:" << filename;
        return 0;
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 9, 0)
    image = image.flipped();
#else
    image = image.mirrored();
#endif

    auto texture = std::make_unique<QOpenGLTexture>(image);
    texture->setWrapMode(QOpenGLTexture::Repeat);
    texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);

    const unsigned int id = texture->textureId();
    m_ownedTextures.push_back(std::move(texture));
    return id;
}
