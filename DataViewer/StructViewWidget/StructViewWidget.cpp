#include "StructViewWidget.h"

#include <DataReader.h>

#include "ui_StructViewWidget.h"

StructViewWidget::StructViewWidget(const std::shared_ptr<Entity::Struct> &item,
                                   const std::shared_ptr<FileReader> file,
                                   QWidget *parent) : QWidget(parent),
                                                      ui(new Ui::StructViewWidget),
                                                      m_reader(file),
                                                      m_item(item) {
    ui->setupUi(this);
}

StructViewWidget::~StructViewWidget() {
    delete ui;
}

void StructViewWidget::loadStruct(const std::shared_ptr<Entity::Struct> &item, const char *value, QTreeWidgetItem *parent) {
    decltype(parent) root = nullptr;
    auto dr = DataReader::instance();

    if (parent) {
        root = parent;
    } else {
        dr->beginRealoadData();
        dr->reloadAliases();
        dr->reloadEnums();
        dr->reloadStructs();
        dr->endReloadData();

        ui->treeWidget->addTopLevelItem(new QTreeWidgetItem(QStringList {item->get_name(), "", QString::number(item->get_size())}));
        root = ui->treeWidget->topLevelItem(0);
    }

    for (auto const &[name, type] : item->get_values()) {
        if (auto item = dr->getAlias(type)) {  // is alias
            auto const &size = item->get_size();
            QString result;
            if (item->get_type() == ALIAS_TYPE::AT_NUMBER) {
                if (size == 1) {
                    result = QString::number(*reinterpret_cast<const int8_t *>(value));
                } else if (size == 2) {
                    result = QString::number(*reinterpret_cast<const int16_t *>(value));
                } else if (size == 4) {
                    result = QString::number(*reinterpret_cast<const int32_t *>(value));

                } else {
                    for (size_t i = 0; i < size; i++) {
                        auto constexpr HEX_DIGITS = "0123456789ABCDEF";
                        auto const &current = value[i];
                        result.prepend(HEX_DIGITS[current & 0x0F]);
                        result.prepend(HEX_DIGITS[current >> 4 & 0x0F]);
                    }
                    result.prepend("0x");
                }
            } else {  // character, treat as a char array
                auto char_data = new char[size + 1];
                memset(char_data, 0, size + 1);
                memcpy(char_data, value, size);
                result = char_data;
                delete[] char_data;
            }
            root->addChild(new QTreeWidgetItem({item->get_name(), result, QString::number(item->get_size())}));
            value += item->get_size();
            continue;
        }
        if (auto item = dr->getStruct(type)) {  // struct
            QTreeWidgetItem *child = new QTreeWidgetItem({item->get_name(), "", QString::number(item->get_size())});
            root->addChild(child);
            loadStruct(item, value, child);
            continue;
        }
        if (auto item = dr->getEnum(type)) {  // enum
            auto values = item->get_values();
            auto const &size = item->get_size();
            int64_t result = 0;
            if (size == 1) {
                result = *reinterpret_cast<const int8_t *>(value);
            } else if (size == 2) {
                result = *reinterpret_cast<const int16_t *>(value);
            } else if (size == 4) {
                result = *reinterpret_cast<const int32_t *>(value);
            } else if (size == 8) {
                result = *reinterpret_cast<const int64_t *>(value);
            } else {
                continue;
            }
            if (auto res = values.find(result); res != values.end()) {
                QString result_str = QString("%1(%2)").arg(res->second).arg(result);
                root->addChild(new QTreeWidgetItem({item->get_name(), result_str, QString::number(item->get_size())}));
            } else {
                QString result_str = QString("%1").arg(result);
                root->addChild(new QTreeWidgetItem({item->get_name(), result_str, QString::number(item->get_size())}));
            }
            value += item->get_size();
            continue;
        }
        qWarning() << "no valid name can be found.";
    }
}

void StructViewWidget::slot_onCursorChanged(size_t pos) {
    auto const &size = m_item->get_size();
    if (0x3FFF <= size) return;  // do not support so big struct
    auto data = m_reader->readBytes(pos, size);
    auto topCount = ui->treeWidget->topLevelItemCount();
    while (topCount) {
        delete ui->treeWidget->topLevelItem(0);
        topCount = ui->treeWidget->topLevelItemCount();
    }
    loadStruct(m_item, data.data());
}
