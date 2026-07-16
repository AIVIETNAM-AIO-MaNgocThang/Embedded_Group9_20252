#ifndef GAMESCREENPRESENTER_HPP
#define GAMESCREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class GameScreenView;

class GameScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    GameScreenPresenter(GameScreenView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    // ===== Game Statistics =====
    void increaseAttempt();
    void increaseJump();
    void increasePlayTime();
    void increaseDistance(int16_t delta);
    void resetAttempt();
    void resetGameStats();

    uint32_t getAttemptCount() const;
    uint32_t getJumpCount() const;
    uint32_t getPlayTime() const;
    uint32_t getDistance() const;

    virtual ~GameScreenPresenter() {}

private:
    GameScreenPresenter();

    GameScreenView& view;
};

#endif // GAMESCREENPRESENTER_HPP
