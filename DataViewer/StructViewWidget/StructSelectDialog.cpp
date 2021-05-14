#include "StructSelectDialog.h"

#include <DataReader.h>

#include "ui_StructSelectDialog.h"

StructSelectDialog::StructSelectDialog(QWidget *parent) : QDialog(parent),
                                                          ui(new Ui::StructSelectDialog) {
    ui->setupUi(this);

    auto dr = DataReader::instance();
    dr->beginRealoadData();
    dr->reloadAliases();
    dr->reloadEnums();
    auto items = dr->reloadStructs();
    dr->endReloadData();

    for (auto const &item : items) {
        auto row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(item->get_name()));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(item->get_size())));
    }

    connect(this, &StructSelectDialog::accepted, this, &StructSelectDialog::slot_onAccepted);
}

StructSelectDialog::~StructSelectDialog() {
    delete ui;
}

void StructSelectDialog::slot_onAccepted() {
    auto row = ui->tableWidget->currentRow();
    auto name = ui->tableWidget->item(row, 0)->text();

    auto dr = DataReader::instance();
    emit signal_structAccepted(dr->getStruct(name));
}
