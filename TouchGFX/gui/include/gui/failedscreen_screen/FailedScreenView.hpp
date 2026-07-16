#ifndef FAILEDSCREENVIEW_HPP
#define FAILEDSCREENVIEW_HPP

#include <gui_generated/failedscreen_screen/FailedScreenViewBase.hpp>
#include <gui/failedscreen_screen/FailedScreenPresenter.hpp>
#include <touchgfx/Callback.hpp>

class FailedScreenView : public FailedScreenViewBase
{
public:
    FailedScreenView();
    virtual ~FailedScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
    // ---- Try Again ----
    touchgfx::Callback<FailedScreenView, const touchgfx::AbstractButtonContainer&> tryAgainButtonClickedCallback;
	void tryAgainButtonClicked(const touchgfx::AbstractButtonContainer& src);

    // ---- Return Menu ----
    touchgfx::Callback<FailedScreenView, const touchgfx::AbstractButtonContainer&> returnMenuButtonClickedCallback;
	void returnMenuButtonClicked(const touchgfx::AbstractButtonContainer& src);
};

#endif // FAILEDSCREENVIEW_HPP
