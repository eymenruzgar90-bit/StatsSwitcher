#include "RightClickLayer.hpp"
#include "EditStatsPopup.hpp"

#include <array>

using namespace geode::prelude;

namespace {

    // Sprite frames used by the (i) button in the level pages.
    constexpr std::array<char const*, 2> kInfoFrames = {
        "GJ_infoIcon_001.png",
        "GJ_infoBtn_001.png",
    };

    // Used only if no button with one of the sprites above exists
    // (these IDs are provided by the optional Geode "Node IDs" mod).
    constexpr std::array<char const*, 2> kInfoIds = {
        "info-button",
        "level-info-button",
    };

    void collectButtons(CCNode* node, CCNode* skip, std::vector<CCMenuItemSpriteExtra*>& out, int depth) {
        if (!node || depth > 10) return;
        auto children = node->getChildren();
        if (!children) return;
        for (auto child : CCArrayExt<CCNode*>(children)) {
            if (!child || child == skip) continue;
            if (auto button = typeinfo_cast<CCMenuItemSpriteExtra*>(child)) {
                out.push_back(button);
            }
            else {
                collectButtons(child, skip, out, depth + 1);
            }
        }
    }

    bool looksLikeInfoButton(CCNode* button) {
        for (auto name : kInfoFrames) {
            if (geode::cocos::isSpriteFrameName(button, name)) return true;
        }
        return false;
    }

    bool isMouseOver(CCNode* node, CCPoint const& mouse) {
        for (CCNode* p = node; p; p = p->getParent()) {
            if (!p->isVisible()) return false;
        }
        auto local = node->convertToNodeSpace(mouse);
        auto size = node->getContentSize();
        return CCRect(0.f, 0.f, size.width, size.height).containsPoint(local);
    }

    // True when any popup (including our own editor and the vanilla info popup)
    // is already on top of the scene.
    bool overlayPresent() {
        auto scene = CCDirector::sharedDirector()->getRunningScene();
        if (!scene) return false;
        auto children = scene->getChildren();
        if (!children) return false;
        for (auto child : CCArrayExt<CCNode*>(children)) {
            if (typeinfo_cast<FLAlertLayer*>(child)) return true;
        }
        return false;
    }
}

RightClickLayer* RightClickLayer::create(CCNode* owner, LevelGetter level, NodeGetter skip) {
    auto ret = new RightClickLayer();
    if (ret->init(owner, std::move(level), std::move(skip))) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool RightClickLayer::init(CCNode* owner, LevelGetter level, NodeGetter skip) {
    if (!CCLayer::init()) return false;

    m_owner = owner;
    m_getLevel = std::move(level);
    m_getSkip = std::move(skip);

    this->setTouchEnabled(false);   // never interferes with left clicks
    this->setMouseEnabled(true);    // registers with the game's mouse dispatcher
    return true;
}

CCMenuItemSpriteExtra* RightClickLayer::findInfoButtonUnder(CCPoint const& mouse) const {
    CCNode* skip = m_getSkip ? m_getSkip() : nullptr;

    std::vector<CCMenuItemSpriteExtra*> buttons;
    collectButtons(m_owner, skip, buttons, 0);

    bool foundAnyInfoButton = false;
    for (auto button : buttons) {
        if (!looksLikeInfoButton(button)) continue;
        foundAnyInfoButton = true;
        if (isMouseOver(button, mouse)) return button;
    }

    if (!foundAnyInfoButton) {
        for (auto id : kInfoIds) {
            auto node = m_owner->getChildByIDRecursive(id);
            if (!node) continue;
            if (auto button = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
                if (isMouseOver(button, mouse)) return button;
            }
        }
    }
    return nullptr;
}

void RightClickLayer::rightKeyDown() {
    if (!m_owner || !m_owner->isRunning() || !m_owner->isVisible()) return;
    if (overlayPresent()) return;

    auto level = m_getLevel ? m_getLevel() : nullptr;
    if (!level) return;

    if (!this->findInfoButtonUnder(geode::cocos::getMousePos())) return;

    EditStatsPopup::openFor(level);
}
