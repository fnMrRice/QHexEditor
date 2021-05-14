#ifndef FILEREADER_H
#define FILEREADER_H

#include <QFileDevice>
#include <QObject>
#include <memory>

class FileReader {
   public:
    explicit FileReader(const std::shared_ptr<QFileDevice> &file);

   public:
    //    QByteArray readBytes(const size_t &size);  // for sequencal devices
    QByteArray readBytes(const size_t &begin, const size_t &size);
    size_t fileSize() const;

   private:
    static size_t constexpr BUFFER_SIZE = 4 * 1024 * 1024;  // 4m bytes
    static size_t constexpr BUFFER_SIZE_HALF = BUFFER_SIZE / 2;
    // assume user shows 1024 bytes per row, a row is 4mm height
    // 4mbytes is 4*1024*1024/1024*4mm=16384mm=16.384m
    // a half is even 8m long
    // the cache is not worranty to be not enough
    size_t m_buffer_begin = 0;
    size_t m_displaySize = 0;
    std::weak_ptr<QFileDevice> m_file;
    QByteArray m_buffer;
};

#endif  // FILEREADER_H
