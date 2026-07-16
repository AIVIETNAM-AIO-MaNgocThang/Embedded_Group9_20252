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
	  gameOverTriggered(false),
	  previousPlayerY(140),
	  tickCounter(0)
{
    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        obstacles[i].active = false;
    }

    for (int i = 0; i < MAX_PLATFORMS; i++)
    {
        platforms[i].active = false;
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
    player.setBitmap(Bitmap(13));
    player.setXY(96, 140);
    add(player);

    // --- Obstacle: add sẵn vào tree nhưng ẩn, kích hoạt khi spawn ---
    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        obstacles[i].img.setBitmap(Bitmap(10)); // TODO: đổi tên đúng khi import ảnh
        obstacles[i].img.setVisible(false);
        add(obstacles[i].img);
    }

    // ===== Platform =====
    for (int i = 0; i < MAX_PLATFORMS; i++)
    {
        platforms[i].img.setBitmap(Bitmap(12)); // đổi ID ảnh platform
        platforms[i].img.setVisible(false);
        platforms[i].active = false;

        add(platforms[i].img);
    }

    // ===== Step =====
    for (int i = 0; i < MAX_STEPS; i++)
    {
    	for (int j = 0; j < MAX_STEP_HEIGHT; j++)
    	{
    		steps[i].blocks[j].setBitmap(Bitmap(6)); // đổi ID ảnh platform
    		steps[i].blocks[j].setVisible(false);
    	}
        steps[i].active = false;

        for (int j = 0; j < MAX_STEP_HEIGHT; j++)
        {
        	add(steps[i].blocks[j]);
        }
   }

    previousPlayerY = player.getY();
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

    // ==== Time ====
    tickCounter++;

    if (tickCounter >= 60) // CHÚ Ý: FPS HIỆN TẠI ĐANG SET LÀ 60
    {
        tickCounter = 0;
        presenter->increasePlayTime();
    }

    // ===== Cuộn nền =====
    bg1.moveTo(bg1.getX() - SCROLL_SPEED, bg1.getY());
    bg2.moveTo(bg2.getX() - SCROLL_SPEED, bg2.getY());

    presenter->increaseDistance(SCROLL_SPEED);

    if (bg1.getX() <= -BG_WIDTH) bg1.moveTo(bg2.getX() + BG_WIDTH, bg1.getY());
    if (bg2.getX() <= -BG_WIDTH) bg2.moveTo(bg1.getX() + BG_WIDTH, bg2.getY());

    // ===== Vật lý nhân vật (nhảy/rơi) + Platform =====
    previousPlayerY = player.getY();

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

    if (spawnTimer == spawnInterval) spawnObstacle();
    else if (spawnTimer == 2 * spawnInterval) spawnPlatform();
    else if (spawnTimer == 3 * spawnInterval) spawnStep(1);
    else if (spawnTimer >= 4 * spawnInterval)
    {
    	spawnStep(2);
        spawnTimer = 0;
    }

    updateObstacles();
    updatePlatforms();
    updateSteps();
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

// Tạo các phiến nhảy
void GameScreenView::spawnPlatform()
{
    for (int i = 0; i < MAX_PLATFORMS; i++)
    {
        if (!platforms[i].active)
        {
            platforms[i].img.setXY(250, 120);
            platforms[i].img.setVisible(true);
            platforms[i].active = true;
            invalidate();
            break;
        }
    }
}

void GameScreenView::updatePlatforms()
{
    for (int i = 0; i < MAX_PLATFORMS; i++)
    {
        if (!platforms[i].active)
        {
            continue;
        }

        Platform& platform = platforms[i];

        // Di chuyển platform sang trái
        platform.img.moveTo(
            platform.img.getX() - SCROLL_SPEED,
            platform.img.getY()
        );

        // Kiểm tra player đáp xuống
        checkPlatformLanding(platform);

        // Platform ra màn hình
        if (platform.img.getX() <
            -(int16_t)platform.img.getWidth())
        {
            platform.img.setVisible(false);
            platform.active = false;
        }
    }
}

