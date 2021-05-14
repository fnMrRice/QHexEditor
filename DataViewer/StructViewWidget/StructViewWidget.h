#ifndef STRUCTVIEWWIDGET_H
#define STRUCTVIEWWIDGET_H

#include <QFile>
#include <QTreeWidgetItem>
#include <QWidget>

#include "Entity/Struct.h"
#include "common/FileReader.h"

namespace Ui {
    class StructViewWidget;
}

class StructViewWidget : public QWidget, public std::enable_shared_from_this<StructViewWidget> {
    Q_OBJECT

   public:
    explicit StructViewWidget(const std::shared_ptr<Entity::Struct> &item,
                              const std::shared_ptr<FileReader> reader,
                              QWidget *parent = nullptr);
    ~StructViewWidget();

   private:
    void loadStruct(const std::shared_ptr<Entity::Struct> &item, const char *value, QTreeWidgetItem *parent = nullptr);

   public slots:
    void slot_onCursorChanged(size_t pos);

   private:
    Ui::StructViewWidget *ui;
    std::shared_ptr<FileReader> m_reader;
    std::shared_ptr<Entity::Struct> m_item;
};

#endif  // STRUCTVIEWWIDGET_H
