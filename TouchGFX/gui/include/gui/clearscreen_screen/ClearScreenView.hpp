#ifndef CLEARSCREENVIEW_HPP
#define CLEARSCREENVIEW_HPP

#include <gui_generated/clearscreen_screen/ClearScreenViewBase.hpp>
#include <gui/clearscreen_screen/ClearScreenPresenter.hpp>

class ClearScreenView : public ClearScreenViewBase
{
public:
    ClearScreenView();
    virtual ~ClearScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // CLEARSCREENVIEW_HPP
