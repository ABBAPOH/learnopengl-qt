#ifndef QTASSIMPIO_H
#define QTASSIMPIO_H

#include <assimp/IOSystem.hpp>

#include <QtCore/QString>

class QtIOSystem : public Assimp::IOSystem
{
public:
    bool Exists(const char *pFile) const override;
    char getOsSeparator() const override;
    Assimp::IOStream *Open(const char *pFile, const char *pMode = "rb") override;
    void Close(Assimp::IOStream *pFile) override;

private:
    QString resolvePath(const char *pFile) const;
};

#endif // QTASSIMPIO_H
