#include "EnumDialog.h"

#include "ui_EnumDialog.h"

EnumDialog::EnumDialog(QWidget *parent) : QDialog(parent),
                                          ui(new Ui::EnumDialog) {
    ui->setupUi(this);
    initSignals();
}

EnumDialog::EnumDialog(const std::shared_ptr<Entity::Enum> &value, QWidget *parent) : EnumDialog(parent) {
    ui->io_name->setText(value->get_name());
    ui->io_size->setText(QString::number(value->get_size()));

    ui->io_table->setRowCount(0);
    int row = 0;
    for (auto const &[value, text] : value->get_values()) {
        ui->io_table->insertRow(row);
        ui->io_table->setItem(row, 0, new QTableWidgetItem(text));
        ui->io_table->setItem(row, 1, new QTableWidgetItem(QString::number(value)));
    }
}

EnumDialog::~EnumDialog() {
    delete ui;
}

void EnumDialog::initSignals() {
    connect(ui->btn_add, &QPushButton::clicked, this, &EnumDialog::slot_onAddOne);
    connect(ui->btn_remove, &QPushButton::clicked, this, &EnumDialog::slot_onRemoveOne);
    connect(this, &EnumDialog::accepted, this, &EnumDialog::slot_onAcepted);
}

void EnumDialog::slot_onAddOne() {
    auto const &row = ui->io_table->rowCount();
    ui->io_table->insertRow(row);
    ui->io_table->setItem(row, 0, new QTableWidgetItem("NewName"));
    ui->io_table->setItem(row, 1, new QTableWidgetItem("0"));
}

void EnumDialog::slot_onRemoveOne() {
    auto const &selected = ui->io_table->selectedItems();
    if (selected.isEmpty()) return;
    auto const &item = selected[0];
    auto const &row = item->row();
    ui->io_table->removeRow(row);
}

void EnumDialog::slot_onAcepted() {
    auto retval = std::make_shared<Entity::Enum>(ui->io_name->text(), ui->io_size->text().toInt());
    for (int i = 0; i < ui->io_table->rowCount(); i++) {
        auto const &name = ui->io_table->item(i, 0)->text();
        auto const &value = ui->io_table->item(i, 1)->text();
        retval->addEnum(name, value.toInt());
    }
    emit signal_dataFinished(retval);
}
