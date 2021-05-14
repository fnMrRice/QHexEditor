#ifndef RESTORETHREAD_H
#define RESTORETHREAD_H

#include <QFile>
#include <QThread>

#include "Entity/BackupInfo.h"

class RestoreThread : public QThread {
   public:
    RestoreThread(const std::shared_ptr<QFile>& backup_file,
                  const std::shared_ptr<QFile>& file);

   protected:
    virtual void run() override final;

   private:
    std::shared_ptr<QFile> m_backup_file, m_file;
};

#endif  // RESTORETHREAD_H
