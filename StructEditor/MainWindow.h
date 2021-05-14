#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

   private:
    void initSignals();
    void initTableSettings();
    void updateWidgetVisiable();
    void updateDiaplayData();

   private slots:
    void slot_onShowTypeChanged();
    void slot_showMenu();
    void slot_onAddOne();
    void slot_onRemoveOne();
    void slot_onEditOne();

   private:
    Ui::MainWindow *ui;
    std::unique_ptr<QMenu> m_menu;
    std::unique_ptr<QAction> m_action_add, m_action_edit, m_action_remove;
};
#endif  // MAINWINDOW_H
