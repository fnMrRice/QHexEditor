#include "FileViewWidget.h"

#include <QFile>
#include <QTabBar>
#include <QVBoxLayout>

#include "DataViewWidget/DataRenderWidget.h"
#include "StructViewWidget/StructSelectDialog.h"
#include "ui_FileViewWidget.h"

FileViewWidget::FileViewWidget(const std::shared_ptr<QFile> &file, QWidget *parent)
    : QWidget(parent), ui(new Ui::Widget), m_file(file) {
    ui->setupUi(this);

    m_widget = std::make_unique<DataRenderWidget>(m_file, this);
    auto layout = ui->container->layout();
    layout->addWidget(m_widget.get());
    m_widget->show();

    ui->tabWidget->tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(m_widget.get(), &DataRenderWidget::signal_cursorPosChanged, this, &FileViewWidget::slot_onCursorPosChanged);
    connect(ui->tabWidget->tabBar(), &QTabBar::customContextMenuRequested, this, &FileViewWidget::slot_onStructViewMenu);
}

FileViewWidget::~FileViewWidget() {
    delete ui;
}

void FileViewWidget::addStructTab(const int &tab) {
    auto current = tab;
    if (current < 0) return;        // invalid
    if (current == 0) current = 1;  // do not insert before first tab
    auto dialog = new StructSelectDialog(this);
    connect(dialog, &StructSelectDialog::signal_structAccepted, this, [this, current](const std::shared_ptr<Entity::Struct> &item) {
        auto viewer = std::make_shared<StructViewWidget>(item, m_widget->reader(), this);
        m_viewers.insert(viewer);
        ui->tabWidget->insertTab(current, viewer.get(), item->get_name());
        connect(m_widget.get(), &DataRenderWidget::signal_cursorPosChanged, viewer.get(), &StructViewWidget::slot_onCursorChanged);
    });
    dialog->show();
}

void FileViewWidget::removeStructTab(const int &tab) {
    auto current = tab;
    if (current <= 0) return;  // cannot remove first tab
    auto widget = dynamic_cast<StructViewWidget *>(ui->tabWidget->widget(current));
    if (widget) {
        m_viewers.erase(widget->shared_from_this());
    }
    ui->tabWidget->removeTab(current);
}

void FileViewWidget::slot_onCursorPosChanged(size_t pos) {
    auto reader = m_widget->reader();
    auto begin = m_widget->begin() + pos;
    auto data = reader->readBytes(begin, 16);
    auto raw = static_cast<void *>(data.data());

#define READ_BYTE(widget, type)                       \
    type value_of_##type = *static_cast<type *>(raw); \
    ui->widget->setText(QString::number(value_of_##type));

    READ_BYTE(io_i8, int8_t);
    READ_BYTE(io_u8, uint8_t);
    READ_BYTE(io_i16, int16_t);
    READ_BYTE(io_u16, uint16_t);
    READ_BYTE(io_i32, int32_t);
    READ_BYTE(io_u32, uint32_t);
    READ_BYTE(io_i64, int64_t);
    READ_BYTE(io_u64, uint64_t);
    READ_BYTE(io_f32, float);
    READ_BYTE(io_f64, double);

    char value_of_char = *static_cast<char *>(raw);
    wchar_t value_of_wchar = *static_cast<wchar_t *>(raw);
    wchar_t arr[2] = {value_of_wchar, 0};
    ui->io_c8->setText(QString(value_of_char));
    ui->io_c16->setText(QString::fromStdWString(arr));

    ui->io_utf8->setText(QString(QString::fromUtf8(data)[0]));
    ui->io_utf16->setText(QString(QString::fromUtf16(static_cast<char16_t *>(raw), 8)[0]));

    ui->io_ansi->setText(QString(QString::fromLocal8Bit(data)[0]));
}

void FileViewWidget::slot_onStructViewMenu(const QPoint &pos) {
    if (!m_struct_menu) {
        m_struct_menu = std::make_unique<QMenu>(ui->tabWidget);
        m_action_add = std::make_unique<QAction>(tr("Add Struct"));
        m_action_close = std::make_unique<QAction>(tr("Close"));

        m_struct_menu->addActions({m_action_add.get(), m_action_close.get()});

        //        connect(m_action_add.get(), &QAction::triggered, this, &FileViewWidget::slot_onAddStruct);
        //        connect(m_action_close.get(), &QAction::triggered, this, &FileViewWidget::slot_onRemoveTab);
    }

    auto current = ui->tabWidget->tabBar()->tabAt(pos);
    m_action_add->disconnect(this);
    m_action_close->disconnect(this);

    connect(m_action_add.get(), &QAction::triggered, this, [this, current] {
        this->addStructTab(current);
    });
    connect(m_action_close.get(), &QAction::triggered, this, [this, current] {
        this->removeStructTab(current);
    });

    if (current <= 0) {
        m_action_close->setVisible(false);
    } else {
        m_action_close->setVisible(true);
    }
    m_struct_menu->exec(QCursor::pos());
}
