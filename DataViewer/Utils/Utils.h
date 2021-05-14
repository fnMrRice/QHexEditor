#ifndef UTILS_H
#define UTILS_H

#include <QtCore>

namespace Utils {
    QString toHexBe(const size_t &value, const size_t &size);
    QString toHexLe(const size_t &value, const size_t &size);

    QByteArray GetHash(const std::shared_ptr<QFile> &);

    template <class _Ty>
    QByteArray toByteArray(const _Ty &value) {
        static_assert(std::is_arithmetic_v<_Ty>, "type of _Ty muse be arithmetic.");
        return QByteArray {reinterpret_cast<const char *>(&value), sizeof(value)};
    }

    template <class _Ty>
    _Ty toArithmetic(const QByteArray &bytes, const int &begin) {
        static_assert(std::is_arithmetic_v<_Ty>, "type of _Ty muse be arithmetic.");
        if (bytes.length() <= sizeof(_Ty) + begin) throw std::out_of_range("bytes.length() <= sizeof(_Ty) + begin");  // pointer out of range
        if (begin < 0) throw std::out_of_range("begin < 0");                                                          // pointer out of range
        auto data_begin = bytes.data() + begin;
        return *reinterpret_cast<const _Ty *>(data_begin);
    }
};  // namespace Utils

#endif  // UTILS_H
