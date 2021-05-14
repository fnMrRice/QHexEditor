#ifndef IBASEENTITY_H
#define IBASEENTITY_H

#include <QJsonObject>

namespace Entity {
    class IBaseEntity {
       public:
        IBaseEntity() = default;
        virtual ~IBaseEntity() = default;

       public:
        virtual QJsonObject toJson() const = 0;
        virtual bool fromJson(const QJsonObject&) = 0;
    };
}  // namespace Entity

#endif  // IBASEENTITY_H
