#ifndef FILEVIEWWIDGET_H
#define FILEVIEWWIDGET_H

#include <QAction>
#include <QMenu>
#include <QWidget>
#include <set>

#include "StructViewWidget/StructViewWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class Widget;
}
QT_END_NAMESPACE

class QFile;
class QVBoxLayout;
class DataRenderWidget;

class FileViewWidget : public QWidget {
    Q_OBJECT

   public:
    FileViewWidget(const std::shared_ptr<QFile> &file, QWidget *parent = nullptr);
    ~FileViewWidget();

   public:
    std::shared_ptr<QFile> file() const { return m_file; }
    std::unordered_map<size_t, std::pair<uint8_t, uint8_t>> modified() const;

   private:
    void addStructTab(const int &tab);
    void removeStructTab(const int &tab);

   private slots:
    void slot_onCursorPosChanged(size_t pos);
    void slot_onStructViewMenu(const QPoint &pos);

   private:
    Ui::Widget *ui;
    std::shared_ptr<QFile> m_file;
    std::unique_ptr<QVBoxLayout> m_layout;
    std::unique_ptr<DataRenderWidget> m_renderWidget;

    std::unique_ptr<QMenu> m_struct_menu;
    std::unique_ptr<QAction> m_action_add, m_action_close;

    std::set<std::shared_ptr<StructViewWidget>> m_viewers;
};
#endif  // FILEVIEWWIDGET_H
