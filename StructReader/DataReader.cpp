#include "DataReader.h"

#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <type_traits>

#include "common/defines.h"

static auto constexpr ALIAS_FILE_NAME = R"(alias.json)";
static auto constexpr STRUCT_FILE_NAME = R"(strut.json)";
static auto constexpr ENUM_FILE_NAME = R"(enum.json)";

DataReader::DataReader() : QObject(nullptr) {}

template <class _Ty>
std::vector<std::shared_ptr<_Ty>> DataReader::readAnyAll() {
    const char *FilePath = nullptr;
    if constexpr (std::is_same_v<_Ty, Entity::Alias>) {
        for (auto const &[name, _] : m_aliases) {
            if (auto iter = m_names.find(name); iter != m_names.end()) {
                m_names.erase(iter);
            }
        }
        m_aliases.clear();
        FilePath = ALIAS_FILE_NAME;
    }
    if constexpr (std::is_same_v<_Ty, Entity::Struct>) {
        for (auto const &[name, _] : m_aliases) {
            if (auto iter = m_names.find(name); iter != m_names.end()) {
                m_names.erase(iter);
            }
        }
        m_structs.clear();
        FilePath = STRUCT_FILE_NAME;
    }
    if constexpr (std::is_same_v<_Ty, Entity::Enum>) {
        for (auto const &[name, _] : m_aliases) {
            if (auto iter = m_names.find(name); iter != m_names.end()) {
                m_names.erase(iter);
            }
        }
        m_enums.clear();
        FilePath = ENUM_FILE_NAME;
    }

    auto const &paths = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
    auto const &localPath = paths[0];
    auto const &path_prefix = localPath + "/" + STRUCT_PATH + "/";

    auto const &file = std::make_shared<QFile>(path_prefix + FilePath);
    file->open(QIODevice::ReadWrite);
    auto const &file_data = file->readAll();
    if (file_data.isEmpty()) {
        qInfo() << "empty file, now create it.";
        file->write(R"({"data":[],"count":0})");
        return {};
    }

    QJsonParseError err;
    auto const &jsonDoc = QJsonDocument::fromJson(file_data, &err);
    if (err.error != QJsonParseError::NoError) {
        qWarning() << "cannot read json file. please clear the file.";
        return {};
    }
    auto const &obj = jsonDoc.object();
    auto const &data_v = obj.value("data");
    if (!data_v.isArray()) {
        qWarning() << "wrong field \"data\" in json file.";
        return {};
    }
    auto const &count_v = obj.value("count");
    if (!count_v.isDouble()) {
        qWarning() << "wrong field \"count\" in json file.";
        return {};
    }

    auto const &count = count_v.toInt();
    if (!count) {
        qInfo() << "count is 0.";
        return {};
    }

    std::vector<std::shared_ptr<_Ty>> retval;
    auto const data = data_v.toArray();
    for (auto const &item : data) {
        if (!item.isObject()) {
            qWarning() << "\"data\" field is invalid.";
            continue;
        }
        auto const &obj = item.toObject();
        auto const &result = std::make_shared<_Ty>(obj);
        auto const &name = result->get_name();
        if (m_names.count(name)) {
            qWarning() << "name" << name << "already exists. skip this one.";
            continue;
        }
        m_names.insert(name);
        retval.push_back(result);
        if constexpr (std::is_same_v<_Ty, Entity::Alias>) {
            m_aliases.insert(std::make_pair(name, result));
        }
        if constexpr (std::is_same_v<_Ty, Entity::Struct>) {
            m_structs.insert(std::make_pair(name, result));
        }
        if constexpr (std::is_same_v<_Ty, Entity::Enum>) {
            m_enums.insert(std::make_pair(name, result));
        }
    }

    file->close();
    return retval;
}

void DataReader::beginRealoadData() {
    m_names.clear();
}

std::vector<std::shared_ptr<Entity::Alias>> DataReader::reloadAliases() { return readAnyAll<Entity::Alias>(); }
std::vector<std::shared_ptr<Entity::Struct>> DataReader::reloadStructs() { return readAnyAll<Entity::Struct>(); }
std::vector<std::shared_ptr<Entity::Enum>> DataReader::reloadEnums() { return readAnyAll<Entity::Enum>(); }

template <class _Ty, class _Any>
std::vector<std::shared_ptr<_Ty>> GetSecond(const std::map<_Any, std::shared_ptr<_Ty>> &all) {
    std::vector<std::shared_ptr<_Ty>> retval;
    for (auto const &[_, second] : all) {
        retval.push_back(second);
    }
    return retval;
}

std::map<QString, std::shared_ptr<Entity::Alias>> DataReader::getAliases() { return m_aliases; }   // return GetSecond(m_aliases); }
std::map<QString, std::shared_ptr<Entity::Struct>> DataReader::getStructs() { return m_structs; }  // return GetSecond(m_structs); }
std::map<QString, std::shared_ptr<Entity::Enum>> DataReader::getEnums() { return m_enums; }        // return GetSecond(m_enums); }

