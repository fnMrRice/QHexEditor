#include "CursorController.h"

#include <QDebug>

#include "IBeamCursorOverlay.h"
#include "common/Settings.h"

CursorController::CursorController(IBeamCursorOverlay *overlay, QObject *parent) : QObject(parent) {
    initSettings();
    initSignals(overlay);
}

void CursorController::initSettings() {
    auto settings = Settings::Instance();
    connect(settings, &Settings::signal_settingsChanged, this, &CursorController::slot_onSettingsChagned);

    // static settings
    m_headerMarginBottom = settings->get<Settings::SETTINGS_STATIC::DATA_STYLE_HEADER_MARGIN_BOTTOM>().toInt();
    m_headerMarginRight = settings->get<Settings::SETTINGS_STATIC::DATA_STYLE_HEADER_MARGIN_RIGHT>().toInt();
    m_rowMargin = settings->get<Settings::SETTINGS_STATIC::DATA_STYLE_VERTICAL_MARGIN>().toInt();

    // dynamic settings
    m_groupSize = settings->get(Settings::SETTINGS_DYNAMIC::DATA_GROUP_SIZE).toInt();
    m_rowSize = settings->get(Settings::SETTINGS_DYNAMIC::DATA_ROW_BYTE_COUNT).toInt();

    // font settings
    m_textSize = settings->get(Settings::SETTINGS_DYNAMIC::DATA_TEXT_SIZE).value<QSize>();

    // calculated
    m_rowHeight = m_rowMargin * 2 + m_textSize.height();
    m_byteSize = m_textSize.width() * 2;
    m_groupWidth = m_groupSize * m_byteSize + m_textSize.width();
    m_rowWidth = ((m_rowSize / m_groupSize) - 1) * m_textSize.width() + m_rowSize * m_byteSize;
}

void CursorController::initSignals(IBeamCursorOverlay *overlay) {
    connect(overlay, &IBeamCursorOverlay::signal_mouseLeftPressed, this, &CursorController::slot_onMouseLeftDown);
    connect(overlay, &IBeamCursorOverlay::signal_mouseLeftMoved, this, &CursorController::slot_onMouseLeftMove);
    connect(overlay, &IBeamCursorOverlay::signal_mouseLeftReleased, this, &CursorController::slot_onMouseLeftUp);

    connect(overlay, &IBeamCursorOverlay::signal_numInput, this, &CursorController::slot_onUserInput);
    connect(overlay, &IBeamCursorOverlay::signal_move, this, &CursorController::slot_onMoveCursorByKeyboard);
    connect(overlay, &IBeamCursorOverlay::signal_insert, this, &CursorController::slot_onInsertPressed);
    connect(overlay, &IBeamCursorOverlay::signal_delete, this, &CursorController::slot_onDeletePressed);
    connect(overlay, &IBeamCursorOverlay::signal_backSpace, this, &CursorController::slot_onBackspacePressed);
}

std::tuple<size_t, size_t> CursorController::PointToPositionRelative(const QPoint &pos) {
    const size_t row = pos.y() / m_rowHeight;
    if ((size_t)pos.x() < m_rowWidth) {
        size_t column = pos.x() / m_groupWidth * m_groupSize + pos.x() % m_groupWidth / m_byteSize;
        return std::make_tuple(row, column);
    } else {
        return std::make_tuple(row, m_rowSize);
    }
}

void CursorController::cursorMove(int len) {
    m_isModifying = false;
    if (len < 0) {  // move left or up
        size_t tlen = -len;
        if (tlen <= m_currentMousePos) {  // cursor is not moving to hidden area
            m_currentMousePos -= tlen;
        } else {
            size_t overflow = tlen - m_currentMousePos;
            size_t overflow_r = 1 + overflow / m_rowSize;
            if (m_totalBegin < overflow) {  // move to the first byte of the file
                m_currentMousePos = 0;
                emit signal_showUpRows(m_totalBegin / m_rowSize);
            } else {
                m_currentMousePos = m_rowSize - overflow % m_rowSize;
                emit signal_showUpRows(overflow_r);
            }
        }
    } else {
        auto const &rowCount = len / m_rowSize;
        auto const &totalRow = m_currentMousePos / m_rowSize + rowCount;
        if (totalRow > m_maxRowCount) {  // overflow
            emit signal_showDownRows(totalRow - m_maxRowCount);
        }
        m_currentMousePos += len;
    }
    size_t col = m_currentMousePos % m_rowSize;
    size_t row = m_currentMousePos / m_rowSize;
    emit signal_cursorMoved(QPoint {(int)(col / m_groupSize * m_groupWidth + col % m_groupSize * m_byteSize),
                                    (int)(row * m_rowHeight)});
    emit signal_textSelected(m_currentMousePos, m_currentMousePos);
    emit signal_cursorIndexChanged(m_currentMousePos);
}

