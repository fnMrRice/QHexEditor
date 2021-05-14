#include "DataRenderWidget.h"

#include <QClipboard>
#include <QDebug>
#include <QFontDatabase>
#include <QFontMetrics>
#include <QGuiApplication>
#include <QPaintEvent>
#include <QPainter>

#include "ContextMenuController.h"
#include "Cursor/CursorController.h"
#include "Cursor/IBeamCursorOverlay.h"
#include "Utils/Utils.h"
#include "common/Settings.h"

DataRenderWidget::DataRenderWidget(const std::shared_ptr<QFile> &file, QWidget *parent) : QWidget(parent) {
    m_reader = std::make_shared<FileReader>(file);

    m_overlay = std::make_unique<IBeamCursorOverlay>(this);
    m_cursorController = std::make_unique<CursorController>(m_overlay.get(), this);
    m_menu = mk_u<ContextMenuController>(m_overlay.get(), this);

    connect(m_cursorController.get(), &CursorController::signal_cursorMoved,
            this, &DataRenderWidget::slot_onCursorMoved);
    connect(m_cursorController.get(), &CursorController::signal_textSelected,
            this, &DataRenderWidget::slot_onSelectionChanged);
    connect(m_cursorController.get(), &CursorController::signal_dataModified,
            this, &DataRenderWidget::slot_onDataModified);
    connect(m_cursorController.get(), &CursorController::signal_cursorIndexChanged,
            this, &DataRenderWidget::signal_cursorPosChanged);
    connect(m_cursorController.get(), &CursorController::signal_showUpRows,
            this, &DataRenderWidget::slot_onShowUpRows);
    connect(m_cursorController.get(), &CursorController::signal_showDownRows,
            this, &DataRenderWidget::slot_onShowDownRows);
    connect(m_menu.get(), &ContextMenuController::signal_copy,
            this, &DataRenderWidget::slot_onCopy);
    connect(m_menu.get(), &ContextMenuController::signal_paste,
            this, &DataRenderWidget::slot_onPaste);

    initSettings();
    //    initFakeData();
}

QSize DataRenderWidget::sizeHint() const {
    auto fs = m_reader->fileSize();
    auto h = (fs / m_rowSize) * (m_textSize.height() + m_rowMargin);
    h += m_textSize.height() + m_headerMarginBottom;
    auto w = (m_groupSize * m_size2.width() + m_textSize.width()) * (m_rowSize / m_groupSize);
    return QSize {w, (int)h};
    return QWidget::sizeHint();
}

void DataRenderWidget::paintEvent(QPaintEvent *e) {
    auto painter = QPainter {this};
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setFont(Settings::Instance()->get(Settings::SETTINGS_DYNAMIC::DATA_FONT).value<QFont>());

    m_data = m_reader->readBytes(m_begin, (m_maxRowCount + 1) * m_rowSize);

    auto rect = e->rect();
    //    qDebug() << __FUNCTION__ << rect;
    rect.setTopLeft({0, 0});
    rect.setWidth(this->width());
    rect.setHeight(this->height());
    drawColumnHeader(&painter, rect);
    rect.setTop(rect.top() + m_textSize.height() + m_headerMarginBottom);
    drawDataTable(0, rect, &painter);
}

void DataRenderWidget::showEvent(QShowEvent *) {
    updateData();
    m_overlay->setLeftTop({m_size8.width() + m_headerMarginRight, m_textSize.height() + m_headerMarginBottom});
}

void DataRenderWidget::resizeEvent(QResizeEvent *) {
    updateData();
}

void DataRenderWidget::initFakeData() {
    m_modified.insert(std::make_pair(0x10, 0x60));

    m_selectedArea.first = 0x05;
    m_selectedArea.second = 0x0C;
}

