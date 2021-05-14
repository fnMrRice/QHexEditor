#include "RenderColor.h"

RenderColor::RenderColor() {
    m_byte_order_color = {QColor {0, 0, 191}, QColor {0, 0, 0, 0}};
    m_odd_data_color = {QColor {128, 96, 96}, QColor {0, 0, 0, 0}};
    m_even_data_color = {QColor {0, 0, 0}, QColor {0, 0, 0, 0}};
    m_modified_data_color = {QColor {255, 0, 0}, QColor {0, 0, 0, 0}};
    m_seleceted_data_color = {QColor {255, 255, 255}, QColor {0, 120, 215}};
}
