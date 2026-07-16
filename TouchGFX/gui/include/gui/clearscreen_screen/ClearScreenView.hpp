#ifndef CLEARSCREENVIEW_HPP
#define CLEARSCREENVIEW_HPP

#include <gui_generated/clearscreen_screen/ClearScreenViewBase.hpp>
#include <gui/clearscreen_screen/ClearScreenPresenter.hpp>
#include <touchgfx/Callback.hpp>

class ClearScreenView : public ClearScreenViewBase
{
public:
    ClearScreenView();
    virtual ~ClearScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

protected:
    // ---- Try Again ----
    touchgfx::Callback<ClearScreenView, const touchgfx::AbstractButtonContainer&> tryAgainButtonClickedCallback;
	void tryAgainButtonClicked(const touchgfx::AbstractButtonContainer& src);

    // ---- Return Menu ----
    touchgfx::Callback<ClearScreenView, const touchgfx::AbstractButtonContainer&> returnMenuButtonClickedCallback;
	void returnMenuButtonClicked(const touchgfx::AbstractButtonContainer& src);
};

#endif // CLEARSCREENVIEW_HPP
