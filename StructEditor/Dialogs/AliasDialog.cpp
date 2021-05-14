#include "AliasDialog.h"

#include "ui_AliasDialog.h"

AliasDialog::AliasDialog(QWidget *parent) : QDialog(parent),
                                            ui(new Ui::AliasDialog) {
    ui->setupUi(this);
    connect(this, &AliasDialog::accepted, this, &AliasDialog::slot_onAcepted);
}

AliasDialog::AliasDialog(const std::shared_ptr<Entity::Alias> &value, QWidget *parent) : AliasDialog(parent) {
    ui->io_name->setText(value->get_name());
    ui->io_size->setText(QString::number(value->get_size()));
    ui->io_type->setCurrentIndex(static_cast<int>(value->get_type()));
}

AliasDialog::~AliasDialog() {
    delete ui;
}

void AliasDialog::slot_onAcepted() {
    auto const &name = ui->io_name->displayText();
    auto const &size = ui->io_size->text();
    auto const &type = ui->io_type->currentIndex();
    emit signal_dataFinished(std::make_shared<Entity::Alias>(name, size.toInt(), static_cast<ALIAS_TYPE>(type)));
}
