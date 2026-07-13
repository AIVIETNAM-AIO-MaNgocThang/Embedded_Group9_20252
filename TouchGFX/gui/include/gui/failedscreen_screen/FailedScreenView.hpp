#ifndef FAILEDSCREENVIEW_HPP
#define FAILEDSCREENVIEW_HPP

#include <gui_generated/failedscreen_screen/FailedScreenViewBase.hpp>
#include <gui/failedscreen_screen/FailedScreenPresenter.hpp>

class FailedScreenView : public FailedScreenViewBase
{
public:
    FailedScreenView();
    virtual ~FailedScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // FAILEDSCREENVIEW_HPP
