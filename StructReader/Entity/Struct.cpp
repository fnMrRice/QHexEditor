#include "Struct.h"

#include <QJsonArray>

#include "DataReader.h"

namespace Entity {

    Struct::Struct(const QString &name) : m_name(name) {}

    Struct::Struct(const QJsonObject &json) {
        auto const &name = json.value("name");
        auto const &values = json.value("values");
        if (name.isString()) {
            m_name = name.toString();
        }
        if (values.isArray()) {
            for (auto const &data : values.toArray()) {
                if (data.isObject()) {
                    auto const &json = data.toObject();
                    auto const &name = json.value("name");
                    auto const &type = json.value("type");
                    if (name.isString() && type.isString()) {
                        m_values.insert(std::make_pair(name.toString(), type.toString()));
                    }
                }
            }
        }
    }

    bool Struct::fromJson(const QJsonObject &json) {
        auto const &name = json.value("name");
        auto const &values = json.value("values");
        if (name.isString()) {
            m_name = name.toString();
        } else {
            return false;
        }
        if (values.isArray()) {
            for (auto const &data : values.toArray()) {
                if (data.isObject()) {
                    auto const &json = data.toObject();
                    auto const &name = json.value("name");
                    auto const &type = json.value("type");
                    if (name.isString() && type.isString()) {
                        m_values.insert(std::make_pair(name.toString(), type.toString()));
                    } else {
                        return false;
                    }
                } else {
                    return false;
                }
            }
        } else {
            return false;
        }
        return true;
    }

    QJsonObject Struct::toJson() const {
        QJsonObject json;
        json.insert("name", m_name);
        QJsonArray values;
        for (auto const &[name, type] : m_values) {
            QJsonObject obj;
            obj.insert("name", name);
            obj.insert("type", type);
            values.append(obj);
        }
        json.insert("values", values);
        return json;
    }

    size_t Struct::get_size() const {
        auto dr = DataReader::instance();
        auto const &a = dr->getAliases();
        auto const &s = dr->getStructs();
        auto const &e = dr->getEnums();
        size_t retval = 0;
        for (auto const &[_, type] : m_values) {
            if (auto aa = a.find(type); aa != a.end()) {
                retval += aa->second->get_size();
                continue;
            }
            if (auto ss = s.find(type); ss != s.end()) {
                retval += ss->second->get_size();
                continue;
            }
            if (auto ee = e.find(type); ee != e.end()) {
                retval += ee->second->get_size();
                continue;
            }
        }
        return retval;
    }
}  // namespace Entity
