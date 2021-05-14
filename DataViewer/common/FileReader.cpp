#include "FileReader.h"

#include "Exceptions.h"

FileReader::FileReader(const std::shared_ptr<QFileDevice> &file) : m_file(file) {
    if (!file->isOpen()) {
        auto success = file->open(QIODevice::ReadOnly);
        if (!success) {
            throw IOException("Cannot open QIODevice");
        }
    }
    if (!file->isReadable()) {
        throw IOException("QIODevice must be readable");
    }
}

QByteArray FileReader::readBytes(const size_t &begin, const size_t &size) {
    if (size > BUFFER_SIZE_HALF) throw IOException("What are you doing? This should never happen");
    auto file = m_file.lock();
    if (!file) {
        throw IOException("File released.");
    }
    if ((size_t)file->size() <= begin) return QByteArray();
    auto left = m_buffer_begin;
    auto right = m_buffer_begin + (size_t)m_buffer.size();
    if (begin < left || right < begin + size) {
        // data not in buffer
        // read buffer
        m_buffer_begin = 0;
        if (BUFFER_SIZE_HALF < begin) m_buffer_begin = begin - BUFFER_SIZE_HALF;
        file->seek((qint64)m_buffer_begin);  // may cause error in a super large file
        m_buffer = file->read((qint64)BUFFER_SIZE);
    }
    // now data in buffer
    auto d_begin = begin - m_buffer_begin;
    if ((size_t)m_buffer.size() < (d_begin + size)) {
        return m_buffer.mid((int)d_begin);
    }
    return m_buffer.mid((int)d_begin, (int)size);
}

size_t FileReader::fileSize() const {
    auto file = m_file.lock();
    if (auto file = m_file.lock()) {
        return file->size();
    }
    throw IOException("File released.");
}
