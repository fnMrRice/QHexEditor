#ifndef SAVEBACKUPFILETHREAD_H
#define SAVEBACKUPFILETHREAD_H

#include <QFile>
#include <QThread>

#include "Entity/BackupInfo.h"

class SaveBackupFileThread : public QThread {
   public:
    SaveBackupFileThread(const std::shared_ptr<QFile>& backup_file,
                         const std::shared_ptr<QFile>& file,
                         const std::shared_ptr<BackupInfo>& bak_info);

   protected:
    virtual void run() override final;

   private:
    std::shared_ptr<QFile> m_backup_file, m_file;
    std::shared_ptr<BackupInfo> m_bak_info;
};

#endif  // SAVEBACKUPFILETHREAD_H
