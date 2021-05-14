#include "Alias.h"

namespace Entity {
    Alias::Alias(const QJsonObject& json) {
        auto const& name = json.value("name");
        auto const& size = json.value("size");
        auto const& type = json.value("type");
        if (name.isString()) {
            m_name = name.toString();
        }
        if (size.isDouble()) {
            m_size = static_cast<size_t>(size.toInt());
        }
        if (type.isDouble()) {
            m_type = static_cast<ALIAS_TYPE>(type.toInt());
        }
    }

    Alias::Alias(const QString& name, const size_t& size, const ALIAS_TYPE& type) : m_name(name), m_size(size), m_type(type) {}

    bool Alias::fromJson(const QJsonObject& json) {
        auto const& name = json.value("name");
        auto const& size = json.value("size");
        auto const& type = json.value("type");
        if (name.isString()) {
            m_name = name.toString();
        } else {
            return false;
        }
        if (size.isDouble()) {
            m_size = static_cast<size_t>(size.toInt());
        } else {
            return false;
        }
        if (type.isDouble()) {
            m_type = static_cast<ALIAS_TYPE>(type.toInt());
        } else {
            return false;
        }
        return true;
    }

    QJsonObject Alias::toJson() const {
        QJsonObject obj;
        obj.insert("name", m_name);
        obj.insert("size", static_cast<int>(m_size));
        obj.insert("type", static_cast<int>(m_type));
        return obj;
    }
}  // namespace Entity
