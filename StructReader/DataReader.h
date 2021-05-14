#ifndef DATAREADER_H
#define DATAREADER_H

#include <QObject>
#include <memory>
#include <mutex>
#include <set>

#include "Entity/Alias.h"
#include "Entity/Enum.h"
#include "Entity/Struct.h"
#include "StructReader_global.h"

class STRUCTREADER_EXPORT DataReader : public QObject {
    Q_OBJECT
   public:
    explicit DataReader();
    DataReader(const DataReader &) = delete;

   public:
    void beginRealoadData();
    std::vector<std::shared_ptr<Entity::Alias>> reloadAliases();
    std::vector<std::shared_ptr<Entity::Struct>> reloadStructs();
    std::vector<std::shared_ptr<Entity::Enum>> reloadEnums();
    void endReloadData() {}

   public:
    std::map<QString, std::shared_ptr<Entity::Alias>> getAliases();
    std::map<QString, std::shared_ptr<Entity::Struct>> getStructs();
    std::map<QString, std::shared_ptr<Entity::Enum>> getEnums();
    std::shared_ptr<Entity::Alias> getAlias(const QString &name);
    std::shared_ptr<Entity::Struct> getStruct(const QString &name);
    std::shared_ptr<Entity::Enum> getEnum(const QString &name);

   public:
    bool addAlias(const std::shared_ptr<Entity::Alias> &);
    bool addStruct(const std::shared_ptr<Entity::Struct> &);
    bool addEnum(const std::shared_ptr<Entity::Enum> &);

   public:
    bool updateAlias(const std::shared_ptr<Entity::Alias> &before, const std::shared_ptr<Entity::Alias> &after);
    bool updateStruct(const std::shared_ptr<Entity::Struct> &before, const std::shared_ptr<Entity::Struct> &after);
    bool updateEnum(const std::shared_ptr<Entity::Enum> &before, const std::shared_ptr<Entity::Enum> &after);

   public:
    void removeAlias(const std::shared_ptr<Entity::Alias> &item);
    void removeStruct(const std::shared_ptr<Entity::Struct> &item);
    void removeEnum(const std::shared_ptr<Entity::Enum> &item);

   private:
    void saveToFile();

   private:
    template <class _Ty>
    std::vector<std::shared_ptr<_Ty>> readAnyAll();
    template <class _Ty>
    inline bool addAnyToMaps(std::map<QString, std::shared_ptr<_Ty>> &container, const std::shared_ptr<_Ty> &data);
    template <class _Ty>
    inline bool updateAnyInMaps(std::map<QString, std::shared_ptr<_Ty>> &container, const std::shared_ptr<_Ty> &old_data, const std::shared_ptr<_Ty> &new_data);
    template <class _Ty>
    inline void removeAnyInMaps(std::map<QString, std::shared_ptr<_Ty>> &container, const std::shared_ptr<_Ty> &data);

   private:
    std::map<QString, std::shared_ptr<Entity::Alias>> m_aliases;
    std::map<QString, std::shared_ptr<Entity::Struct>> m_structs;
    std::map<QString, std::shared_ptr<Entity::Enum>> m_enums;
    std::set<QString> m_names;

   public:
    static std::shared_ptr<DataReader> instance();

   private:
    static std::shared_ptr<DataReader> s_ptr;
    static std::mutex s_mutex;
};

#endif  // DATAREADER_H
