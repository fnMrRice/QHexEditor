#include "SaveBackupFileThread.h"

#include "Utils/Utils.h"

SaveBackupFileThread::SaveBackupFileThread(const std::shared_ptr<QFile>& backup_file,
                                           const std::shared_ptr<QFile>& file,
                                           const std::shared_ptr<BackupInfo>& bak_info) : m_backup_file(backup_file),
                                                                                          m_file(file),
                                                                                          m_bak_info(bak_info) {}

void SaveBackupFileThread::run() {
    auto before_hash = Utils::GetHash(m_file);
    if (!m_file->isWritable()) {
        if (m_file->isOpen()) {
            m_file->close();
            m_file->open(QIODevice::ReadWrite);
        } else {
            m_file->open(QIODevice::ReadWrite);
        }
    }

    for (auto const& [pos, values] : m_bak_info->get_modifications()) {
        auto const& [before, after] = values;
        m_file->seek(pos);
        m_file->write(QByteArray {reinterpret_cast<const char*>(&after), sizeof(after)});
    }
    m_file->close();
    m_file->open(QIODevice::ReadOnly);
    auto after_hash = Utils::GetHash(m_file);

    m_bak_info->set_md5_before(before_hash);
    m_bak_info->set_md5_after(after_hash);

    if (!m_backup_file->isWritable()) {
        if (!m_backup_file->isOpen()) {
            m_backup_file->open(QIODevice::WriteOnly);

        } else {
            m_backup_file->close();
            m_backup_file->open(QIODevice::WriteOnly);
        }
    }
    m_backup_file->write(m_bak_info->serialize());
    m_backup_file->close();
}
