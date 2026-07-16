#ifndef FAILEDSCREENPRESENTER_HPP
#define FAILEDSCREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class FailedScreenView;

class FailedScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    FailedScreenPresenter(FailedScreenView& v);

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

    virtual ~FailedScreenPresenter() {}

    // Lấy thông số
    uint32_t getAttemptCount() const;
	uint32_t getJumpCount() const;
	uint32_t getPlayTime() const;
	uint32_t getDistance() const;

	void resetAttempt();
	void resetGameStats();
	void increaseAttempt();
private:
    FailedScreenPresenter();

    FailedScreenView& view;
};

#endif // FAILEDSCREENPRESENTER_HPP