void DataRenderWidget::initSettings() {
    auto settings = Settings::Instance();
    connect(settings, &Settings::signal_settingsChanged, this, &DataRenderWidget::slot_reloadSettings);

    // static settings
    m_headerMarginBottom = settings->get<Settings::SETTINGS_STATIC::DATA_STYLE_HEADER_MARGIN_BOTTOM>().toInt();
    m_headerMarginRight = settings->get<Settings::SETTINGS_STATIC::DATA_STYLE_HEADER_MARGIN_RIGHT>().toInt();
    m_rowMargin = settings->get<Settings::SETTINGS_STATIC::DATA_STYLE_VERTICAL_MARGIN>().toInt();

    // dynamic settings
    m_groupSize = settings->get(Settings::SETTINGS_DYNAMIC::DATA_GROUP_SIZE).toInt();
    m_rowSize = settings->get(Settings::SETTINGS_DYNAMIC::DATA_ROW_BYTE_COUNT).toInt();

    // font settings
    m_font = settings->get(Settings::SETTINGS_DYNAMIC::DATA_FONT).value<QFont>();
    m_textSize = settings->get(Settings::SETTINGS_DYNAMIC::DATA_TEXT_SIZE).value<QSize>();

    m_size2 = m_textSize;
    m_size2.setWidth(m_size2.width() * 2);
    m_size4 = m_textSize;
    m_size4.setWidth(m_size4.width() * 4);
    m_size8 = m_textSize;
    m_size8.setWidth(m_size8.width() * 8);
    m_size16 = m_textSize;
    m_size16.setWidth(m_size16.width() * 16);
}

void DataRenderWidget::updateData() {
    auto height = static_cast<QWidget *>(this->parent())->height();
    height = this->height();
    m_maxRowCount = height / (m_textSize.height() + m_rowMargin);
    m_cursorController->setMaxRowCount(m_maxRowCount);
    size_t end = m_begin + m_maxRowCount * m_rowSize;
    m_data = m_reader->readBytes(m_begin, end);
}

void DataRenderWidget::drawColumnHeader(QPainter *painter, const QRect &rect) {
    painter->save();
    painter->setPen(m_colors.getColor<RenderColor::COLOR_GROUP::BYTE_ORDER, false>());
    painter->setBrush(m_colors.getColor<RenderColor::COLOR_GROUP::BYTE_ORDER, true>());

    auto horHeaderRect = rect;
    auto verHeaderRect = rect;
    horHeaderRect.setBottom(rect.top() + m_textSize.height());
    verHeaderRect.setRight(rect.left() + m_size8.width());

    horHeaderRect.setLeft(horHeaderRect.left() + verHeaderRect.width() + m_headerMarginRight);
    horHeaderRect.setRight(horHeaderRect.right() + verHeaderRect.width() + m_headerMarginRight);

    QString value;
#define SPACE_1 " "
#define SPACE_2 SPACE_1 SPACE_1
#define SPACE_4 SPACE_2 SPACE_2
#define SPACE_8 SPACE_4 SPACE_4
#define SPACE_16 SPACE_8 SPACE_8
#define SPACE_32 SPACE_16 SPACE_16

    for (int i = 0; i < m_rowSize; i += m_groupSize) {
        value.append(Utils::toHexBe(i, 1));
        switch (m_groupSize) {
        case 1: value.append(" "); break;
        case 2: value.append(SPACE_2 SPACE_1); break;
        case 4: value.append(SPACE_4 SPACE_1); break;
        case 8: value.append(SPACE_8 SPACE_1); break;
        case 16: value.append(SPACE_16 SPACE_1); break;
        case 32: value.append(SPACE_32 SPACE_1); break;
        }
    }
    painter->drawText(horHeaderRect, value);

    painter->restore();
}

void DataRenderWidget::drawDataTable(const size_t &begin, const QRect &dataArea, QPainter *painter) {
    auto const &rowHeight = m_textSize.height() + m_rowMargin * 2;
    auto area = dataArea;
    area.setBottom(area.top() + rowHeight);
    for (int i = 0; i < m_maxRowCount; ++i) {
        drawTableRow(begin + (size_t)i * m_rowSize, area, painter);
        area.setTop(area.bottom());
        area.setBottom(area.top() + rowHeight);
    }
}

void DataRenderWidget::drawTableRow(const size_t &begin, const QRect &rowRect, QPainter *painter) {
    auto rect = rowRect;

    // draw row index
    painter->save();
    painter->setPen(m_colors.getColor<RenderColor::COLOR_GROUP::BYTE_ORDER, false>());
    painter->setBrush(m_colors.getColor<RenderColor::COLOR_GROUP::BYTE_ORDER, true>());
    auto const &header = Utils::toHexLe(m_begin + begin, 4);
    rect.setRight(rect.left() + m_size8.width());
    painter->drawText(rect, Qt::AlignVCenter | Qt::AlignLeft, header);
    painter->restore();

    // draw row data
    rect.setLeft(rect.right() + m_headerMarginRight);
    rect.setRight(rect.left() + m_size2.width());
    for (int i = 0; i < m_rowSize; ++i) {
        if (i && (i % m_groupSize == 0)) {
            rect.setLeft(rect.left() + m_textSize.width());
            rect.setRight(rect.right() + m_textSize.width());
        }
        drawSingleByte(begin + (size_t)i, rect, painter);

        rect.setLeft(rect.right());
        rect.setRight(rect.left() + m_size2.width());
    }
}

