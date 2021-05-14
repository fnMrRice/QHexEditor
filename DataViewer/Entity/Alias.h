#ifndef ALIAS_H
#define ALIAS_H
#include <QJsonObject>
#include <QString>

#include "IBaseEntity.h"
#include "common/defines.h"
#include "common/macros.h"

namespace Entity {
    class Alias : public IBaseEntity {
       public:
        Alias() = default;
        Alias(const QJsonObject &);
        Alias(const QString &name, const size_t &size, const ALIAS_TYPE &type);
        virtual ~Alias() = default;

       public:
        virtual bool fromJson(const QJsonObject &) override;
        virtual QJsonObject toJson() const override;

       public:
        MAKE_ENTITY_ATTRIBUTE(QString, name);
        MAKE_ENTITY_ATTRIBUTE(size_t, size) = 0;
        MAKE_ENTITY_ATTRIBUTE(ALIAS_TYPE, type) = ALIAS_TYPE::AT_NUMBER;
    };
}  // namespace Entity

#endif  // ALIAS_H
