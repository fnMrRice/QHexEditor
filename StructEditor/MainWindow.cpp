#include "MainWindow.h"

#include <QMenu>

#include "DataReader.h"
#include "Dialogs/AliasDialog.h"
#include "Dialogs/EnumDialog.h"
#include "Dialogs/StructDialog.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    ui->toolBar->setVisible(false);

    initSignals();
    initTableSettings();
    updateWidgetVisiable();
    updateDiaplayData();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::initSignals() {
    connect(ui->a_showAsTable, &QAction::triggered, this, &MainWindow::slot_onShowTypeChanged);
    connect(ui->a_showAsText, &QAction::triggered, this, &MainWindow::slot_onShowTypeChanged);

    connect(ui->ta_table, &QTableWidget::customContextMenuRequested, this, &MainWindow::slot_showMenu);
    connect(ui->te_table, &QTableWidget::customContextMenuRequested, this, &MainWindow::slot_showMenu);
    connect(ui->ts_table, &QTableWidget::customContextMenuRequested, this, &MainWindow::slot_showMenu);
}

void MainWindow::initTableSettings() {
    ui->ta_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->ts_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->te_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::updateWidgetVisiable() {
    if (ui->a_showAsTable->isChecked()) {
        ui->ta_table->setVisible(true);
        ui->te_table->setVisible(true);
        ui->ts_table->setVisible(true);
        ui->ta_text->setVisible(false);
        ui->te_text->setVisible(false);
        ui->ts_text->setVisible(false);
    } else if (ui->a_showAsText->isChecked()) {
        ui->ta_table->setVisible(false);
        ui->te_table->setVisible(false);
        ui->ts_table->setVisible(false);
        ui->ta_text->setVisible(true);
        ui->te_text->setVisible(true);
        ui->ts_text->setVisible(true);
    } else {  // set default
        ui->a_showAsTable->setChecked(true);
        ui->a_showAsText->setChecked(false);
        updateWidgetVisiable();
    }
}

void MainWindow::updateDiaplayData() {
    auto const &dr = DataReader::instance();

    dr->beginRealoadData();
    {
        ui->ta_table->setRowCount(0);
        int row = 0;
        auto const &aliases = dr->reloadAliases();
        for (auto const &item : aliases) {
            auto const &name = item->get_name();
            auto const &size = item->get_size();
            auto const &type = item->get_type();

            ui->ta_table->insertRow(row);
            ui->ta_table->setItem(row, 0, new QTableWidgetItem(name));
            ui->ta_table->setItem(row, 1, new QTableWidgetItem(QString::number(size)));
            QString typeString;
            switch (type) {
            case ALIAS_TYPE::AT_CHARACTER: typeString = "Character"; break;
            case ALIAS_TYPE::AT_NUMBER: typeString = "Number"; break;
            default: break;
            }
            ui->ta_table->setItem(row, 2, new QTableWidgetItem(typeString));
            ++row;
        }
    }

    {
        ui->te_table->setRowCount(0);
        int row = 0;
        auto const &enums = dr->reloadEnums();
        for (auto const &item : enums) {
            auto const &name = item->get_name();
            auto const &size = item->get_size();

            ui->te_table->insertRow(row);
            ui->te_table->setItem(row, 0, new QTableWidgetItem(name));
            ui->te_table->setItem(row, 1, new QTableWidgetItem(QString::number(size)));
            ++row;
        }
    }

    {
        ui->ts_table->setRowCount(0);
        int row = 0;
        auto const &structs = dr->reloadStructs();
        for (auto const &item : structs) {
            auto const &name = item->get_name();
            auto const &size = item->get_size();

            ui->ts_table->insertRow(row);
            ui->ts_table->setItem(row, 0, new QTableWidgetItem(name));
            ui->ts_table->setItem(row, 1, new QTableWidgetItem(QString::number(size)));
            ++row;
        }
    }
    dr->endReloadData();
}

void MainWindow::slot_onShowTypeChanged() {
    auto action = static_cast<QAction *>(sender());
    if (action == ui->a_showAsTable) {
        ui->a_showAsTable->setChecked(true);
        ui->a_showAsText->setChecked(false);
    } else if (action == ui->a_showAsText) {
        ui->a_showAsTable->setChecked(false);
        ui->a_showAsText->setChecked(true);
    }
    updateWidgetVisiable();
}

void MainWindow::slot_showMenu() {
    if (!m_menu) {
        m_action_add = std::make_unique<QAction>(tr("Add"));
        m_action_edit = std::make_unique<QAction>(tr("Edit"));
        m_action_remove = std::make_unique<QAction>(tr("Remove"));
        connect(m_action_add.get(), &QAction::triggered, this, &MainWindow::slot_onAddOne);
        connect(m_action_edit.get(), &QAction::triggered, this, &MainWindow::slot_onEditOne);
        connect(m_action_remove.get(), &QAction::triggered, this, &MainWindow::slot_onRemoveOne);

        m_menu = std::make_unique<QMenu>(this);
        m_menu->addActions({m_action_add.get(), m_action_edit.get(), m_action_remove.get()});
    }

    switch (ui->tabWidget->currentIndex()) {
    case 0: {
        auto const &selected = ui->ta_table->selectedItems();
        auto const &empty = selected.isEmpty();
        m_action_edit->setVisible(!empty);
        m_action_remove->setVisible(!empty);
        break;
    }
    case 1: {
        auto const &selected = ui->ts_table->selectedItems();
        auto const &empty = selected.isEmpty();
        m_action_edit->setVisible(!empty);
        m_action_remove->setVisible(!empty);
        break;
    }
    case 2: {
        auto const &selected = ui->te_table->selectedItems();
        auto const &empty = selected.isEmpty();
        m_action_edit->setVisible(!empty);
        m_action_remove->setVisible(!empty);
        break;
    }
    default: break;
    }
    m_menu->exec(QCursor::pos());
}

void MainWindow::slot_onAddOne() {
    switch (ui->tabWidget->currentIndex()) {
    case 0: {
        auto dialog = new AliasDialog(this);
        dialog->setWindowTitle(tr("Add Alias"));
        connect(dialog, &AliasDialog::signal_dataFinished, this, [this](auto data) {
            DataReader::instance()->addAlias(data);
            updateDiaplayData();
        });
        dialog->show();
    } break;
    case 1: {
        auto dialog = new StructDialog(this);
        dialog->setWindowTitle(tr("Add Struct"));
        connect(dialog, &StructDialog::signal_dataFinished, this, [this](auto data) {
            DataReader::instance()->addStruct(data);
            updateDiaplayData();
        });
        dialog->show();
    } break;
    case 2: {
        auto dialog = new EnumDialog(this);
        dialog->setWindowTitle(tr("Add Enum"));
        connect(dialog, &EnumDialog::signal_dataFinished, this, [this](auto data) {
            DataReader::instance()->addEnum(data);
            updateDiaplayData();
        });
        dialog->show();
    } break;
    default: return;
    }
}

void MainWindow::slot_onRemoveOne() {
    auto const &dr = DataReader::instance();
    switch (ui->tabWidget->currentIndex()) {
    case 0: {
        auto const &selected = ui->ta_table->selectedItems();
        if (selected.isEmpty()) return;
        auto const &name = ui->ta_table->item(selected[0]->row(), 0)->text();
        auto const &item = dr->getAlias(name);
        dr->removeAlias(item);
        break;
    }
    case 1: {
        auto const &selected = ui->ts_table->selectedItems();
        if (selected.isEmpty()) return;
        auto const &name = ui->ts_table->item(selected[0]->row(), 0)->text();
        auto const &item = dr->getStruct(name);
        dr->removeStruct(item);
        break;
    }
    case 2: {
        auto const &selected = ui->te_table->selectedItems();
        if (selected.isEmpty()) return;
        auto const &name = ui->te_table->item(selected[0]->row(), 0)->text();
        auto const &item = dr->getEnum(name);
        dr->removeEnum(item);
        break;
    }
    default: break;
    }
    updateDiaplayData();
}

void MainWindow::slot_onEditOne() {
    auto const &dr = DataReader::instance();
    switch (ui->tabWidget->currentIndex()) {
    case 0: {
        auto const &selected = ui->ta_table->selectedItems();
        if (selected.isEmpty()) return;
        auto const &name = ui->ta_table->item(selected[0]->row(), 0)->text();
        auto item = dr->getAlias(name);
        auto dialog = new AliasDialog(item, this);
        dialog->setWindowTitle(tr("Edit Alias"));
        connect(dialog, &AliasDialog::signal_dataFinished, this, [this, item](auto data) {
            DataReader::instance()->updateAlias(item, data);
            updateDiaplayData();
        });
        dialog->show();
    } break;
    case 1: {
        auto const &selected = ui->ts_table->selectedItems();
        if (selected.isEmpty()) return;
        auto const &name = ui->ts_table->item(selected[0]->row(), 0)->text();
        auto item = dr->getStruct(name);
        auto dialog = new StructDialog(item, this);
        dialog->setWindowTitle(tr("Edit Struct"));
        connect(dialog, &StructDialog::signal_dataFinished, this, [this, item](auto data) {
            DataReader::instance()->updateStruct(item, data);
            updateDiaplayData();
        });
        dialog->show();
    } break;
    case 2: {
        auto const &selected = ui->te_table->selectedItems();
        if (selected.isEmpty()) return;
        auto const &name = ui->te_table->item(selected[0]->row(), 0)->text();
        auto item = dr->getEnum(name);
        auto dialog = new EnumDialog(item, this);
        dialog->setWindowTitle(tr("Edit Enum"));
        connect(dialog, &EnumDialog::signal_dataFinished, this, [this, item](auto data) {
            DataReader::instance()->updateEnum(item, data);
            updateDiaplayData();
        });
        dialog->show();
    } break;
    default: break;
    }
}
