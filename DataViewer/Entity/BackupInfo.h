#ifndef BACKUPINFO_H
#define BACKUPINFO_H

#include <QByteArray>
#include <unordered_map>

class BackupInfo {
   public:
    BackupInfo();

   public:
    QByteArray serialize() const;
    bool deserialize(const QByteArray& data);

   public:
    void set_md5_before(const QByteArray& value) { m_md5_before = value; }
    QByteArray get_md5_before() const { return m_md5_before; }
    void set_md5_after(const QByteArray& value) { m_md5_after = value; }
    QByteArray get_md5_after() const { return m_md5_after; }
    void add_modification(const size_t& pos, const uint8_t& before, const uint8_t& after);
    void add_modification(const size_t& pos, const std::pair<uint8_t, uint8_t>& value);
    void add_modification(const std::pair<size_t, std::pair<uint8_t, uint8_t>>& data);
    void set_modification(const std::unordered_map<size_t, std::pair<uint8_t, uint8_t>>& value) { m_modifications = value; }
    std::unordered_map<size_t, std::pair<uint8_t, uint8_t>> get_modifications() const { return m_modifications; }

   private:
    QByteArray m_md5_before;
    QByteArray m_md5_after;
    std::unordered_map<size_t, std::pair<uint8_t, uint8_t>> m_modifications;
};

#endif  // BACKUPINFO_H
