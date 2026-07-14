#include <gui/gamescreen_screen/GameScreenView.hpp>
#include <touchgfx/Color.hpp>

GameScreenView::GameScreenView()
    : playerVelY(0), isJumping(false), spawnTimer(0), spawnInterval(90)
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
}

void GameScreenView::tearDownScreen()
{
    GameScreenViewBase::tearDownScreen();
}

void GameScreenView::handleTickEvent()
{
    GameScreenViewBase::handleTickEvent();

    // ===== 1. Cuộn nền =====
    bg1.moveTo(bg1.getX() - SCROLL_SPEED, bg1.getY());
    bg2.moveTo(bg2.getX() - SCROLL_SPEED, bg2.getY());

    if (bg1.getX() <= -BG_WIDTH) bg1.moveTo(bg2.getX() + BG_WIDTH, bg1.getY());
    if (bg2.getX() <= -BG_WIDTH) bg2.moveTo(bg1.getX() + BG_WIDTH, bg2.getY());

    // ===== 2. Vật lý nhân vật (nhảy/rơi) =====
    playerVelY += GRAVITY;
    int16_t newY = player.getY() + (int16_t)playerVelY;

    if (newY >= 140)
    {
        newY = 140;
        playerVelY = 0;
        isJumping = false;
    }
    player.moveTo(player.getX(), newY);

    // ===== 3. Spawn & update obstacle =====
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

        // Ra khỏi màn hình bên trái -> tắt, tái sử dụng slot
        if (obstacles[i].img.getX() < -(int16_t)obstacles[i].img.getWidth())
        {
            obstacles[i].img.setVisible(false);
            obstacles[i].active = false;
        }

        // Va chạm với player
        if (checkCollision(player, obstacles[i].img))
        {
            onPlayerHit();
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
    // TODO: chuyển sang màn RESULT_FAILED
    // Ví dụ: application().gotoResultFailedScreenNoTransition();
}

void GameScreenView::handleClickEvent(const touchgfx::ClickEvent& evt)
{
    GameScreenViewBase::handleClickEvent(evt);

    if (evt.getType() == ClickEvent::PRESSED && !isJumping)
    {
        playerVelY = JUMP_STRENGTH;
        isJumping = true;
    }
}
