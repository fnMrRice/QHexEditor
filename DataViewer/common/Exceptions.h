#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <QString>
#include <stdexcept>

class Exception : public std::runtime_error {
   public:
    Exception(const std::string& what) : std::runtime_error(what) {}
    Exception(char const* what) : std::runtime_error(what) {}
    Exception(const QString& what) : std::runtime_error(what.toStdString()) {}
};

class IOException : public Exception {
   public:
    IOException(const std::string& what) : Exception(what) {}
    IOException(char const* what) : Exception(what) {}
    IOException(const QString& what) : Exception(what.toStdString()) {}
};

#endif  // EXCEPTIONS_H
