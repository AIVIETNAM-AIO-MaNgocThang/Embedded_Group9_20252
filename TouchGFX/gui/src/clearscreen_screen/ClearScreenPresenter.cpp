#include <gui/clearscreen_screen/ClearScreenView.hpp>
#include <gui/clearscreen_screen/ClearScreenPresenter.hpp>

ClearScreenPresenter::ClearScreenPresenter(ClearScreenView& v)
    : view(v)
{

}

void ClearScreenPresenter::activate()
{

}

void ClearScreenPresenter::deactivate()
{

}

uint32_t ClearScreenPresenter::getAttemptCount() const
{
    return model->attemptCount;
}

uint32_t ClearScreenPresenter::getJumpCount() const
{
    return model->jumpCount;
}

uint32_t ClearScreenPresenter::getPlayTime() const
{
    return model->playTime;
}

void ClearScreenPresenter::resetAttempt ()
{
	model->attemptCount = 1;
}

void ClearScreenPresenter::resetGameStats()
{
	model->resetGameStats();
}
