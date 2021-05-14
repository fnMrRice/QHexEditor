#include "IOverlayWidget.h"

#include <QEvent>

IOverlayWidget::IOverlayWidget(QWidget *parent) : QWidget(parent) {
    if (parent) {
        parent->installEventFilter(this);
        this->show();
    }
}

void IOverlayWidget::updatePosition() {
    if (this->parent() && this->parent()->isWidgetType()) {
        auto w = static_cast<QWidget *>(this->parent());
        this->setFixedSize(w->width() - offsetLeftTop().x() + offsetRightBottom().x(), w->height() - offsetLeftTop().y() + offsetRightBottom().y());
        this->move(offsetLeftTop());
    }
}

bool IOverlayWidget::event(QEvent *e) {
    if (auto const &p = this->parent()) {
        if (p->isWidgetType()) {
            if (e->type() == QEvent::ParentChange) {
                updatePosition();
                p->installEventFilter(this);
            }
        }
    }
    return QWidget::event(e);
}

bool IOverlayWidget::eventFilter(QObject *object, QEvent *e) {
    if (this->parent() && object == this->parent()) {
        if (this->parent()->isWidgetType()) {
            if (e->type() == QEvent::Resize || e->type() == QEvent::Show) {
                updatePosition();
            }
        }
    }
    return QWidget::eventFilter(object, e);
}
