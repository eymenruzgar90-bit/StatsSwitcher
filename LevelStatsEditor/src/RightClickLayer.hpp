#pragma once

#include <Geode/Geode.hpp>
#include <functional>
#include <vector>

// An invisible layer that is added as a child of a LevelInfoLayer / LevelPage.
// It listens for the right mouse button (CCMouseDelegate::rightKeyDown, which the
// game delivers to every mouse-enabled CCLayer) and, if the cursor is over the
// level's info (i) button, opens the editor.
//
// It is NOT touch-enabled, so it can never swallow or trigger a left click.
class RightClickLayer : public cocos2d::CCLayer {
public:
    using LevelGetter = std::function<GJGameLevel*()>;
    using NodeGetter = std::function<cocos2d::CCNode*()>;

    // owner:  the layer this node lives in (searched for the info button)
    // level:  returns the level the owner currently shows
    // skip:   returns a subtree to ignore (e.g. the song widget has its own info button); may be null
    static RightClickLayer* create(cocos2d::CCNode* owner, LevelGetter level, NodeGetter skip);

    void rightKeyDown() override;

protected:
    bool init(cocos2d::CCNode* owner, LevelGetter level, NodeGetter skip);

    CCMenuItemSpriteExtra* findInfoButtonUnder(cocos2d::CCPoint const& mouse) const;

    cocos2d::CCNode* m_owner = nullptr; // our parent; it always outlives this node
    LevelGetter m_getLevel;
    NodeGetter m_getSkip;
};
