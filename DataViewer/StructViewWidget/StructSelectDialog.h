#ifndef STRUCTSELECTDIALOG_H
#define STRUCTSELECTDIALOG_H

#include <QDialog>

#include "Entity/Struct.h"

namespace Ui {
    class StructSelectDialog;
}

class StructSelectDialog : public QDialog {
    Q_OBJECT

   public:
    explicit StructSelectDialog(QWidget *parent = nullptr);
    ~StructSelectDialog();

   private:
    void slot_onAccepted();

   signals:
    void signal_structAccepted(const std::shared_ptr<Entity::Struct> &);

   private:
    Ui::StructSelectDialog *ui;
};

#endif  // STRUCTSELECTDIALOG_H
