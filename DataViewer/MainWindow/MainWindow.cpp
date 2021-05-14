#include "MainWindow.h"

#include <QFileDialog>
#include <QStandardPaths>

#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow) {
    ui->setupUi(this);

    initMenu();
    initSignals();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::initMenu() {
    m_file_list_menu = std::make_unique<QMenu>(ui->o_fileList);
    //    m_action_save = std::make_unique<QAction>(tr("Save"));
    //    m_action_close = std::make_unique<QAction>(tr("Close"));
    m_file_list_menu->addActions({ui->actionSave, ui->actionClose});
}

void MainWindow::initSignals() {
    connect(ui->o_fileList, &QListWidget::itemClicked, this, &MainWindow::slot_onFileSelected);
    connect(ui->o_fileList, &QListWidget::customContextMenuRequested, this, &MainWindow::slot_onFileListMenu);
    connect(ui->tabWidget, &QTabWidget::tabBarClicked, this, &MainWindow::slot_onTabChanged);

    connect(ui->action_Open, &QAction::triggered, this, &MainWindow::slot_onOpenFile);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::slot_onSaveFile);
    connect(ui->actionClose, &QAction::triggered, this, &MainWindow::slot_onCloseFile);

    //    connect(m_action_save.get(), &QAction::triggered, this, &MainWindow::slot_onSaveFile);
    //    connect(m_action_close.get(), &QAction::triggered, this, &MainWindow::slot_onCloseFile);
}

void MainWindow::slot_onOpenFile() {
    auto const &homes = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    QString homePath;
    if (!homes.isEmpty()) {
        homePath = homes[0];
    }
    auto path = QFileDialog::getOpenFileName(this, tr("Open File"), QStandardPaths::standardLocations(QStandardPaths::HomeLocation).at(0));
    if (path.isNull()) return;  // user cancel

    ui->o_fileList->addItem(path);
    auto fileName = path.split("/").last();
    auto file = std::make_shared<QFile>(path);
    auto widget = std::make_shared<FileViewWidget>(file, this);
    m_widgets.insert(std::make_pair(ui->o_fileList->count() - 1, widget));
    ui->tabWidget->addTab(widget.get(), fileName);

    if (ui->tabWidget->isTabVisible(0)) {  // first time to open a file
        ui->tabWidget->setEnabled(true);
        ui->tabWidget->setTabVisible(0, false);  // from Qt5.15, cannot be used before this
    }
}

void MainWindow::slot_onFileListMenu() {
    auto row = ui->o_fileList->currentRow();
    if (row == -1) return;
    m_file_list_menu->exec(QCursor::pos());
}

void MainWindow::slot_onFileSelected(QListWidgetItem *item) {
    auto const &row = ui->o_fileList->row(item);
    ui->tabWidget->setCurrentIndex(row + 1);
}

void MainWindow::slot_onTabChanged(int index) {
    ui->o_fileList->setCurrentRow(index - 1);
}

void MainWindow::slot_onSaveFile() {
    // save backup file first
    std::shared_ptr<QFile> backup_file;
    if (ui->actionEnable_Backup->isChecked()) {  // save backup file
        // select backup file
        // read file and calculate checksum
        // write checksum to backup file
        // write modified data to backup file
    }

    // save file here

    if (ui->actionEnable_Backup->isChecked()) {  // save backup file
        // read saved file and calculate checksum
        // write checksum
    }
}

void MainWindow::slot_onCloseFile() {
    auto row = ui->o_fileList->currentRow();
    if (row == -1) return;
    auto iter = m_widgets.find(row);
    if (iter == m_widgets.end()) return;

    // begin remove
    m_widgets.erase(iter);
    //    ui->o_fileList->removeItemWidget(ui->o_fileList->currentItem());
    qDeleteAll(ui->o_fileList->selectedItems());
    if (!m_widgets.size()) {  // no opened file now
        ui->tabWidget->setEnabled(false);
        ui->tabWidget->setTabVisible(0, true);
    }
}
