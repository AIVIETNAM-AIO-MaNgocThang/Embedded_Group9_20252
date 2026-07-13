#ifndef PAUSESCREENPRESENTER_HPP
#define PAUSESCREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class PauseScreenView;

class PauseScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    PauseScreenPresenter(PauseScreenView& v);

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

    virtual ~PauseScreenPresenter() {}

private:
    PauseScreenPresenter();

    PauseScreenView& view;
};

#endif // PAUSESCREENPRESENTER_HPP
