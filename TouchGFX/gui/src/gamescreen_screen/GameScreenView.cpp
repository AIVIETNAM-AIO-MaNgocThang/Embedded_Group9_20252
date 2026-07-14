#include <gui/gamescreen_screen/GameScreenView.hpp>
#include <touchgfx/Color.hpp>

GameScreenView::GameScreenView()
    : playerVelY(0),
	  isJumping(false),
	  spawnTimer(0),
	  spawnInterval(90),
	  pauseButtonClickedCallback(this, &GameScreenView::pauseButtonClicked),
	  continueButtonClickedCallback(this, &GameScreenView::continueButtonClicked),
	  tryAgainButtonClickedCallback(this, &GameScreenView::tryAgainButtonClicked),
	  quitButtonClickedCallback(this, &GameScreenView::quitButtonClicked),
	  isPaused(false),
	  ignoreNextClick(false),
	  gameOverTriggered(false)
{
    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        obstacles[i].active = false;
    }
}

void GameScreenView::setupScreen()
{
    GameScreenViewBase::setupScreen();

    // --- Nền cuộn ---
    bg1.setBitmap(Bitmap(2));
    bg1.setXY(0, 0);
    add(bg1);

    bg2.setBitmap(Bitmap(2));
    bg2.setXY(BG_WIDTH, 0);
    add(bg2);

    // --- Nhân vật ---
    player.setBitmap(Bitmap(10));
    player.setXY(96, 140);
    add(player);

    // --- Obstacle: add sẵn vào tree nhưng ẩn, kích hoạt khi spawn ---
    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        obstacles[i].img.setBitmap(Bitmap(3)); // TODO: đổi tên đúng khi import ảnh
        obstacles[i].img.setVisible(false);
        add(obstacles[i].img);
    }

    // --- Pause button ---
    remove(pauseButton);
    add(pauseButton);

    remove(pauseOverlay);
    add(pauseOverlay);

    pauseOverlay.setVisible(false);

    pauseButton.setAction(pauseButtonClickedCallback);
    pauseButton.setVisible(true);

    // --- Continue button ---
    continueButton.setAction(continueButtonClickedCallback);


    // --- TryAgain button ---
    tryAgainButton.setAction(tryAgainButtonClickedCallback);

    // --- Quit button ---
    quitButton.setAction(quitButtonClickedCallback);
    invalidate();
}

void GameScreenView::tearDownScreen()
{
    GameScreenViewBase::tearDownScreen();
}

void GameScreenView::handleTickEvent()
{
    GameScreenViewBase::handleTickEvent();

    // ===== Pause =====
    if (isPaused) return;

    // ===== Cuộn nền =====
    bg1.moveTo(bg1.getX() - SCROLL_SPEED, bg1.getY());
    bg2.moveTo(bg2.getX() - SCROLL_SPEED, bg2.getY());

    if (bg1.getX() <= -BG_WIDTH) bg1.moveTo(bg2.getX() + BG_WIDTH, bg1.getY());
    if (bg2.getX() <= -BG_WIDTH) bg2.moveTo(bg1.getX() + BG_WIDTH, bg2.getY());

    // ===== Vật lý nhân vật (nhảy/rơi) =====
    playerVelY += GRAVITY;
    int16_t newY = player.getY() + (int16_t)playerVelY;

    if (newY >= 140)
    {
        newY = 140;
        playerVelY = 0;
        isJumping = false;
    }
    player.moveTo(player.getX(), newY);

    // ===== Spawn & update obstacle =====
    spawnTimer++;
    if (spawnTimer >= spawnInterval)
    {
        spawnObstacle();
        spawnTimer = 0;
    }
    updateObstacles();
}

// Tạo vật cản
void GameScreenView::spawnObstacle()
{
    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        if (!obstacles[i].active)
        {
            obstacles[i].img.setXY(320, 140); // chỉnh Y theo chiều cao ảnh obstacle thật
            obstacles[i].img.setVisible(true);
            obstacles[i].active = true;
            invalidate();
            break;
        }
    }
}

void GameScreenView::updateObstacles()
{
    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        if (!obstacles[i].active) continue;

        obstacles[i].img.moveTo(obstacles[i].img.getX() - SCROLL_SPEED,
                                 obstacles[i].img.getY());

        if (obstacles[i].img.getX() < -(int16_t)obstacles[i].img.getWidth())
        {
            obstacles[i].img.setVisible(false);
            obstacles[i].active = false;
        }

        if (!gameOverTriggered && checkCollision(player, obstacles[i].img))
        {
            onPlayerHit();
            return; // thoát sớm, khỏi cần xử lý tiếp obstacle khác trong frame này
        }
    }
}

bool GameScreenView::checkCollision(const Image& a, const Image& b)
{
    // AABB đơn giản, có thể thu nhỏ hitbox (-2px mỗi cạnh) để dễ chơi hơn
    Rect ra(a.getX(), a.getY(), a.getWidth(), a.getHeight());
    Rect rb(b.getX(), b.getY(), b.getWidth(), b.getHeight());
    return ra.intersect(rb);
}

void GameScreenView::onPlayerHit()
{
    if (gameOverTriggered) return;
    gameOverTriggered = true;

    isPaused = true; // chặn tick tiếp tục chạy trong lúc chuyển màn hình

    application().gotoFailedScreenScreenNoTransition();
    // hoặc dùng transition nếu bạn đã cấu hình trong Designer:
    // application().gotoResultFailedScreen();
}

void GameScreenView::handleClickEvent(const touchgfx::ClickEvent& evt)
{
    GameScreenViewBase::handleClickEvent(evt);

    if (isPaused) return;

    // ===== Continue logic =====
    	if (ignoreNextClick)
    		{
    			ignoreNextClick = false;
    			return;
    		}
    if (evt.getType() == ClickEvent::PRESSED && !isJumping)
    {
        playerVelY = JUMP_STRENGTH;
        isJumping = true;
    }
}

void GameScreenView::pauseButtonClicked(const touchgfx::AbstractButtonContainer& src)
{
    isPaused = true;

    pauseOverlay.setVisible(true);
    pauseOverlay.invalidate();
}

void GameScreenView::continueButtonClicked(const touchgfx::AbstractButtonContainer& src)
{
    isPaused = false;

    ignoreNextClick = true;

    pauseOverlay.setVisible(false);
    pauseOverlay.invalidate();
}

void GameScreenView::tryAgainButtonClicked(const touchgfx::AbstractButtonContainer& src)
{
	application().gotoFailedScreenScreenNoTransition();
}

void GameScreenView::quitButtonClicked(const touchgfx::AbstractButtonContainer& src)
{
	application().gotoGameScreenScreenNoTransition();
}
