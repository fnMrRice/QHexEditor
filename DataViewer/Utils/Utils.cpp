#include "Utils.h"

namespace Utils {
    QString toHexBe(const size_t &value, const size_t &size) {
        static auto constexpr HEX_DIGITS = "0123456789ABCDEF";
        if (size == 1) {
            QString retval;
            retval.append(HEX_DIGITS[(value >> 4) & 0x0F]);
            retval.append(HEX_DIGITS[value & 0x0F]);
            return retval;
        }
        auto tsize = size / 2;
        QString retval;
        retval.append(toHexBe(value, tsize));
        retval.append(toHexBe(value >> tsize * 8, tsize));
        return retval;
    }

    QString toHexLe(const size_t &value, const size_t &size) {
        static auto constexpr HEX_DIGITS = "0123456789ABCDEF";
        if (size == 1) {
            QString retval;
            retval.append(HEX_DIGITS[(value >> 4) & 0x0F]);
            retval.append(HEX_DIGITS[value & 0x0F]);
            return retval;
        }
        auto tsize = size / 2;
        QString retval;
        retval.append(toHexLe(value >> tsize * 8, tsize));
        retval.append(toHexLe(value, tsize));
        return retval;
    }

    QByteArray GetHash(const std::shared_ptr<QFile> &file) {
        auto g = std::make_unique<QCryptographicHash>(QCryptographicHash ::Md5);
        if (!file->isOpen()) {
            if (!file->open(QIODevice::ReadOnly)) return QByteArray();
        }
        if (g->addData(file.get())) return g->result();
        return QByteArray();
    }
}  // namespace Utils
