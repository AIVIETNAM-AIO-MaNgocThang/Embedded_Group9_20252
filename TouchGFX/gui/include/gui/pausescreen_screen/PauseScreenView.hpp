#ifndef PAUSESCREENVIEW_HPP
#define PAUSESCREENVIEW_HPP

#include <gui_generated/pausescreen_screen/PauseScreenViewBase.hpp>
#include <gui/pausescreen_screen/PauseScreenPresenter.hpp>

class PauseScreenView : public PauseScreenViewBase
{
public:
    PauseScreenView();
    virtual ~PauseScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // PAUSESCREENVIEW_HPP
