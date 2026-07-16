#include <gui/gamescreen_screen/GameScreenView.hpp>
#include <gui/gamescreen_screen/GameScreenPresenter.hpp>

GameScreenPresenter::GameScreenPresenter(GameScreenView& v)
    : view(v)
{

}

void GameScreenPresenter::activate()
{

}

void GameScreenPresenter::deactivate()
{

}

void GameScreenPresenter::increaseAttempt()
{
    model->attemptCount++;
}

void GameScreenPresenter::increaseJump()
{
    model->jumpCount++;
}

void GameScreenPresenter::increasePlayTime()
{
    model->playTime++;
}

void GameScreenPresenter::increaseDistance(int16_t delta)
{
    model->distance += delta;
}

void GameScreenPresenter::resetGameStats()
{
    model->resetGameStats();
}

void GameScreenPresenter::resetAttempt()
{
    model->attemptCount = 1;
}

uint32_t GameScreenPresenter::getAttemptCount() const
{
    return model->attemptCount;
}

uint32_t GameScreenPresenter::getJumpCount() const
{
    return model->jumpCount;
}

uint32_t GameScreenPresenter::getPlayTime() const
{
    return model->playTime;
}

uint32_t GameScreenPresenter::getDistance() const
{
    return model->distance;
}
