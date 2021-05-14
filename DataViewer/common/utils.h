#ifndef UTILS_H
#define UTILS_H

#include <QString>

class Utils {
   private:
    Utils();

   public:
    static QString toHexBe(const size_t &value, const size_t &size);
    static QString toHexLe(const size_t &value, const size_t &size);
};

#endif  // UTILS_H