bool GameScreenView::checkPlatformLanding(
    Platform& platform
)
{
    // ===== Hitbox platform =====
    const int16_t PLATFORM_TOP_OFFSET = 8;
    const int16_t PLATFORM_REAL_HEIGHT = 8;

    // ===== Tọa độ player =====
    int16_t playerLeft =
        player.getX();

    int16_t playerRight =
        player.getX() + player.getWidth();

    int16_t previousPlayerTop =
        previousPlayerY;

    int16_t previousPlayerBottom =
        previousPlayerY + player.getHeight();

    int16_t currentPlayerTop =
        player.getY();

    int16_t currentPlayerBottom =
        player.getY() + player.getHeight();

    // ===== Tọa độ platform =====
    int16_t platformLeft =
        platform.img.getX();

    int16_t platformRight =
        platform.img.getX() +
        platform.img.getWidth();

    int16_t platformTop =
        platform.img.getY() +
        PLATFORM_TOP_OFFSET;

    int16_t platformBottom =
        platformTop +
        PLATFORM_REAL_HEIGHT;

    // ===== Kiểm tra overlap chiều ngang =====
    bool horizontalOverlap =
        playerRight > platformLeft &&
        playerLeft < platformRight;

    if (!horizontalOverlap)
    {
        return false;
    }

    // ===== Player đang rơi xuống =====
    if (playerVelY > 0)
    {
        if (previousPlayerBottom <= platformTop &&
            currentPlayerBottom >= platformTop)
        {
            player.moveTo(
                player.getX(),
                platformTop - player.getHeight()
            );

            playerVelY = 0;
            isJumping = false;

            return true;
        }
    }

    // ===== Player đang nhảy lên =====
    if (playerVelY < 0)
    {
        if (previousPlayerTop >= platformBottom &&
            currentPlayerTop <= platformBottom)
        {
            player.moveTo(
                player.getX(),
                platformBottom
            );

            playerVelY = 0;

            return true;
        }
    }

    return false;
}

void GameScreenView::spawnStep(uint8_t h)
{
    for (int i = 0; i < MAX_STEPS; i++)
    {
        if (!steps[i].active)
        {
            steps[i].height = h;
            steps[i].active = true;

            for (int j = 0; j < MAX_STEP_HEIGHT; j++)
            {
                if (j < h)
                {
                    // Ô j: đặt từ mặt đất (GROUND_Y) chồng lên
                    int16_t blockY = GROUND_Y - (j * 16);
                    steps[i].blocks[j].setXY(250, blockY);
                    steps[i].blocks[j].setVisible(true);
                }
                else
                {
                    steps[i].blocks[j].setVisible(false);
                }
            }
            invalidate();
            break;
        }
    }
}

bool GameScreenView::checkStepCollision(Step& step)
{
    // Bounding box tổng thể của step
	int16_t stepLeft = step.blocks[0].getX();
    int16_t stepRight  = stepLeft + 16;
    int16_t stepTop    = GROUND_Y - (step.height - 1) * 16;  // đỉnh ô cao nhất
    int16_t stepBottom = GROUND_Y + 16;  // đáy = mặt đất

    int16_t pLeft   = player.getX();
    int16_t pRight  = pLeft + 16;
    int16_t pTop    = player.getY();
    int16_t pBottom = pTop + 16;
    int16_t prevTop = previousPlayerY;
    int16_t prevBot = previousPlayerY + 16;

    // Kiểm tra overlap cơ bản
    if (pRight <= stepLeft || pLeft >= stepRight) return false;
    if (pBottom <= stepTop || pTop >= stepBottom) return false;

    // --- Ưu tiên 1: Landing từ trên ---
    if (playerVelY >= 0 && prevBot <= stepTop)
    {
        // Player rơi xuống từ phía trên → đáp xuống
        player.moveTo(pLeft, stepTop - 16);
        playerVelY = 0;
        isJumping = false;
        return true; // safe landing
    }

    // --- Ưu tiên 2: Va chạm cạnh bên (từ trái) ---
    // Player đang di chuyển tương đối sang phải (step di chuyển sang trái)
    // → player chạm mặt bên trái của step → CHẾT
    if (pRight > stepLeft && pLeft < stepLeft)
    {
        onPlayerHit(); // CHẾT
        return true;
    }

    // --- Ưu tiên 3: Va chạm từ dưới ---
    if (playerVelY < 0 && prevTop >= stepBottom)
    {
        // Player nhảy lên chạm đáy step → chặn lại
        player.moveTo(pLeft, stepBottom);
        playerVelY = 0;
        return true;
    }

    // Các trường hợp còn lại → CHẾT
    onPlayerHit();
    return true;
}

void GameScreenView::updateSteps()
{
    for (int i = 0; i < MAX_STEPS; i++)
    {
        if (!steps[i].active) continue;

        // Di chuyển tất cả block sang trái
        for (int j = 0; j < steps[i].height; j++)
        {
            steps[i].blocks[j].moveTo(
                steps[i].blocks[j].getX() - SCROLL_SPEED,
                steps[i].blocks[j].getY()
            );
        }

        // Kiểm tra ra khỏi màn hình
        if (steps[i].blocks[0].getX() < -16)
        {
            for (int j = 0; j < MAX_STEP_HEIGHT; j++)
                steps[i].blocks[j].setVisible(false);
            steps[i].active = false;
            continue;
        }

        // Kiểm tra va chạm
        if (!gameOverTriggered)
            checkStepCollision(steps[i]);
    }
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
    	presenter->increaseJump();

        playerVelY = JUMP_STRENGTH;
        isJumping = true;
    }
}

void GameScreenView::pauseButtonClicked(const touchgfx::AbstractButtonContainer& src)
{
    isPaused = true;

    pauseOverlay.setVisible(true);
    pauseOverlay.invalidate();

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
	presenter->increaseAttempt();
	presenter->resetGameStats();
	application().gotoGameScreenScreenNoTransition();
}
