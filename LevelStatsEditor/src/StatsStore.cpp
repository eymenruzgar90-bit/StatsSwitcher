#include "StatsStore.hpp"

#include <cctype>
#include <charconv>
#include <string_view>

using namespace geode::prelude;

namespace lse {

    static std::string storageKey(std::string const& key) {
        return "edit:" + key;
    }

    bool parseCount(std::string const& text, int& out) {
        std::string_view s = text;
        while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front()))) s.remove_prefix(1);
        while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) s.remove_suffix(1);
        if (s.empty()) return false;

        // Digits only. This also rejects "-5" and "+5".
        for (char c : s) {
            if (c < '0' || c > '9') return false;
        }

        int value = 0;
        auto [end, ec] = std::from_chars(s.data(), s.data() + s.size(), value);
        if (ec != std::errc() || end != s.data() + s.size()) return false; // overflow etc.

        out = value;
        return true;
    }

    std::string levelKey(GJGameLevel* level) {
        int type = static_cast<int>(level->m_levelType);
        int id = level->m_levelID.value();
        if (id > 0) {
            return fmt::format("t{}:{}", type, id);
        }
        // Rare fallback for a level that has no ID yet. Only the name is available.
        return fmt::format("t{}:0:{}", type, std::string(level->m_levelName));
    }

    Stats realStats(GJGameLevel* level) {
        Stats s;
        s.attempts = level->m_attempts.value();
        s.jumps = level->m_jumps.value();
        return s;
    }

    std::optional<Stats> loadOverride(std::string const& key) {
        auto raw = Mod::get()->getSavedValue<std::string>(storageKey(key), "");
        if (raw.empty()) return std::nullopt;

        auto comma = raw.find(',');
        if (comma == std::string::npos) return std::nullopt;

        Stats s;
        if (!parseCount(raw.substr(0, comma), s.attempts)) return std::nullopt;
        if (!parseCount(raw.substr(comma + 1), s.jumps)) return std::nullopt;
        return s;
    }

    void saveOverride(std::string const& key, Stats const& stats) {
        Mod::get()->setSavedValue<std::string>(
            storageKey(key),
            fmt::format("{},{}", stats.attempts, stats.jumps)
        );
    }

    void clearOverride(std::string const& key) {
        auto k = storageKey(key);
        if (Mod::get()->getSavedValue<std::string>(k, "").empty()) return; // nothing to undo
        Mod::get()->setSavedValue<std::string>(k, std::string());
    }
}
