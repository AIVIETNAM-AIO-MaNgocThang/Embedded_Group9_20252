#include <gui/gamescreen_screen/GameScreenView.hpp>
#include <touchgfx/Color.hpp>
#include <cstdlib>   // cho rand()
#include <algorithm>

GameScreenView::GameScreenView()
    : playerVelY(0),
	  isJumping(false),
	  spawnTimer(0),
	  spawnInterval(30),
	  pauseButtonClickedCallback(this, &GameScreenView::pauseButtonClicked),
	  continueButtonClickedCallback(this, &GameScreenView::continueButtonClicked),
	  tryAgainButtonClickedCallback(this, &GameScreenView::tryAgainButtonClicked),
	  quitButtonClickedCallback(this, &GameScreenView::quitButtonClicked),
	  isPaused(false),
	  ignoreNextClick(false),
	  gameOverTriggered(false),
	  previousPlayerY(140)
{
    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        obstacles[i].active = false;
    }

    for (int i = 0; i < MAX_PLATFORMS; i++)
    {
        platforms[i].active = false;
    }

    for (int i = 0; i < MAX_STEPS; i++)
    {
        steps[i].active = false;
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

    // ===== Step =====
    for (int i = 0; i < MAX_STEPS; i++)
    {
        steps[i].img.setBitmap(Bitmap(6)); // đổi ID ảnh platform
        steps[i].img.setVisible(false);
        steps[i].active = false;

        add(steps[i].img);
    }

    // ===== Platform =====
    for (int i = 0; i < MAX_PLATFORMS; i++)
    {
        platforms[i].img.setBitmap(Bitmap(12)); // đổi ID ảnh platform
        platforms[i].img.setVisible(false);
        platforms[i].active = false;

        add(platforms[i].img);
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

    // ===== Cuộn nền =====
    bg1.moveTo(bg1.getX() - SCROLL_SPEED, bg1.getY());
    bg2.moveTo(bg2.getX() - SCROLL_SPEED, bg2.getY());

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

    if (spawnTimer >= spawnInterval)
    {
        spawnStep();

        spawnTimer = 0;
    }

    // Di chuyển player ngang nếu đang đứng trên step
    if (isOnStep && !isJumping && currentStep != nullptr && currentStep->active) {
        // Di chuyển player sang trái cùng tốc độ với step
        player.moveTo(player.getX() - SCROLL_SPEED, player.getY());
    }

    // Cập nhật vị trí các step (sau khi đã di chuyển step)

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
            obstacles[i].active = false;
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

        if (steps[i].img.getX() < -(int16_t)steps[i].img.getWidth()) {
            steps[i].img.setVisible(false);
            steps[i].active = false;
            if (currentStep == &steps[i]) {
                currentStep = nullptr;
                isOnStep = false;
            }
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

void GameScreenView::spawnStep()
{
    for (int i = 0; i < MAX_STEPS; i++)
    {
        if (!steps[i].active)
        {
            uint8_t height = (rand() % 3) + 1;   // 1,2,3
            steps[i].height = height;
            steps[i].img.setBitmap(Bitmap(6));
            int16_t stepY = 140 - (height * 16);
            steps[i].img.setXY(320, stepY);

            steps[i].img.setVisible(true);
            steps[i].active = true;
            invalidate();
            break;
        }
    }
}


bool GameScreenView::checkStepCollision(Step& step) {
    int16_t pLeft = player.getX();
    int16_t pRight = player.getX() + player.getWidth();
    int16_t pTop = player.getY();
    int16_t pBottom = player.getY() + player.getHeight();

    int16_t sLeft = step.img.getX();
    int16_t sRight = step.img.getX() + step.img.getWidth();
    int16_t sTop = step.img.getY();
    int16_t sBottom = step.img.getY() + step.img.getHeight();

    int16_t overlapX = std::min(pRight, sRight) - std::max(pLeft, sLeft);
    int16_t overlapY = std::min(pBottom, sBottom) - std::max(pTop, sTop);

    if (overlapX <= 0 || overlapY <= 0) return false;

    // Va chạm bên hông → chết
    if (overlapX < overlapY) {
        onPlayerHit();
        return true;
    }

    // Va chạm dọc: từ trên xuống → đứng lên
    if (playerVelY >= 0 && pBottom <= sTop + 2) {
        player.moveTo(player.getX(), sTop - player.getHeight());
        playerVelY = 0;
        isJumping = false;
        currentStep = &step;      // lưu step đang đứng
        isOnStep = true;
        return true;
    }

    // Các trường hợp khác (từ dưới lên, …) – không xử lý (vì không xảy ra)
    return false;
}

void GameScreenView::updateSteps()
{
    for (int i = 0; i < MAX_STEPS; i++)
    {
        if (!steps[i].active) continue;

        // Di chuyển sang trái
        steps[i].img.moveTo(steps[i].img.getX() - SCROLL_SPEED,
                            steps[i].img.getY());

        // Kiểm tra va chạm với player
        if (!gameOverTriggered && checkStepCollision(steps[i]))
        {
            // Xử lý va chạm đã được thực hiện bên trong checkStepCollision
            // (có thể đã gọi onPlayerHit() hoặc đứng lên)
        }

        // Xóa step khi ra khỏi màn hình bên trái
        if (steps[i].img.getX() < -(int16_t)steps[i].img.getWidth())
        {
            steps[i].img.setVisible(false);
            steps[i].active = false;
        }
    }
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
            platforms[i].active = false;
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

void GameScreenView::onPlayerHit()
{
    if (gameOverTriggered) return;
    gameOverTriggered = true;

    // Bỏ dòng: isPaused = true;

    // Chuyển sang màn hình thua (nếu đã cấu hình)
    application().gotoFailedScreenScreenNoTransition();
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

    if (evt.getType() == ClickEvent::PRESSED && !isJumping) {
            playerVelY = JUMP_STRENGTH;
            isJumping = true;
            isOnStep = false;   // rời khỏi step
            currentStep = nullptr;
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
