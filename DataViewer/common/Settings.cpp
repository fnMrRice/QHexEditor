#include "Settings.h"

#include <QFontDatabase>
#include <QFontMetrics>

Settings::Settings() : QObject(nullptr) {
    m_staticSettings = {
        {SETTINGS_STATIC::DATA_STYLE_VERTICAL_MARGIN, 2},
        {SETTINGS_STATIC::DATA_STYLE_HEADER_MARGIN_BOTTOM, 8},
        {SETTINGS_STATIC::DATA_STYLE_HEADER_MARGIN_RIGHT, 12},
    };

    auto font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    auto fm = QFontMetrics {font};
    QSize textSize = {fm.horizontalAdvance('_'), fm.height()};
    m_dynamicSettings = {
        {SETTINGS_DYNAMIC::DATA_GROUP_SIZE, 1},
        {SETTINGS_DYNAMIC::DATA_ROW_BYTE_COUNT, 0x10},
        {SETTINGS_DYNAMIC::DATA_FONT, std::move(font)},
        {SETTINGS_DYNAMIC::DATA_TEXT_SIZE, std::move(textSize)},
    };
}
void Settings::set(const SETTINGS_DYNAMIC& fields, const QVariant& data) {
    m_dynamicSettings[fields] = data;
    emit signal_settingsChanged();
}

QVariant Settings::get(const SETTINGS_DYNAMIC& fields) const {
    return m_dynamicSettings.at(fields);
}

Settings* Settings::s_ptr = nullptr;
std::mutex Settings::s_mutex;
Settings* Settings::Instance() {
    if (s_ptr) return s_ptr;
    s_mutex.lock();
    if (s_ptr) return s_ptr;
    s_ptr = new Settings();
    s_mutex.unlock();
    return s_ptr;
}
