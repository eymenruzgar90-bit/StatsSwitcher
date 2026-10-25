#pragma once

#include <Geode/Geode.hpp>
#include <optional>
#include <string>

// "lse" = Level Stats Editor. Everything that touches saved data lives here.
namespace lse {

    struct Stats {
        int attempts = 0;
        int jumps = 0;
    };

    // Unique, collision-safe identifier for a level, e.g. "t1:1" (an official level)
    // or "t4:128" (an online level). The level type is part of the key so an
    // official level and an online level can never share the same entry.
    std::string levelKey(GJGameLevel* level);

    // The game's real, untouched numbers.
    Stats realStats(GJGameLevel* level);

    // The player's saved edit for this level, or nullopt if there is none
    // (or if the stored data is missing or corrupted).
    std::optional<Stats> loadOverride(std::string const& key);

    void saveOverride(std::string const& key, Stats const& stats);

    // Removes the edit. Does nothing (writes nothing) if there is no edit.
    void clearOverride(std::string const& key);

    // Strict whole-number parser: digits only, no sign, no overflow, not empty.
    bool parseCount(std::string const& text, int& out);
}
