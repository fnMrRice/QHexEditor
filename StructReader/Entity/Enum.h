#ifndef ENUM_H
#define ENUM_H

#include "IBaseEntity.h"
#include "StructReader_global.h"
#include "common/macros.h"

namespace Entity {
    class STRUCTREADER_EXPORT Enum : public IBaseEntity {
       public:
        Enum() = default;
        Enum(const QJsonObject &);
        Enum(const QString &name, const size_t &size);
        virtual ~Enum() = default;

       public:
        bool fromJson(const QJsonObject &) override;
        QJsonObject toJson() const override;

       public:
        MAKE_ENTITY_ATTRIBUTE(QString, name);
        MAKE_ENTITY_ATTRIBUTE(size_t, size);

       public:
        void addEnum(const QString &name, const int &value) { m_values.insert(std::make_pair(value, name)); }
        void removeEnum(const QString &name);
        void removeEnum(const int64_t &id) { m_values.erase(id); }
        std::map<int64_t, QString> get_values() const { return m_values; }

       private:
        std::map<int64_t, QString> m_values;
    };
}  // namespace Entity

#endif  // ENUM_H
