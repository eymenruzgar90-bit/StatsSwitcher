#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/LevelPage.hpp>

#include "EditStatsPopup.hpp"
#include "RightClickLayer.hpp"
#include "StatsStore.hpp"

using namespace geode::prelude;

namespace {

    // While this object is alive, the level's Attempts/Jumps hold the player's
    // edited numbers. The real numbers are put back the moment it is destroyed,
    // so the game's actual statistics are never changed. The guard only lives for
    // the duration of the call that builds the vanilla info popup.
    class DisplayGuard {
    public:
        explicit DisplayGuard(GJGameLevel* level) {
            if (!level) return;
            if (!Mod::get()->getSettingValue<bool>("show-edited-values")) return;

            auto edited = lse::loadOverride(lse::levelKey(level));
            if (!edited) return;

            m_level = level;
            m_real = lse::realStats(level);
            level->m_attempts = edited->attempts;
            level->m_jumps = edited->jumps;
        }

        ~DisplayGuard() {
            if (!m_level) return;
            m_level->m_attempts = m_real.attempts;
            m_level->m_jumps = m_real.jumps;
        }

        DisplayGuard(DisplayGuard const&) = delete;
        DisplayGuard& operator=(DisplayGuard const&) = delete;

    private:
        GJGameLevel* m_level = nullptr;
        lse::Stats m_real;
    };
}

// The level page you get when you open an online / saved / daily level.
class $modify(StatsInfoLayer, LevelInfoLayer) {
    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

        auto rightClick = RightClickLayer::create(
            this,
            [this]() -> GJGameLevel* { return m_level; },
            [this]() -> CCNode* { return m_songWidget; } // the song widget has its own (i)
        );
        if (rightClick) this->addChild(rightClick);
        return true;
    }

    // Left click: vanilla behaviour, just with the edited numbers visible.
    void onInfo(CCObject* sender) {
        DisplayGuard guard(m_level);
        LevelInfoLayer::onInfo(sender);
    }

    void onLevelInfo(CCObject* sender) {
        DisplayGuard guard(m_level);
        LevelInfoLayer::onLevelInfo(sender);
    }
};

// The pages of the level-select screen (official levels).
class $modify(StatsLevelPage, LevelPage) {
    bool init(GJGameLevel* level) {
        if (!LevelPage::init(level)) return false;

        auto rightClick = RightClickLayer::create(
            this,
            [this]() -> GJGameLevel* { return m_level; },
            []() -> CCNode* { return nullptr; }
        );
        if (rightClick) this->addChild(rightClick);
        return true;
    }

    void onInfo(CCObject* sender) {
        DisplayGuard guard(m_level);
        LevelPage::onInfo(sender);
    }
};
