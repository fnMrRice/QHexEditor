#ifndef STRUCT_H
#define STRUCT_H
#include "IBaseEntity.h"
#include "common/macros.h"

namespace Entity {
    class Struct : public IBaseEntity {
       public:
        Struct() = default;
        Struct(const QString &name);
        Struct(const QJsonObject &);
        virtual ~Struct() = default;

       public:
        virtual bool fromJson(const QJsonObject &) override;
        virtual QJsonObject toJson() const override;

       public:
        MAKE_ENTITY_ATTRIBUTE(QString, name);

       public:
        void addValue(const QString &name, const QString &type) { m_values.insert(std::make_pair(name, type)); }
        void removeValue(const QString &name) { m_values.erase(name); }
        size_t get_size() const;
        std::map<QString, QString> get_values() const { return m_values; }

       private:
        std::map<QString, QString> m_values;
    };
}  // namespace Entity

#endif  // STRUCT_H
