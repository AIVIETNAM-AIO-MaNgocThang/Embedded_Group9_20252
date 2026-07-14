#ifndef GAMESCREENVIEW_HPP
#define GAMESCREENVIEW_HPP

#include <gui_generated/gamescreen_screen/GameScreenViewBase.hpp>
#include <gui/gamescreen_screen/GameScreenPresenter.hpp>

class GameScreenView : public GameScreenViewBase
{
public:
    GameScreenView();
    virtual ~GameScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();

protected:
    // ---- Màn hình 320x240 ----
    static const int16_t SCREEN_WIDTH  = 320;
    static const int16_t SCREEN_HEIGHT = 240;

    // ---- Nền cuộn (ảnh 1600x240) ----
    Image bg1;
    Image bg2;
    static const int16_t BG_WIDTH    = 1600;
    static const int16_t SCROLL_SPEED = 2;   // px/frame, tăng = chạy nhanh hơn

    // ---- Nhân vật ----
    Image player;
    static const int16_t PLAYER_SIZE = 16;
    static const int16_t GROUND_Y    = SCREEN_HEIGHT - PLAYER_SIZE - 20; // 20 = mặt đất cao 20px, chỉnh theo ý bạn
    float playerVelY;
    bool  isJumping;
    static constexpr float GRAVITY      = 0.6f;
    static constexpr float JUMP_STRENGTH = -8.0f;

    // ---- Vật cản ----
    static const int MAX_OBSTACLES = 5;
    struct Obstacle
    {
        Image img;
        bool  active;
    };
    Obstacle obstacles[MAX_OBSTACLES];
    int spawnTimer;
    int spawnInterval; // số tick giữa 2 lần spawn

    void spawnObstacle();
    void updateObstacles();
    bool checkCollision(const Image& a, const Image& b);
    void onPlayerHit();

public:
    virtual void handleClickEvent(const touchgfx::ClickEvent& evt); // để nhảy khi chạm màn hình
};

#endif // GAMESCREENVIEW_HPP
