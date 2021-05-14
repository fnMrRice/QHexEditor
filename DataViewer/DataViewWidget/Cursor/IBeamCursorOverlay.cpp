#include "IBeamCursorOverlay.h"

#include <QDebug>
#include <QKeyEvent>
#include <QMouseEvent>

IBeamCursorOverlay::IBeamCursorOverlay(QWidget *parent) : IOverlayWidget(parent) {
    this->setMouseTracking(true);
    this->setCursor(Qt::CursorShape::IBeamCursor);
    this->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
    this->setContextMenuPolicy(Qt::CustomContextMenu);

    m_cursor = std::make_unique<QLabel>(this);
    m_cursor->show();
    m_cursor->setStyleSheet("background-color: black;border: hidden;");
    m_flashTimer.callOnTimeout([this] {
        if (m_flashInterval < 0) {
            m_cursor->setVisible(false);
        } else if (m_flashInterval == 0) {
            m_cursor->setVisible(true);
        } else {
            if (!m_flashIntervalCounter) {
                m_cursor->setVisible(!m_cursor->isVisible());
                m_flashIntervalCounter = m_flashInterval;
            }
            --m_flashIntervalCounter;
        }
    });
}

void IBeamCursorOverlay::setLeftTop(const QPoint &pos) {
    m_leftTop = pos;
    updatePosition();
}

QPoint IBeamCursorOverlay::offsetLeftTop() const {
    return m_leftTop;
}

void IBeamCursorOverlay::setEditIndicator(const QPoint &topLeft, const int &height, const int &flashInterval) {
    m_flashInterval = flashInterval;

    m_cursor->setFixedSize(CURSOR_BLOCK_WIDTH, height);
    m_cursor->move(topLeft);
}

void IBeamCursorOverlay::unsetEditIndicator() {
    m_flashInterval = -1;
}

bool IBeamCursorOverlay::event(QEvent *e) {
    if (e->type() == QEvent::MouseButtonPress) {
        auto me = static_cast<QMouseEvent *>(e);
        if (me->buttons() & Qt::LeftButton) {
            emit signal_mouseLeftPressed(me->pos());
            return true;
        }
    } else if (e->type() == QEvent::MouseMove) {
        auto me = static_cast<QMouseEvent *>(e);
        if (me->buttons() & Qt::LeftButton) {
            emit signal_mouseLeftMoved(me->pos());
            return true;
        }
    } else if (e->type() == QEvent::MouseButtonRelease) {
        auto me = static_cast<QMouseEvent *>(e);
        if (me->button() == Qt::LeftButton) {
            emit signal_mouseLeftReleased(me->pos());
            return true;
        }
    } else if (e->type() == QEvent::MouseButtonDblClick) {
        auto me = static_cast<QMouseEvent *>(e);
        emit signal_mouseDblClicked(me->pos());
        return true;
    } else if (e->type() == QEvent::KeyPress) {
        auto ke = static_cast<QKeyEvent *>(e);
        //        qDebug() << __FUNCTION__ << "key down";
        if (Qt::Key_0 <= ke->key() && ke->key() <= Qt::Key_9) {  // num input
            emit signal_numInput(ke->key() - Qt::Key_0);
        } else if (Qt::Key_A <= ke->key() && ke->key() <= Qt::Key_F) {  // a-f input
            emit signal_numInput(ke->key() - Qt::Key_A + 10);
        } else {
            if (ke->matches(QKeySequence::StandardKey::MoveToEndOfDocument)) {
                emit signal_move(MOVE_DIRECTION::DOC_END);
            } else if (ke->matches(QKeySequence::StandardKey::MoveToEndOfLine)) {
                emit signal_move(MOVE_DIRECTION::RIGHT_LINE);
            } else if (ke->matches(QKeySequence::StandardKey::MoveToNextChar)) {
                emit signal_move(MOVE_DIRECTION::RIGHT);
            } else if (ke->matches(QKeySequence::StandardKey::MoveToNextLine)) {
                emit signal_move(MOVE_DIRECTION::DOWN);
            } else if (ke->matches(QKeySequence::StandardKey::MoveToNextPage)) {
                emit signal_move(MOVE_DIRECTION::DOWN_PAGE);
            } else if (ke->matches(QKeySequence::StandardKey::MoveToNextWord)) {
                emit signal_move(MOVE_DIRECTION::RIGHT_WORD);
            } else if (ke->matches(QKeySequence::StandardKey::MoveToPreviousChar)) {
                emit signal_move(MOVE_DIRECTION::LEFT);
            } else if (ke->matches(QKeySequence::StandardKey::MoveToPreviousLine)) {
                emit signal_move(MOVE_DIRECTION::UP);
            } else if (ke->matches(QKeySequence::StandardKey::MoveToPreviousPage)) {
                emit signal_move(MOVE_DIRECTION::UP_PAGE);
            } else if (ke->matches(QKeySequence::StandardKey::MoveToPreviousWord)) {
                emit signal_move(MOVE_DIRECTION::LEFT_WORD);
            } else if (ke->matches(QKeySequence::StandardKey::MoveToStartOfDocument)) {
                emit signal_move(MOVE_DIRECTION::DOC_BEGIN);
            } else if (ke->matches(QKeySequence::StandardKey::MoveToStartOfLine)) {
                emit signal_move(MOVE_DIRECTION::LEFT_LINE);
            }
        }
    } else if (e->type() == QEvent::KeyRelease) {
        //        auto ke = static_cast<QKeyEvent *>(e);
        //        qDebug() << __FUNCTION__ << "key up";
    }
    return IOverlayWidget::event(e);
}

void IBeamCursorOverlay::showEvent(QShowEvent *) {
    m_flashTimer.start(10);
}
