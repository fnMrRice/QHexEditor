#ifndef ALIASDIALOG_H
#define ALIASDIALOG_H

#include <QDialog>

#include "Entity/Alias.h"

namespace Ui {
    class AliasDialog;
}

class AliasDialog : public QDialog {
    Q_OBJECT

   public:
    explicit AliasDialog(QWidget *parent = nullptr);
    explicit AliasDialog(const std::shared_ptr<Entity::Alias> &value, QWidget *parent = nullptr);
    ~AliasDialog();

   private slots:
    void slot_onAcepted();

   signals:
    void signal_dataFinished(const std::shared_ptr<Entity::Alias> &);

   private:
    Ui::AliasDialog *ui;
};

#endif  // ALIASDIALOG_H
