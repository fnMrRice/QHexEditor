#include "BackupInfo.h"

#include "Utils/ByteArrayReader.h"
#include "Utils/Utils.h"

BackupInfo::BackupInfo() {}

QByteArray BackupInfo::serialize() const {
    QByteArray retval;

    retval.push_back(Utils::toByteArray(m_md5_before.length()));
    retval.push_back(m_md5_before);

    retval.push_back(Utils::toByteArray(m_modifications.size()));

    for (auto [pos, value] : m_modifications) {
        auto [before, after] = value;
        retval.push_back(Utils::toByteArray(pos));
        retval.push_back(Utils::toByteArray(before));
        retval.push_back(Utils::toByteArray(after));
    }

    retval.push_back(Utils::toByteArray(m_md5_after.length()));
    retval.push_back(m_md5_after);

    return retval;
}

bool BackupInfo::deserialize(const QByteArray& data) {
    try {
        ByteArrayReader reader {data};

        auto before_len = reader.read<decltype(data.length())>();
        m_md5_before = reader.read(before_len);

        auto mod_size = reader.read<decltype(m_modifications.size())>();

        for (decltype(mod_size) i = 0; i < mod_size; i++) {
            auto pos = reader.read<size_t>();
            auto before = reader.read<uint8_t>();
            auto after = reader.read<uint8_t>();

            if (auto iter = m_modifications.find(pos); iter != m_modifications.end()) {
                iter->second = std::make_pair(before, after);
            } else {
                m_modifications.insert(std::make_pair(pos, std::make_pair(before, after)));
            }
        }

        auto after_len = reader.read<decltype(data.length())>();
        m_md5_after = reader.read(after_len);

        return true;
    } catch (const std::out_of_range& e) {
        qWarning() << e.what();
        return false;
    }
}

void BackupInfo::add_modification(const size_t& pos, const uint8_t& before, const uint8_t& after) {
    if (auto iter = m_modifications.find(pos); iter != m_modifications.end()) {
        iter->second = std::make_pair(before, after);
    } else {
        m_modifications.insert(std::make_pair(pos, std::make_pair(before, after)));
    }
}

void BackupInfo::add_modification(const size_t& pos, const std::pair<uint8_t, uint8_t>& value) {
    if (auto iter = m_modifications.find(pos); iter != m_modifications.end()) {
        iter->second = value;
    } else {
        m_modifications.insert(std::make_pair(pos, value));
    }
}

void BackupInfo::add_modification(const std::pair<size_t, std::pair<uint8_t, uint8_t>>& data) {
    auto [pos, value] = data;
    if (auto iter = m_modifications.find(pos); iter != m_modifications.end()) {
        iter->second = value;
    } else {
        m_modifications.insert(std::make_pair(pos, value));
    }
}
