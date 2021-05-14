#include "Enum.h"

#include <QJsonArray>

namespace Entity {

    Enum::Enum(const QJsonObject &json) {
        auto const &name = json.value("name");
        auto const &size = json.value("size");
        auto const &values = json.value("values");
        if (name.isString()) {
            m_name = name.toString();
        }
        if (size.isDouble()) {
            m_size = static_cast<size_t>(size.toInt());
        }
        if (values.isArray()) {
            for (auto const &data : values.toArray()) {
                if (data.isObject()) {
                    auto const &json = data.toObject();
                    auto const &name = json.value("name");
                    auto const &value = json.value("value");
                    if (name.isString() && value.isDouble()) {
                        m_values.insert(std::make_pair(value.toInt(), name.toString()));
                    }
                }
            }
        }
    }

    Enum::Enum(const QString &name, const size_t &size) : m_name(name), m_size(size) {}

    bool Enum::fromJson(const QJsonObject &json) {
        auto const &name = json.value("name");
        auto const &size = json.value("size");
        auto const &values = json.value("values");
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
        if (values.isArray()) {
            for (auto const &data : values.toArray()) {
                if (data.isObject()) {
                    auto const &json = data.toObject();
                    auto const &name = json.value("name");
                    auto const &value = json.value("value");
                    if (name.isString() && value.isDouble()) {
                        m_values.insert(std::make_pair(value.toInt(), name.toString()));
                    } else {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    QJsonObject Enum::toJson() const {
        QJsonObject json;
        json.insert("name", m_name);
        json.insert("size", static_cast<int>(m_size));
        QJsonArray values;
        for (auto const &[value, text] : m_values) {
            QJsonObject obj;
            obj.insert("name", text);
            obj.insert("value", value);
            values.append(obj);
        }
        json.insert("values", values);
        return json;
    }

    void Enum::removeEnum(const QString &name) {
        for (auto iter = m_values.begin(); iter != m_values.end(); iter++) {
            if (iter->second == name) {
                m_values.erase(iter);
                return;
            }
        }
    }
}  // namespace Entity