void CursorController::slot_onMouseLeftDown(const QPoint &pos) {
    if (pos.x() < 0 || pos.y() < 0) return;
    m_mouseDown = true;
    m_isModifying = false;
    auto const &[row, col] = PointToPositionRelative(pos);
    m_lastPressedPos = row * m_rowSize + col;
    m_lastMovedPos = m_lastPressedPos;
    emit signal_cursorMoved(QPoint {(int)(col / m_groupSize * m_groupWidth + col % m_groupSize * m_byteSize),
                                    (int)(row * m_rowHeight)});
}

void CursorController::slot_onMouseLeftMove(const QPoint &pos) {
    if (!m_mouseDown) return;
    if (pos.x() < 0 || pos.y() < 0) return;
    auto const &[row, col] = PointToPositionRelative(pos);
    auto const &curMouse = row * m_rowSize + col;
    if (curMouse != m_lastMovedPos) {
        m_lastMovedPos = curMouse;
        emit signal_textSelected(std::min(m_lastPressedPos, curMouse), std::max(m_lastPressedPos, curMouse));
        emit signal_cursorMoved(QPoint {(int)(col / m_groupSize * m_groupWidth + col % m_groupSize * m_byteSize),
                                        (int)(row * m_rowHeight)});
    }
}

void CursorController::slot_onMouseLeftUp(const QPoint &pos) {
    if (pos.x() < 0 || pos.y() < 0) return;
    if (!m_mouseDown) { return; }
    m_mouseDown = false;
    auto const &[row, col] = PointToPositionRelative(pos);
    m_currentMousePos = row * m_rowSize + col;
    emit signal_textSelected(std::min(m_lastPressedPos, m_currentMousePos), std::max(m_lastPressedPos, m_currentMousePos));
}

void CursorController::slot_onUserInput(int n) {
    if (m_isModifying) {
        emit signal_dataModified(m_currentMousePos, n, false);
        cursorMove(1);
    } else {
        emit signal_dataModified(m_currentMousePos, n, true);
        m_isModifying = true;
    }
}

void CursorController::slot_onMoveCursorByKeyboard(MOVE_DIRECTION dir) {
    switch (dir) {
    case MOVE_DIRECTION::LEFT: cursorMove(-1); break;
    case MOVE_DIRECTION::RIGHT: cursorMove(1); break;
    case MOVE_DIRECTION::UP: cursorMove(-m_rowSize); break;
    case MOVE_DIRECTION::DOWN: cursorMove(m_rowSize); break;
    case MOVE_DIRECTION::LEFT_WORD: cursorMove(-1); break;
    case MOVE_DIRECTION::RIGHT_WORD: cursorMove(1); break;
    case MOVE_DIRECTION::LEFT_LINE: break;
    case MOVE_DIRECTION::RIGHT_LINE: break;
    case MOVE_DIRECTION::UP_PAGE: break;
    case MOVE_DIRECTION::DOWN_PAGE: break;
    case MOVE_DIRECTION::DOC_BEGIN: break;
    case MOVE_DIRECTION::DOC_END: break;
    default: break;
    }
}

void CursorController::slot_onInsertPressed() {}
void CursorController::slot_onDeletePressed() {}
void CursorController::slot_onBackspacePressed() {}

void CursorController::slot_onSettingsChagned() {
    m_groupSize = Settings::Instance()->get(Settings::SETTINGS_DYNAMIC::DATA_GROUP_SIZE).toInt();
    m_rowSize = Settings::Instance()->get(Settings::SETTINGS_DYNAMIC::DATA_ROW_BYTE_COUNT).toInt();

    m_rowHeight = m_rowMargin * 2 + m_textSize.height();
    m_byteSize = m_textSize.width() * 2;
    m_groupWidth = m_groupSize * m_byteSize + m_textSize.width();
    m_rowWidth = ((m_rowSize / m_groupSize) - 1) * m_textSize.width() + m_rowSize * m_byteSize;
}
