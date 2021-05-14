#ifndef IOVERLAYWIDGET_H
#define IOVERLAYWIDGET_H

#include <QWidget>

class IOverlayWidget : public QWidget {
    Q_OBJECT
   public:
    explicit IOverlayWidget(QWidget *parent = nullptr);

   protected:
    void updatePosition();

    virtual QPoint offsetLeftTop() const { return QPoint {0, 0}; }
    virtual QPoint offsetRightBottom() const { return QPoint {0, 0}; }

   protected:
    virtual bool event(QEvent *) override;
    bool eventFilter(QObject *object, QEvent *e) override;
};

#endif  // IOVERLAYWIDGET_H
