#ifndef IBEAMCURSOROVERLAY_H
#define IBEAMCURSOROVERLAY_H

#include <QLabel>
#include <QTimer>

#include "IOverlayWidget.h"
#include "common/pch.h"

class IBeamCursorOverlay : public IOverlayWidget {
    Q_OBJECT

   public:
    explicit IBeamCursorOverlay(QWidget *parent = nullptr);

   public:
    void setLeftTop(const QPoint &pos);
    QPoint offsetLeftTop() const override final;

    void setEditIndicator(const QPoint &topLeft, const int &height, const int &flashInterval);
    void unsetEditIndicator();

   protected:
    bool event(QEvent *event) override final;
    void showEvent(QShowEvent *) override final;

   signals:
    void signal_mouseLeftPressed(const QPoint &pos);
    void signal_mouseLeftMoved(const QPoint &pos);
    void signal_mouseLeftReleased(const QPoint &pos);
    void signal_mouseDblClicked(const QPoint &pos);

    void signal_numInput(int n);
    void signal_move(MOVE_DIRECTION dir);
    void signal_backSpace();
    void signal_delete();
    void signal_insert();

   private:
    static auto constexpr CURSOR_BLOCK_WIDTH = 2;
    QPoint m_leftTop {0, 0};
    std::unique_ptr<QLabel> m_cursor;

    QTimer m_flashTimer;
    int m_flashIntervalCounter = 0;
    int m_flashInterval = -1;  // -1 means close, 0 means always on, 1 means 10ms
};

#endif  // IBEAMCURSOROVERLAY_H
