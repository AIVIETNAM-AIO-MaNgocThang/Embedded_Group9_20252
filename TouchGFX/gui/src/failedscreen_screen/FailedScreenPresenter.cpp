#include <gui/failedscreen_screen/FailedScreenView.hpp>
#include <gui/failedscreen_screen/FailedScreenPresenter.hpp>

FailedScreenPresenter::FailedScreenPresenter(FailedScreenView& v)
    : view(v)
{

}

void FailedScreenPresenter::activate()
{

}

void FailedScreenPresenter::deactivate()
{

}

uint32_t FailedScreenPresenter::getAttemptCount() const
{
    return model->attemptCount;
}

uint32_t FailedScreenPresenter::getJumpCount() const
{
    return model->jumpCount;
}

uint32_t FailedScreenPresenter::getPlayTime() const
{
    return model->playTime;
}

uint32_t FailedScreenPresenter::getDistance() const
{
    return model->distance;
}

void FailedScreenPresenter::resetAttempt ()
{
	model->attemptCount = 1;
}

void FailedScreenPresenter::resetGameStats()
{
	model->resetGameStats();
}

void FailedScreenPresenter::increaseAttempt()
{
	model->attemptCount++;
}