void DataRenderWidget::drawSingleByte(const size_t &pos, const QRect &bodyArea, QPainter *painter) {
    painter->save();
    if (m_selectedArea.first <= pos && pos < m_selectedArea.second) {
        drawSingleSelected(bodyArea, pos, painter);
    } else if (m_modified.count(pos)) {
        drawSingleModified(bodyArea, pos, painter);
    } else if ((pos / m_groupSize) % 2) {
        drawSingleOddColumn(bodyArea, pos, painter);
    } else {
        drawSingleEvenColumn(bodyArea, pos, painter);
    }
    painter->restore();
}

void DataRenderWidget::drawSingleSelected(const QRect &rect, const size_t &pos, QPainter *painter) {
    auto const &data = m_data.at((int)pos);
    painter->setPen(m_colors.getColor<RenderColor::COLOR_GROUP::SELECTED, false>());
    painter->setBrush(m_colors.getColor<RenderColor::COLOR_GROUP::SELECTED, true>());

    if ((pos % m_rowSize) && (pos % m_groupSize == 0) && (pos != m_selectedArea.first)) {
        auto bRect = rect;
        bRect.setLeft(rect.left() - m_textSize.width());
        painter->fillRect(bRect, painter->brush());
    } else {
        painter->fillRect(rect, painter->brush());
    }
    if (auto mod = m_modified.find(pos); mod != m_modified.end()) {
        painter->drawText(rect, Qt::AlignVCenter | Qt::AlignLeft, Utils::toHexBe(mod->second, 1));
    } else {
        painter->drawText(rect, Qt::AlignVCenter | Qt::AlignLeft, Utils::toHexBe(data, 1));
    }
}

void DataRenderWidget::drawSingleModified(const QRect &rect, const size_t &pos, QPainter *painter) {
    auto const &data = m_modified.at(pos);
    painter->setPen(m_colors.getColor<RenderColor::COLOR_GROUP::MODIFIED, false>());
    painter->setBrush(m_colors.getColor<RenderColor::COLOR_GROUP::MODIFIED, true>());

    painter->drawText(rect, Qt::AlignVCenter | Qt::AlignLeft, Utils::toHexBe(data, 1));
}

void DataRenderWidget::drawSingleOddColumn(const QRect &rect, const size_t &pos, QPainter *painter) {
    painter->setPen(m_colors.getColor<RenderColor::COLOR_GROUP::ODD_DATA, false>());
    painter->setBrush(m_colors.getColor<RenderColor::COLOR_GROUP::ODD_DATA, true>());
    auto const &data = m_data.at((int)pos);

    painter->drawText(rect, Qt::AlignVCenter | Qt::AlignLeft, Utils::toHexBe(data, 1));
}

void DataRenderWidget::drawSingleEvenColumn(const QRect &rect, const size_t &pos, QPainter *painter) {
    painter->setPen(m_colors.getColor<RenderColor::COLOR_GROUP::EVEN_DATA, false>());
    painter->setBrush(m_colors.getColor<RenderColor::COLOR_GROUP::EVEN_DATA, true>());
    auto const &data = m_data.at((int)pos);

    painter->drawText(rect, Qt::AlignVCenter | Qt::AlignLeft, Utils::toHexBe(data, 1));
}

void DataRenderWidget::slot_onCursorMoved(const QPoint &leftTop) {
    m_overlay->setEditIndicator(leftTop, m_textSize.height() + 2 * m_rowMargin, 50);
}

void DataRenderWidget::slot_reloadSettings() {
    m_groupSize = Settings::Instance()->get(Settings::SETTINGS_DYNAMIC::DATA_GROUP_SIZE).toInt();
    m_rowSize = Settings::Instance()->get(Settings::SETTINGS_DYNAMIC::DATA_ROW_BYTE_COUNT).toInt();
}

void DataRenderWidget::slot_onSelectionChanged(const size_t &begin, const size_t &end) {
    m_selectedArea.first = m_begin + begin;
    m_selectedArea.second = m_begin + end;
    this->update();
}

