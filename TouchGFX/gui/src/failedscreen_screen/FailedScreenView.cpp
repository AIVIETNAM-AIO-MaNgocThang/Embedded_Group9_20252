#include <gui/failedscreen_screen/FailedScreenView.hpp>
#include <touchgfx/Color.hpp>

FailedScreenView::FailedScreenView()
:tryAgainButtonClickedCallback(this, &FailedScreenView::tryAgainButtonClicked),
 returnMenuButtonClickedCallback(this, &FailedScreenView::returnMenuButtonClicked)
{

}

void FailedScreenView::setupScreen()
{
    FailedScreenViewBase::setupScreen();

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

    Unicode::snprintf(
		progressTextAreaBuffer,
		PROGRESSTEXTAREA_SIZE,
		"%d",
		presenter->getDistance());
    progressTextArea.resizeToCurrentText();
	progressTextArea.invalidate();

	// --- TryAgain button ---
	tryAgainButton.setAction(tryAgainButtonClickedCallback);

	// --- Return Menu button ---
	returnMenuButton.setAction(returnMenuButtonClickedCallback);

	invalidate();

}

void FailedScreenView::tearDownScreen()
{
    FailedScreenViewBase::tearDownScreen();
}

void FailedScreenView::tryAgainButtonClicked(const touchgfx::AbstractButtonContainer& src)
{
	presenter->resetAttempt();
	presenter->resetGameStats();
	application().gotoMenuScreenScreenNoTransition();
}

void FailedScreenView::returnMenuButtonClicked(const touchgfx::AbstractButtonContainer& src)
{
	presenter->increaseAttempt();
	presenter->resetGameStats();
	application().gotoGameScreenScreenNoTransition();
}
