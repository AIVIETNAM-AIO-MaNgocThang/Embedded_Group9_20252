#ifndef CLEARSCREENPRESENTER_HPP
#define CLEARSCREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class ClearScreenView;

class ClearScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    ClearScreenPresenter(ClearScreenView& v);

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

    virtual ~ClearScreenPresenter() {}

    // Lấy thông tin
    uint32_t getAttemptCount() const;
    uint32_t getJumpCount() const;
    uint32_t getPlayTime() const;

	void resetAttempt();
	void resetGameStats();
private:
    ClearScreenPresenter();

    ClearScreenView& view;
};

#endif // CLEARSCREENPRESENTER_HPP
