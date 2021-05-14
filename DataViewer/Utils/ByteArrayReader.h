#ifndef BYTEARRAYREADER_H
#define BYTEARRAYREADER_H
#include <QByteArray>

#include "Utils/Utils.h"

class ByteArrayReader {
   public:
    ByteArrayReader(const QByteArray& bytes) : m_data(bytes) {}

   public:
    inline void seek(const int& pos) { m_current_pos = pos; }
    inline void resetArray(const QByteArray& bytes) { m_data = bytes; }

    template <class _Ty>
    inline _Ty read() {
        auto value = Utils::toArithmetic<_Ty>(m_data, m_current_pos);
        m_current_pos += sizeof(value);
        return value;
    }
    inline QByteArray read(const int& size) {
        auto retval = m_data.mid(m_current_pos, size);
        m_current_pos += size;
        return retval;
    }

   private:
    QByteArray m_data;
    int m_current_pos = 0;
};

#endif  // BYTEARRAYREADER_H