template <class _Ty, class _Any>
inline std::shared_ptr<_Ty> GetOneInMap(const std::map<_Any, std::shared_ptr<_Ty>> &all, const _Any &key) {
    if (auto iter = all.find(key); iter != all.end()) {
        return iter->second;
    }
    return nullptr;
}

std::shared_ptr<Entity::Alias> DataReader::getAlias(const QString &name) { return GetOneInMap(m_aliases, name); }
std::shared_ptr<Entity::Struct> DataReader::getStruct(const QString &name) { return GetOneInMap(m_structs, name); }
std::shared_ptr<Entity::Enum> DataReader::getEnum(const QString &name) { return GetOneInMap(m_enums, name); }

template <class _Ty>
bool DataReader::addAnyToMaps(std::map<QString, std::shared_ptr<_Ty>> &container, const std::shared_ptr<_Ty> &data) {
    auto const &name = data->get_name();
    if (m_names.count(name)) {
        qWarning() << "name duplicated." << name;
        return false;
    }
    container.insert(std::make_pair(name, data));
    saveToFile();
    return false;
}

bool DataReader::addAlias(const std::shared_ptr<Entity::Alias> &item) { return addAnyToMaps(m_aliases, item); }
bool DataReader::addStruct(const std::shared_ptr<Entity::Struct> &item) { return addAnyToMaps(m_structs, item); }
bool DataReader::addEnum(const std::shared_ptr<Entity::Enum> &item) { return addAnyToMaps(m_enums, item); }

template <class _Ty>
bool DataReader::updateAnyInMaps(std::map<QString, std::shared_ptr<_Ty>> &container, const std::shared_ptr<_Ty> &old_data, const std::shared_ptr<_Ty> &new_data) {
    auto const &old_name = old_data->get_name();
    auto const &new_name = new_data->get_name();
    if (old_name == new_name) {
        if (auto iter = container.find(old_name); iter != container.end()) {
            iter->second = new_data;
        } else {
            return false;
        }
    } else {
        if (m_names.count(new_name)) {
            qWarning() << "name duplicated." << new_name;
            return false;
        } else {
            m_names.erase(old_name);
            m_names.insert(new_name);
            container.erase(old_name);
            container.insert(std::make_pair(new_name, new_data));
        }
    }
    saveToFile();
    return true;
}

bool DataReader::updateAlias(const std::shared_ptr<Entity::Alias> &before, const std::shared_ptr<Entity::Alias> &after) { return updateAnyInMaps(m_aliases, before, after); }
bool DataReader::updateStruct(const std::shared_ptr<Entity::Struct> &before, const std::shared_ptr<Entity::Struct> &after) { return updateAnyInMaps(m_structs, before, after); }
bool DataReader::updateEnum(const std::shared_ptr<Entity::Enum> &before, const std::shared_ptr<Entity::Enum> &after) { return updateAnyInMaps(m_enums, before, after); }

template <class _Ty>
inline void DataReader::removeAnyInMaps(std::map<QString, std::shared_ptr<_Ty>> &container, const std::shared_ptr<_Ty> &data) {
    auto const &name = data->get_name();
    if (auto iter = container.find(name); iter != container.end()) {
        container.erase(iter);
    }
    if (auto iter = m_names.find(name); iter != m_names.end()) {
        m_names.erase(iter);
    }
    saveToFile();
}

void DataReader::removeAlias(const std::shared_ptr<Entity::Alias> &item) { return removeAnyInMaps(m_aliases, item); }
void DataReader::removeStruct(const std::shared_ptr<Entity::Struct> &item) { return removeAnyInMaps(m_structs, item); }
void DataReader::removeEnum(const std::shared_ptr<Entity::Enum> &item) { return removeAnyInMaps(m_enums, item); }

template <class _Ty, class _Any>
void SaveToFile(const std::map<_Any, std::shared_ptr<_Ty>> &container, const QString &file_name) {
    int counter = 0;
    QJsonArray arr;
    for (auto const &[name, item] : container) {
        arr.append(item->toJson());
        counter += 1;
    }
    QJsonObject obj;
    obj.insert("data", arr);
    obj.insert("count", counter);
    QJsonDocument doc;
    doc.setObject(obj);
    auto const &file = std::make_shared<QFile>(file_name);
    file->open(QIODevice::WriteOnly);
    file->write(doc.toJson());
    file->close();
}

void DataReader::saveToFile() {
    auto const &paths = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
    auto const &localPath = paths[0];
    auto const &path_prefix = localPath + "/" + STRUCT_PATH + "/";

    SaveToFile(m_aliases, path_prefix + ALIAS_FILE_NAME);
    SaveToFile(m_structs, path_prefix + STRUCT_FILE_NAME);
    SaveToFile(m_enums, path_prefix + ENUM_FILE_NAME);
}

std::shared_ptr<DataReader> DataReader::s_ptr;
std::mutex DataReader::s_mutex;
std::shared_ptr<DataReader> DataReader::instance() {
    if (s_ptr) { return s_ptr; }
    s_mutex.lock();
    if (!s_ptr) s_ptr = std::make_shared<DataReader>();
    s_mutex.unlock();
    return s_ptr;
}
