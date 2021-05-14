#include "RestoreThread.h"

#include "Utils/Utils.h"

RestoreThread::RestoreThread(const std::shared_ptr<QFile>& backup_file,
                             const std::shared_ptr<QFile>& file) : m_backup_file(backup_file),
                                                                   m_file(file) {}

void RestoreThread::run() {
    if (!m_backup_file->isReadable()) {
        if (!m_backup_file->isOpen()) {
            m_backup_file->open(QIODevice::ReadOnly);
        } else {
            m_backup_file->close();
            m_backup_file->open(QIODevice::ReadOnly);
        }
    }
    m_backup_file->seek(0);
    auto bak_data = m_backup_file->readAll();
    m_backup_file->close();

    auto bak_info = std::make_unique<BackupInfo>();
    if (!bak_info->deserialize(bak_data)) {
        qWarning() << "invalid backup file";
        return;
    }

    auto before_hash = Utils::GetHash(m_file);
    if (before_hash != bak_info->get_md5_after()) {
        qWarning() << "file modified after last time save a backup file.";
    }
    if (!m_file->isWritable()) {
        if (m_file->isOpen()) {
            m_file->close();
            m_file->open(QIODevice::ReadWrite);
        } else {
            m_file->open(QIODevice::ReadWrite);
        }
    }

    for (auto const& [pos, values] : bak_info->get_modifications()) {
        auto const& [before, after] = values;
        m_file->seek(pos);
        m_file->write(QByteArray {reinterpret_cast<const char*>(&before), sizeof(before)});
    }
    m_file->close();
    m_file->open(QIODevice::ReadOnly);
}
