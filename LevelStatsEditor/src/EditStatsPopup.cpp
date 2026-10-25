#include "EditStatsPopup.hpp"

#include <Geode/ui/Notification.hpp>

using namespace geode::prelude;

void EditStatsPopup::openFor(GJGameLevel* level) {
    if (!level) return;

    auto key = lse::levelKey(level);
    auto real = lse::realStats(level);
    auto shown = lse::loadOverride(key).value_or(real);

    auto popup = EditStatsPopup::create(key, std::string(level->m_levelName), real, shown);
    if (popup) popup->show();
}

EditStatsPopup* EditStatsPopup::create(
    std::string key, std::string levelName, lse::Stats real, lse::Stats shown
) {
    auto ret = new EditStatsPopup();
    if (ret->init(std::move(key), std::move(levelName), real, shown)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool EditStatsPopup::init(
    std::string key, std::string levelName, lse::Stats real, lse::Stats shown
) {
    if (!Popup::init(280.f, 280.f)) return false;

    m_key = std::move(key);
    m_real = real;

    this->setTitle("Edit Attempts & Jumps");

    // Level name (small, shrunk if it is too long)
    auto nameLabel = CCLabelBMFont::create(levelName.c_str(), "bigFont.fnt");
    nameLabel->setScale(0.5f);
    if (nameLabel->getContentSize().width * 0.5f > 220.f) {
        nameLabel->setScale(220.f / nameLabel->getContentSize().width);
    }
    m_mainLayer->addChildAtPosition(nameLabel, Anchor::Center, ccp(0.f, 100.f));

    // Attempts
    auto attemptsLabel = CCLabelBMFont::create("Attempts", "goldFont.fnt");
    attemptsLabel->setScale(0.7f);
    m_mainLayer->addChildAtPosition(attemptsLabel, Anchor::Center, ccp(0.f, 72.f));

    m_attemptsInput = TextInput::create(180.f, "Attempts", "bigFont.fnt");
    m_attemptsInput->setFilter("0123456789");
    m_attemptsInput->setMaxCharCount(10);
    m_attemptsInput->setString(std::to_string(shown.attempts));
    m_mainLayer->addChildAtPosition(m_attemptsInput, Anchor::Center, ccp(0.f, 46.f));

    // Jumps
    auto jumpsLabel = CCLabelBMFont::create("Jumps", "goldFont.fnt");
    jumpsLabel->setScale(0.7f);
    m_mainLayer->addChildAtPosition(jumpsLabel, Anchor::Center, ccp(0.f, 12.f));

    m_jumpsInput = TextInput::create(180.f, "Jumps", "bigFont.fnt");
    m_jumpsInput->setFilter("0123456789");
    m_jumpsInput->setMaxCharCount(10);
    m_jumpsInput->setString(std::to_string(shown.jumps));
    m_mainLayer->addChildAtPosition(m_jumpsInput, Anchor::Center, ccp(0.f, -14.f));

    // Done
    auto doneBtn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Done"), this, menu_selector(EditStatsPopup::onDone)
    );
    m_buttonMenu->addChildAtPosition(doneBtn, Anchor::Center, ccp(0.f, -58.f));

    // Reset: directly underneath Done
    auto resetBtn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Reset"), this, menu_selector(EditStatsPopup::onReset)
    );
    m_buttonMenu->addChildAtPosition(resetBtn, Anchor::Center, ccp(0.f, -98.f));

    // Honest note: these numbers are display-only.
    auto note = CCLabelBMFont::create("Display only. Real stats are not changed.", "chatFont.fnt");
    note->setScale(0.5f);
    note->setOpacity(160);
    m_mainLayer->addChildAtPosition(note, Anchor::Center, ccp(0.f, -128.f));

    return true;
}

void EditStatsPopup::onDone(CCObject*) {
    lse::Stats entered;

    if (!lse::parseCount(std::string(m_attemptsInput->getString()), entered.attempts)) {
        Notification::create(
            "Attempts must be a whole number from 0 to 2147483647.", NotificationIcon::Error
        )->show();
        return;
    }
    if (!lse::parseCount(std::string(m_jumpsInput->getString()), entered.jumps)) {
        Notification::create(
            "Jumps must be a whole number from 0 to 2147483647.", NotificationIcon::Error
        )->show();
        return;
    }

    if (entered.attempts == m_real.attempts && entered.jumps == m_real.jumps) {
        // Same as the real numbers: an edit would be pointless, so remove any old one.
        lse::clearOverride(m_key);
    }
    else {
        lse::saveOverride(m_key, entered);
    }

    this->onClose(nullptr);
}

void EditStatsPopup::onReset(CCObject*) {
    // Restores the real, pre-edit numbers. (They were never overwritten.)
    lse::clearOverride(m_key);

    m_attemptsInput->setString(std::to_string(m_real.attempts));
    m_jumpsInput->setString(std::to_string(m_real.jumps));

    Notification::create("Original values restored.", NotificationIcon::Success)->show();
}
