#include "qtassimpio.h"

#include <assimp/IOStream.hpp>

#include <QtCore/QFile>

#include <memory>

namespace {

// A wrapper to be able to read Qt Resources
class QtIOStream : public Assimp::IOStream
{
public:
    explicit QtIOStream(const QString &path)
        : m_file(path)
    {
    }

    bool open()
    {
        return m_file.open(QIODevice::ReadOnly);
    }

    size_t Read(void *pvBuffer, size_t pSize, size_t pCount) override
    {
        const qint64 bytes = m_file.read(static_cast<char *>(pvBuffer),
                                         static_cast<qint64>(pSize * pCount));
        if (bytes <= 0)
            return 0;
        return static_cast<size_t>(bytes) / pSize;
    }

    size_t Write(const void *, size_t, size_t) override
    {
        return 0;
    }

    aiReturn Seek(size_t pOffset, aiOrigin pOrigin) override
    {
        const qint64 offset = static_cast<qint64>(static_cast<long>(pOffset));
        bool ok = false;
        switch (pOrigin) {
        case aiOrigin_SET:
            ok = m_file.seek(offset);
            break;
        case aiOrigin_CUR:
            ok = m_file.seek(m_file.pos() + offset);
            break;
        case aiOrigin_END:
            ok = m_file.seek(m_file.size() + offset);
            break;
        default:
            break;
        }
        return ok ? aiReturn_SUCCESS : aiReturn_FAILURE;
    }

    size_t Tell() const override
    {
        return static_cast<size_t>(m_file.pos());
    }

    size_t FileSize() const override
    {
        return static_cast<size_t>(m_file.size());
    }

    void Flush() override
    {
    }

private:
    QFile m_file;
};

} // namespace

bool QtIOSystem::Exists(const char *pFile) const
{
    return QFile::exists(resolvePath(pFile));
}

char QtIOSystem::getOsSeparator() const
{
    return '/';
}

Assimp::IOStream *QtIOSystem::Open(const char *pFile, const char * /*pMode*/)
{
    auto stream = std::make_unique<QtIOStream>(resolvePath(pFile));
    if (!stream->open())
        return nullptr;
    return stream.release();
}

void QtIOSystem::Close(Assimp::IOStream *pFile)
{
    delete pFile;
}

QString QtIOSystem::resolvePath(const char *pFile) const
{
    const QString path = QString::fromUtf8(pFile);
    if (QFile::exists(path))
        return path;

    if (StackSize() > 0) {
        const QString joined =
            QString::fromStdString(CurrentDirectory()) + QLatin1Char('/') + path;
        if (QFile::exists(joined))
            return joined;
    }

    return path;
}