void DataRenderWidget::slot_onDataModified(size_t pos, int n, bool isLeft) {
    if (auto iter = m_modified.find(m_begin + pos); iter != m_modified.end()) {
        auto &data = iter->second;
        if (isLeft) {
            data &= 0x0F;
            data |= (n << 4);
        } else {
            data &= 0xF0;
            data |= (n & 0x0F);
        }
    } else {
        m_modified.insert(std::make_pair(m_begin + pos, m_data.at(pos)));
        auto &data = m_modified.find(m_begin + pos)->second;
        if (isLeft) {
            data &= 0x0F;
            data |= (n << 4);
        } else {
            data &= 0xF0;
            data |= (n & 4);
        }
    }
    update();
}

void DataRenderWidget::slot_onShowUpRows(size_t nrow) {
    if (nrow < (m_begin / m_rowSize))
        m_begin -= nrow * m_rowSize;
    else
        m_begin = 0;

    m_cursorController->setTotalBegin(m_begin);
    this->update();
}

void DataRenderWidget::slot_onShowDownRows(size_t nrow) {
    m_begin += nrow * m_rowSize;
    m_cursorController->setTotalBegin(m_begin);
    this->update();
}

void DataRenderWidget::slot_onCopy() {
    auto const &[begin, end] = m_selectedArea;
    QString clipboard_text;
    for (size_t i = begin; i < end; i++) {
        static auto constexpr HEX_DIGITS = "0123456789ABCDEF";
        auto const byte = m_reader->readBytes(i, 1)[0];  // do not use ref
        auto const &left = byte >> 4 & 0x0F;
        auto const &right = byte & 0x0F;
        auto const &left_c = HEX_DIGITS[left];
        auto const &right_c = HEX_DIGITS[right];
        auto const temp = QString("%1%2").arg(left_c).arg(right_c);
        clipboard_text.append(temp);
        if (i % m_groupSize) continue;
        if (i == end - 1) continue;
        clipboard_text.append(' ');
    }

    auto clipboard = QGuiApplication::clipboard();
    clipboard->setText(clipboard_text);

    //    qDebug() << clipboard->text();
}

void DataRenderWidget::slot_onPaste(bool isCover) {
    if (isCover) {
        auto clipboard = QGuiApplication::clipboard();
        auto text = clipboard->text();

        std::list<char> charList;
        char temp = '\0';
        bool isFirst = true;
        for (auto const &ch : text.toStdString()) {
            if ('0' <= ch && ch <= '9') {
                // is num digit
                if (isFirst) {
                    temp = (ch - '0') << 4 & 0xF0;
                } else {
                    temp |= ((ch - '0') & 0x0F);
                }
                isFirst = !isFirst;
            } else if ('a' <= ch && ch <= 'f') {
                // is lower hex digit
                if (isFirst) {
                    temp = (ch - 'a' + 0x0A) << 4 & 0xF0;
                } else {
                    temp |= ((ch - 'a' + 0x0A) & 0x0F);
                }
                isFirst = !isFirst;
            } else if ('A' <= ch && ch <= 'F') {
                // is upper hex digit
                if (isFirst) {
                    temp = (ch - 'A' + 0x0A) << 4 & 0xF0;
                } else {
                    temp |= ((ch - 'A' + 0x0A) & 0x0F);
                }
                isFirst = !isFirst;
            } else if (' ' == ch) {
                // space, ignore
                if (!isFirst) {  // spaces is between 2 hexdigits in one char, its a fault
                    qWarning() << "unexpected space position.";
                    return;
                }
                continue;  // avoid enter the if statement below
            } else {
                qWarning() << "invalid clipboard text.";
                return;
            }

            if (isFirst) {  // checked chars, performance enhancement
                charList.push_back(temp);
                temp = '\0';
            }
        }

        // check finished and successed, now do the cover work
        auto pos_begin = m_cursorController->currentPositionRelative() + m_begin;
        for (auto const &ch : charList) {
            if (auto iter = m_modified.find(pos_begin); iter != m_modified.end()) {  // data has been modified before
                iter->second = ch;
            } else {
                m_modified.insert(std::make_pair(pos_begin, ch));
            }
            ++pos_begin;
        }
        this->update();
    } else {
        qWarning() << "Not supported yet.";
    }
}
