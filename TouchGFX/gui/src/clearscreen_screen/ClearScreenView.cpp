#include <gui/clearscreen_screen/ClearScreenView.hpp>

ClearScreenView::ClearScreenView()
:tryAgainButtonClickedCallback(this, &ClearScreenView::tryAgainButtonClicked),
 returnMenuButtonClickedCallback(this, &ClearScreenView::returnMenuButtonClicked)
{

}

void ClearScreenView::setupScreen()
{
    ClearScreenViewBase::setupScreen();

    Unicode::snprintf(
            attemptsTextAreaBuffer,
            ATTEMPTSTEXTAREA_SIZE,
            "%d",
    		presenter->getAttemptCount());
	attemptsTextArea.resizeToCurrentText();
	attemptsTextArea.invalidate();

	Unicode::snprintf(
		jumpsTextAreaBuffer,
		JUMPSTEXTAREA_SIZE,
		"%d",
		presenter->getJumpCount());
	jumpsTextArea.resizeToCurrentText();
	jumpsTextArea.invalidate();

	Unicode::snprintf(
		timeTextAreaBuffer,
		TIMETEXTAREA_SIZE,
		"%d s",
		presenter->getPlayTime());
	timeTextArea.resizeToCurrentText();
	timeTextArea.invalidate();

	// --- TryAgain button ---
	tryAgainButton.setAction(tryAgainButtonClickedCallback);

	// --- Return Menu button ---
	returnMenuButton.setAction(returnMenuButtonClickedCallback);
}

void ClearScreenView::tearDownScreen()
{
    ClearScreenViewBase::tearDownScreen();
}

void ClearScreenView::tryAgainButtonClicked(const touchgfx::AbstractButtonContainer& src)
{
	presenter->resetAttempt();
	presenter->resetGameStats();
	application().gotoMenuScreenScreenNoTransition();
}

void ClearScreenView::returnMenuButtonClicked(const touchgfx::AbstractButtonContainer& src)
{
	presenter->resetAttempt();
	presenter->resetGameStats();
	application().gotoGameScreenScreenNoTransition();
}
