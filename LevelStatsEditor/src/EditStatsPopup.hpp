#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/TextInput.hpp>
#include "StatsStore.hpp"

// The Attempts / Jumps editor menu.
class EditStatsPopup : public geode::Popup {
public:
    // Builds and shows the editor for one level.
    static void openFor(GJGameLevel* level);

protected:
    static EditStatsPopup* create(
        std::string key, std::string levelName, lse::Stats real, lse::Stats shown
    );
    bool init(
        std::string key, std::string levelName, lse::Stats real, lse::Stats shown
    );

    void onDone(cocos2d::CCObject* sender);
    void onReset(cocos2d::CCObject* sender);

    std::string m_key;
    lse::Stats m_real;
    geode::TextInput* m_attemptsInput = nullptr;
    geode::TextInput* m_jumpsInput = nullptr;
};
