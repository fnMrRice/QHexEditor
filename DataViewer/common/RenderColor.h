#ifndef RENDERCOLOR_H
#define RENDERCOLOR_H
#include <QColor>
#include <tuple>

class RenderColor {
   public:
    enum class COLOR_GROUP {
        BYTE_ORDER,
        ODD_DATA,
        EVEN_DATA,
        MODIFIED,
        SELECTED,
    };

   public:
    RenderColor();

   public:
    template <COLOR_GROUP _Group, bool _IsBack>
    QColor getColor() {
        int constexpr pos = _IsBack;
        switch (_Group) {
        case COLOR_GROUP::BYTE_ORDER: return std::get<pos>(m_byte_order_color);
        case COLOR_GROUP::ODD_DATA: return std::get<pos>(m_odd_data_color);
        case COLOR_GROUP::EVEN_DATA: return std::get<pos>(m_even_data_color);
        case COLOR_GROUP::MODIFIED: return std::get<pos>(m_modified_data_color);
        case COLOR_GROUP::SELECTED: return std::get<pos>(m_seleceted_data_color);
        default: return QColor();
        }
    }

   private:
    using bg_color = std::tuple<QColor, QColor>;  // front back
    bg_color m_byte_order_color;
    bg_color m_odd_data_color;
    bg_color m_even_data_color;
    bg_color m_modified_data_color;
    bg_color m_seleceted_data_color;
};

#endif  // RENDERCOLOR_H
