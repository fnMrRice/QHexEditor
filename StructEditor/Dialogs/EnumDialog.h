#ifndef ENUMDIALOG_H
#define ENUMDIALOG_H

#include <QDialog>

#include "Entity/Enum.h"

namespace Ui {
    class EnumDialog;
}

class EnumDialog : public QDialog {
    Q_OBJECT

   public:
    explicit EnumDialog(QWidget *parent = nullptr);
    explicit EnumDialog(const std::shared_ptr<Entity::Enum> &value, QWidget *parent = nullptr);
    ~EnumDialog();

   private:
    void initSignals();

   private slots:
    void slot_onAddOne();
    void slot_onRemoveOne();
    void slot_onAcepted();

   signals:
    void signal_dataFinished(const std::shared_ptr<Entity::Enum> &);

   private:
    Ui::EnumDialog *ui;
};

#endif  // ENUMDIALOG_H
