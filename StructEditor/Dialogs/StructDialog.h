#ifndef STRUCTDIALOG_H
#define STRUCTDIALOG_H

#include <QDialog>

#include "Entity/Struct.h"

namespace Ui {
    class StructDialog;
}

class StructDialog : public QDialog {
    Q_OBJECT

   public:
    explicit StructDialog(QWidget *parent = nullptr);
    explicit StructDialog(const std::shared_ptr<Entity::Struct> &value, QWidget *parent = nullptr);
    ~StructDialog();

   private:
    void initSignals();

   private slots:
    void slot_onAddOne();
    void slot_onRemoveOne();
    void slot_onAcepted();

   signals:
    void signal_dataFinished(const std::shared_ptr<Entity::Struct> &);

   private:
    Ui::StructDialog *ui;
};

#endif  // STRUCTDIALOG_H
