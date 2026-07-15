#include <gui/gamescreen_screen/GameScreenView.hpp>
#include <touchgfx/Color.hpp>

GameScreenView::GameScreenView()
    : playerVelY(0),
      isJumping(false),
      pauseButtonClickedCallback(this, &GameScreenView::pauseButtonClicked),
      continueButtonClickedCallback(this, &GameScreenView::continueButtonClicked),
      tryAgainButtonClickedCallback(this, &GameScreenView::tryAgainButtonClicked),
      quitButtonClickedCallback(this, &GameScreenView::quitButtonClicked),
      isPaused(false),
      ignoreNextClick(false),
      gameOverTriggered(false),
      previousPlayerY(GROUND_Y),
      objectCount(0)
{
    for (int i = 0; i < MAX_GAME_OBJECTS; i++) {
        gameObjects[i].active = false;
        gameObjects[i].img.setVisible(false);
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
    player.setBitmap(Bitmap(12));
    player.setXY(96, GROUND_Y);
    add(player);

    // --- Khởi tạo các object (chưa có dữ liệu, sẽ được gán sau) ---
    for (int i = 0; i < MAX_GAME_OBJECTS; i++) {
        gameObjects[i].img.setVisible(false);
        add(gameObjects[i].img);
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

// Hàm khởi tạo từ dữ liệu level (gọi trước khi vào game)
void GameScreenView::initFromLevel(const Level& level)
{
    // Xóa các object cũ
    for (int i = 0; i < MAX_GAME_OBJECTS; i++) {
        gameObjects[i].active = false;
        gameObjects[i].img.setVisible(false);
    }

    objectCount = (level.objCount < MAX_GAME_OBJECTS) ? level.objCount : MAX_GAME_OBJECTS;

    for (int i = 0; i < objectCount; i++) {
        const MapObject& src = level.objects[i];
        GameObject& dst = gameObjects[i];

        dst.type = src.type;
        // Quy đổi từ toạ độ ô sang pixel (giả sử mỗi ô = 32px)
        dst.x = src.x * 32;
        dst.y = GROUND_Y - (src.height * 32); // đáy object nằm trên mặt đất
        dst.width = src.length * 32;
        dst.height = src.height * 32;

        // Chọn bitmap theo loại
        switch (src.type) {
            case OBJ_STEP:
                dst.img.setBitmap(Bitmap(10)); // ID bitmap của step (thay số thực tế)
                break;
            case OBJ_SPIKE:
                dst.img.setBitmap(Bitmap(9));  // ID bitmap của spike (giữ nguyên)
                break;
            case OBJ_PLATFORM:
                dst.img.setBitmap(Bitmap(11)); // ID bitmap của platform
                break;
            default:
                continue;
        }

        dst.img.setXY(dst.x, dst.y);
        dst.img.setVisible(true);
        dst.active = true;
    }
    invalidate();
}

void GameScreenView::handleTickEvent()
{
    GameScreenViewBase::handleTickEvent();

    if (isPaused) return;

    // ---- Cuộn nền ----
    bg1.moveTo(bg1.getX() - SCROLL_SPEED, bg1.getY());
    bg2.moveTo(bg2.getX() - SCROLL_SPEED, bg2.getY());
    if (bg1.getX() <= -BG_WIDTH) bg1.moveTo(bg2.getX() + BG_WIDTH, bg1.getY());
    if (bg2.getX() <= -BG_WIDTH) bg2.moveTo(bg1.getX() + BG_WIDTH, bg2.getY());

    // ---- Vật lý nhân vật ----
    previousPlayerY = player.getY();

    playerVelY += GRAVITY;
    int16_t newY = player.getY() + (int16_t)playerVelY;

    // Mặt đất
    if (newY >= GROUND_Y) {
        newY = GROUND_Y;
        playerVelY = 0;
        isJumping = false;
    }
    player.moveTo(player.getX(), newY);

    // ---- Cập nhật tất cả chướng ngại ----
    for (int i = 0; i < objectCount; i++) {
        GameObject& obj = gameObjects[i];
        if (!obj.active) continue;

        // Di chuyển sang trái
        obj.x -= SCROLL_SPEED;
        obj.img.moveTo(obj.x, obj.y);

        // Xóa khi ra khỏi màn hình bên trái
        if (obj.x + obj.width < 0) {
            obj.active = false;
            obj.img.setVisible(false);
            continue;
        }

        // Xử lý va chạm theo loại
        if (!gameOverTriggered) {
            switch (obj.type) {
                case OBJ_SPIKE:
                    if (checkCollision(player, obj.img)) {
                        onPlayerHit();
                        return;
                    }
                    break;
                case OBJ_STEP:
                    handleStepCollision(obj);
                    break;
                case OBJ_PLATFORM:
                    handlePlatformCollision(obj);
                    break;
            }
        }
    }
}

// ----- Va chạm AABB đơn giản -----
bool GameScreenView::checkCollision(const Image& a, const Image& b)
{
    Rect ra(a.getX(), a.getY(), a.getWidth(), a.getHeight());
    Rect rb(b.getX(), b.getY(), b.getWidth(), b.getHeight());
    return ra.intersect(rb);
}

// ----- Xử lý step (khối đặc, cản 4 hướng) -----
void GameScreenView::handleStepCollision(GameObject& obj)
{
    Rect playerRect(player.getX(), player.getY(), player.getWidth(), player.getHeight());
    Rect stepRect(obj.x, obj.y, obj.width, obj.height);

    if (!playerRect.intersect(stepRect)) return;

    // Tính độ chồng lấn theo từng cạnh
    int overlapLeft   = (player.getX() + player.getWidth()) - obj.x;
    int overlapRight  = (obj.x + obj.width) - player.getX();
    int overlapTop    = (player.getY() + player.getHeight()) - obj.y;
    int overlapBottom = (obj.y + obj.height) - player.getY();

    // Tìm độ chồng nhỏ nhất để xác định hướng đẩy
    int minOverlap = overlapLeft;
    int direction = 0; // 0: left, 1: right, 2: top, 3: bottom
    if (overlapRight < minOverlap) { minOverlap = overlapRight; direction = 1; }
    if (overlapTop < minOverlap)   { minOverlap = overlapTop;   direction = 2; }
    if (overlapBottom < minOverlap){ minOverlap = overlapBottom; direction = 3; }

    // Đẩy player ra khỏi step
    switch (direction) {
        case 0: // va chạm từ bên phải -> đẩy sang trái
            player.moveTo(obj.x - player.getWidth(), player.getY());
            break;
        case 1: // va chạm từ bên trái -> đẩy sang phải
            player.moveTo(obj.x + obj.width, player.getY());
            break;
        case 2: // va chạm từ trên xuống (đứng trên step)
            player.moveTo(player.getX(), obj.y - player.getHeight());
            playerVelY = 0;
            isJumping = false;
            break;
        case 3: // va chạm từ dưới lên (đập đầu)
            player.moveTo(player.getX(), obj.y + obj.height);
            playerVelY = 0;
            break;
    }
}

// ----- Xử lý platform (chỉ tương tác dọc, đi xuyên ngang) -----
void GameScreenView::handlePlatformCollision(GameObject& obj)
{
    // Tương tự logic checkPlatformLanding cũ nhưng dùng obj
    int16_t playerLeft = player.getX();
    int16_t playerRight = player.getX() + player.getWidth();
    int16_t platformLeft = obj.x;
    int16_t platformRight = obj.x + obj.width;

    // Kiểm tra overlap ngang
    if (!(playerRight > platformLeft && playerLeft < platformRight)) return;

    int16_t prevTop = previousPlayerY;
    int16_t prevBottom = previousPlayerY + player.getHeight();
    int16_t curTop = player.getY();
    int16_t curBottom = player.getY() + player.getHeight();
    int16_t platTop = obj.y;
    int16_t platBottom = obj.y + obj.height;

    // Đang rơi xuống -> đáp lên platform
    if (playerVelY > 0) {
        if (prevBottom <= platTop && curBottom >= platTop) {
            player.moveTo(player.getX(), platTop - player.getHeight());
            playerVelY = 0;
            isJumping = false;
        }
    }
    // Đang nhảy lên -> đụng đầu vào platform
    else if (playerVelY < 0) {
        if (prevTop >= platBottom && curTop <= platBottom) {
            player.moveTo(player.getX(), platBottom);
            playerVelY = 0;
        }
    }
}

// ----- Khi player chạm spike -----
void GameScreenView::onPlayerHit()
{
    if (gameOverTriggered) return;
    gameOverTriggered = true;
    isPaused = true;
    application().gotoFailedScreenScreenNoTransition();
}

// ----- Xử lý click để nhảy -----
void GameScreenView::handleClickEvent(const touchgfx::ClickEvent& evt)
{
    GameScreenViewBase::handleClickEvent(evt);

    if (isPaused) return;

    if (ignoreNextClick) {
        ignoreNextClick = false;
        return;
    }

    if (evt.getType() == ClickEvent::PRESSED && !isJumping) {
        playerVelY = JUMP_STRENGTH;
        isJumping = true;
    }
}

// ----- Các callback button -----
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
