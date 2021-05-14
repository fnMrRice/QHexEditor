#include "StructDialog.h"

#include "ui_StructDialog.h"

StructDialog::StructDialog(QWidget *parent) : QDialog(parent),
                                              ui(new Ui::StructDialog) {
    ui->setupUi(this);

    ui->io_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    initSignals();
}

StructDialog::StructDialog(const std::shared_ptr<Entity::Struct> &value, QWidget *parent) : StructDialog(parent) {
    ui->io_name->setText(value->get_name());
    ui->o_size->setText(QString::number(value->get_size()));

    ui->io_table->setRowCount(0);
    int row = 0;
    for (auto const &[name, type] : value->get_values()) {
        ui->io_table->insertRow(row);
        ui->io_table->setItem(row, 0, new QTableWidgetItem(name));
        ui->io_table->setItem(row, 1, new QTableWidgetItem(type));
    }
}

StructDialog::~StructDialog() {
    delete ui;
}

void StructDialog::initSignals() {
    connect(ui->btn_add, &QPushButton::clicked, this, &StructDialog::slot_onAddOne);
    connect(ui->btn_remove, &QPushButton::clicked, this, &StructDialog::slot_onRemoveOne);
    connect(this, &StructDialog::accepted, this, &StructDialog::slot_onAcepted);
}

void StructDialog::slot_onAddOne() {
    auto const &row = ui->io_table->rowCount();
    ui->io_table->insertRow(row);
    ui->io_table->setItem(row, 0, new QTableWidgetItem("NewItem"));
    ui->io_table->setItem(row, 1, new QTableWidgetItem("NewItemType"));
}

void StructDialog::slot_onRemoveOne() {
    auto const &selected = ui->io_table->selectedItems();
    if (selected.isEmpty()) return;
    auto const &item = selected[0];
    auto const &row = item->row();
    ui->io_table->removeRow(row);
}

void StructDialog::slot_onAcepted() {
    auto retval = std::make_shared<Entity::Struct>(ui->io_name->text());
    for (int i = 0; i < ui->io_table->rowCount(); i++) {
        auto const &name = ui->io_table->item(i, 0)->text();
        auto const &type = ui->io_table->item(i, 1)->text();
        retval->addValue(name, type);
    }
    emit signal_dataFinished(retval);
}
