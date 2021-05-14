#ifndef CONTEXTMENUCONTROLLER_H
#define CONTEXTMENUCONTROLLER_H

#include <QAction>
#include <QMenu>
#include <QObject>

#include "common/pch.h"
class IBeamCursorOverlay;

class ContextMenuController : public QObject {
    Q_OBJECT
   public:
    explicit ContextMenuController(IBeamCursorOverlay* overlay, QObject* parent = nullptr);

   private:
    void initMenus(QWidget* parent);
    void initSignals(IBeamCursorOverlay* widget);

   private slots:
    void slot_onMenuRequested(const QPoint& pos);
    void slot_onPaste();

   signals:
    void signal_copy();
    void signal_paste(bool isCover);
    void signal_cut();
    void signal_delete();
    void signal_revert();
    void signal_fill();

   private:
    uptr<QMenu> m_menu;
    uptr<QAction> m_action_copy, m_action_cut, m_action_paste_cover, m_action_paste_insert, m_action_delete;
    uptr<QAction> m_action_revert;
    uptr<QAction> m_action_fill;
    uptr<QAction> m_action_selectRange;
};

#endif  // CONTEXTMENUCONTROLLER_H
