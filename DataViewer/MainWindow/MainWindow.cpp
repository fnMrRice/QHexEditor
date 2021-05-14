#include "MainWindow.h"

#include <QFileDialog>
#include <QStandardPaths>

#include "Entity/BackupInfo.h"
#include "Utils/RestoreThread.h"
#include "Utils/SaveBackupFileThread.h"
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
    connect(ui->actionRestore_File, &QAction::triggered, this, &MainWindow::slot_onRestoreFile);

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
    // find current file viewer widget
    auto row = ui->o_fileList->currentRow();
    if (row == -1) return;
    auto iter = m_widgets.find(row);
    if (iter == m_widgets.end()) return;
    auto widget = iter->second;

    if (ui->actionEnable_Backup->isChecked()) {
        // save backup file, this needs md5 checksum, which may take a long time.
        // so do these works on a new thread

        // select backup file
        auto const &homes = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
        QString homePath;
        if (!homes.isEmpty()) {
            homePath = homes[0];
        }
        auto path = QFileDialog::getSaveFileName(this, tr("Choose to save backup file"), homePath);

        // init vairants the thread needs
        auto backup_file = std::make_shared<QFile>(path);
        auto file = widget->file();
        auto mod = widget->modified();
        auto backupInfo = std::make_shared<BackupInfo>();
        backupInfo->set_modification(mod);

        // can move to a new thread now.
        auto thread = new SaveBackupFileThread(backup_file, file, backupInfo);
        thread->start();
    } else {
        // save the file only
        auto file = widget->file();
        auto mod = widget->modified();
        if (!file->isWritable()) {
            if (file->isOpen()) {
                file->close();
                file->open(QIODevice::ReadWrite);
            } else {
                file->open(QIODevice::ReadWrite);
            }
        }

        for (auto const &[pos, values] : mod) {
            auto const &[before, after] = values;
            file->seek(pos);
            file->write(QByteArray {reinterpret_cast<const char *>(&after), sizeof(after)});
        }
        file->close();
        file->open(QIODevice::ReadOnly);
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

void MainWindow::slot_onRestoreFile() {
    // find current file viewer widget
    auto row = ui->o_fileList->currentRow();
    if (row == -1) return;
    auto iter = m_widgets.find(row);
    if (iter == m_widgets.end()) return;
    auto widget = iter->second;

    // select backup file
    auto const &homes = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    QString homePath;
    if (!homes.isEmpty()) {
        homePath = homes[0];
    }
    auto path = QFileDialog::getOpenFileName(this, tr("Choose to save backup file"), homePath);

    // restore also needs to be done in a thread
    auto bak_file = std::make_shared<QFile>(path);
    auto file = widget->file();

    auto thread = new RestoreThread(bak_file, file);
    thread->start();
}
