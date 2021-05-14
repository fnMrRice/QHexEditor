#include "ContextMenuController.h"

#include "Cursor/IBeamCursorOverlay.h"

ContextMenuController::ContextMenuController(IBeamCursorOverlay* overlay, QObject* parent) : QObject(parent) {
    initMenus(overlay);
    initSignals(overlay);
}

void ContextMenuController::initMenus(QWidget* parent) {
    m_menu = mk_u<QMenu>(tr("Menu"), parent);
    m_action_copy = mk_u<QAction>(tr("Copy"));
    m_action_cut = mk_u<QAction>(tr("Cut"));
    m_action_paste_cover = mk_u<QAction>(tr("Paste(Cover)"));
    m_action_paste_insert = mk_u<QAction>(tr("Paste(Insert)"));
    m_action_delete = mk_u<QAction>(tr("Delete"));

    m_action_revert = mk_u<QAction>(tr("Withdraw"));
    m_action_fill = mk_u<QAction>(tr("Fill"));
    m_action_selectRange = mk_u<QAction>(tr("Select Range"));

    m_menu->addActions({
        m_action_copy.get(),
        //        m_action_cut.get(),
        m_action_paste_cover.get(),
        //        m_action_paste_insert.get(),
        //        m_action_delete.get(),
        //        m_action_revert.get(),
        //        m_action_fill.get(),
        //        m_action_selectRange.get(),
    });

    m_menu->insertSeparator(m_action_revert.get());

    connect(m_action_copy.get(), &QAction::triggered,
            this, &ContextMenuController::signal_copy);
    connect(m_action_cut.get(), &QAction::triggered,
            this, &ContextMenuController::signal_cut);
    connect(m_action_paste_cover.get(), &QAction::triggered,
            this, &ContextMenuController::slot_onPaste);
    connect(m_action_paste_insert.get(), &QAction::triggered,
            this, &ContextMenuController::slot_onPaste);
    connect(m_action_delete.get(), &QAction::triggered,
            this, &ContextMenuController::signal_delete);
    connect(m_action_revert.get(), &QAction::triggered,
            this, &ContextMenuController::signal_revert);
    connect(m_action_fill.get(), &QAction::triggered,
            this, &ContextMenuController::signal_fill);

    // these are hard
    m_action_paste_insert->setVisible(false);
    m_action_selectRange->setVisible(false);
}

void ContextMenuController::initSignals(IBeamCursorOverlay* widget) {
    connect(widget, &IBeamCursorOverlay::customContextMenuRequested,
            this, &ContextMenuController::slot_onMenuRequested);
}

void ContextMenuController::slot_onMenuRequested(const QPoint& pos) {
    m_menu->exec(QCursor::pos());
}

void ContextMenuController::slot_onPaste() {
    if (sender() == m_action_paste_cover.get()) {
        emit signal_paste(true);
    } else if (sender() == m_action_paste_insert.get()) {
        emit signal_paste(false);
    }
}
