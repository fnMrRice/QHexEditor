#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QVariant>
#include <mutex>

class Settings : public QObject {
    Q_OBJECT
   public:
    enum class SETTINGS_STATIC {
        DATA_STYLE_VERTICAL_MARGIN,
        DATA_STYLE_HEADER_MARGIN_BOTTOM,
        DATA_STYLE_HEADER_MARGIN_RIGHT,
    };

    enum class SETTINGS_DYNAMIC {
        DATA_GROUP_SIZE,
        DATA_ROW_BYTE_COUNT,
        DATA_FONT,
        DATA_TEXT_SIZE,
    };

   private:
    explicit Settings();

   public:
    void set(const SETTINGS_DYNAMIC& fields, const QVariant& data);
    QVariant get(const SETTINGS_DYNAMIC& fields) const;
    template <SETTINGS_STATIC fields>
    QVariant get() const {
        return m_staticSettings.at(fields);
    }

   signals:
    void signal_settingsChanged();

   private:
    std::unordered_map<SETTINGS_STATIC, QVariant> m_staticSettings;
    std::unordered_map<SETTINGS_DYNAMIC, QVariant> m_dynamicSettings;

   public:
    static Settings* Instance();

   private:
    static Settings* s_ptr;
    static std::mutex s_mutex;
};

#endif  // SETTINGS_H
