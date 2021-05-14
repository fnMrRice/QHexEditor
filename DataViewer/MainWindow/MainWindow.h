#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFile>
#include <QListWidgetItem>
#include <QMainWindow>

#include "FileViewWidget.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

   private:
    void initMenu();
    void initSignals();

   private slots:
    void slot_onOpenFile();
    void slot_onFileListMenu();
    void slot_onFileSelected(QListWidgetItem *);
    void slot_onTabChanged(int index);
    void slot_onSaveFile();
    void slot_onCloseFile();
    void slot_onRestoreFile();

   private:
    Ui::MainWindow *ui;
    std::map<int, std::shared_ptr<FileViewWidget>> m_widgets;
    std::unique_ptr<QMenu> m_file_list_menu;
    std::unique_ptr<QAction> m_action_close, m_action_save;
};

#endif  // MAINWINDOW_H
