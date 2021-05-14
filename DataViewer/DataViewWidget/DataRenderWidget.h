#ifndef DATARENDERWIDGET_H
#define DATARENDERWIDGET_H

#include <QFileDevice>
#include <QWidget>
#include <array>
#include <memory>
#include <mutex>

#include "common/FileReader.h"
#include "common/RenderColor.h"
#include "common/pch.h"

class IBeamCursorOverlay;
class CursorController;
class ContextMenuController;

class DataRenderWidget : public QWidget {
    Q_OBJECT
   public:
    explicit DataRenderWidget(const std::shared_ptr<QFileDevice> &file, QWidget *parent = nullptr);

   public:
    QSize sizeHint() const override final;
    std::shared_ptr<FileReader> reader() const { return m_reader; }
    size_t begin() const { return m_begin; }

   protected:
    void paintEvent(QPaintEvent *) override final;
    void showEvent(QShowEvent *) override final;
    void resizeEvent(QResizeEvent *) override final;

   private:
    void initFakeData();

    void initSettings();
    void updateData();

    void drawColumnHeader(QPainter *painter, const QRect &rect);
    void drawDataTable(const size_t &begin, const QRect &dataArea, QPainter *painter);
    void drawTableRow(const size_t &begin, const QRect &rowRect, QPainter *painter);
    void drawSingleByte(const size_t &bytePos, const QRect &bodyArea, QPainter *painter);
    void drawSingleSelected(const QRect &rect, const size_t &pos, QPainter *painter);
    void drawSingleModified(const QRect &rect, const size_t &pos, QPainter *painter);
    void drawSingleOddColumn(const QRect &rect, const size_t &pos, QPainter *painter);
    void drawSingleEvenColumn(const QRect &rect, const size_t &pos, QPainter *painter);

   private slots:
    void slot_reloadSettings();
    void slot_onCursorMoved(const QPoint &);
    void slot_onSelectionChanged(const size_t &, const size_t &);
    void slot_onDataModified(size_t pos, int n, bool isLeft);
    void slot_onShowUpRows(size_t nrow);
    void slot_onShowDownRows(size_t nrow);
    void slot_onCopy();
    void slot_onPaste(bool isCover);

   signals:
    void signal_cursorPosChanged(size_t);

   private:
    // do not modify after init
    QFont m_font;
    QSize m_textSize, m_size2, m_size4, m_size8, m_size16, m_size32;
    RenderColor m_colors;
    std::shared_ptr<FileReader> m_reader;
    // modified by any action
    QByteArray m_data;
    // modified by change settings
    int m_headerMarginBottom = 8;
    int m_headerMarginRight = 16;
    int m_rowMargin = 2;
    int m_groupSize = 1;  // 1,2,4,8,16,32
    int m_rowSize = 16;   // 16*(1-16)
    // modified by user window action
    int m_maxRowCount = 0;
    // modified by user normal action
    size_t m_begin = 0x00;
    std::pair<size_t, size_t> m_selectedArea;
    std::unordered_map<size_t, uint8_t> m_modified;
    // others
    std::unique_ptr<IBeamCursorOverlay> m_overlay;
    std::unique_ptr<CursorController> m_cursorController;
    uptr<ContextMenuController> m_menu;
};

#endif  // DATARENDERWIDGET_H
