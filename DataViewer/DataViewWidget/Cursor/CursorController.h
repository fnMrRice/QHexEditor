#ifndef CURSORCONTROLLER_H
#define CURSORCONTROLLER_H

#include <QDateTime>
#include <QObject>
#include <QSize>
#include <queue>

#include "common/pch.h"

class IBeamCursorOverlay;

class CursorController : public QObject {
    Q_OBJECT
   public:
    explicit CursorController(IBeamCursorOverlay *overlay, QObject *parent = nullptr);

   public:
    void setTotalBegin(const size_t &v) { m_totalBegin = v; }
    void setMaxRowCount(const size_t &v) { m_maxRowCount = v; }
    size_t currentPositionRelative() const { return m_currentMousePos; }

   private:
    void initSettings();
    void initSignals(IBeamCursorOverlay *overlay);
    std::tuple<size_t, size_t> PointToPositionRelative(const QPoint &pos);
    void cursorMove(int len);

   public slots:
    void slot_onMouseLeftDown(const QPoint &pos);
    void slot_onMouseLeftMove(const QPoint &pos);
    void slot_onMouseLeftUp(const QPoint &pos);

   private slots:
    void slot_onUserInput(int n);
    void slot_onMoveCursorByKeyboard(MOVE_DIRECTION dir);
    void slot_onInsertPressed();
    void slot_onDeletePressed();
    void slot_onBackspacePressed();

   private slots:
    void slot_onSettingsChagned();

   signals:
    // positions here are relative to the first byte's position
    void signal_textSelected(const size_t &begin, const size_t &end);
    void signal_cursorMoved(const QPoint &leftTop);
    // relative to the first
    void signal_cursorIndexChanged(const size_t &index);

    void signal_cursorOutOfRange(bool isTop);
    void signal_showUpRows(size_t n);
    void signal_showDownRows(size_t n);

    void signal_dataModified(size_t pos, int n, bool isLeft);

   private:
    // check double-click or triple-click
    std::queue<QDateTime> m_mouseDownTimes;
    // settings
    QSize m_textSize;
    // modified by change settings
    int m_headerMarginBottom = 8;
    int m_headerMarginRight = 16;
    int m_rowMargin = 2;
    int m_groupSize = 2;  // 1,2,4,8,16,32
    int m_rowSize = 16;   // 16*(1-16)
    // calculated when change settings
    size_t m_rowHeight = 0;
    size_t m_byteSize = 0;
    size_t m_groupWidth = 0;
    size_t m_rowWidth = 0;
    // members
    bool m_mouseDown = false;
    size_t m_lastPressedPos = 0;
    size_t m_lastMovedPos = 0;
    size_t m_currentMousePos = 0;
    size_t m_totalBegin = 0;
    size_t m_maxRowCount = 0;

    bool m_isModifying = false;
};

#endif  // CURSORCONTROLLER_H
