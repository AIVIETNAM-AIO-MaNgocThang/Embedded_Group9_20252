#ifndef GAMESCREENVIEW_HPP
#define GAMESCREENVIEW_HPP

#include <gui_generated/gamescreen_screen/GameScreenViewBase.hpp>
#include <gui/gamescreen_screen/GameScreenPresenter.hpp>
#include <touchgfx/Callback.hpp>
#include <gui/common/LevelData.hpp>

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
    static const int16_t GROUND_Y    = 140;
    float playerVelY;
    bool  isJumping;
    static constexpr float GRAVITY      = 0.6f;
    static constexpr float JUMP_STRENGTH = -8.0f;

    // ---- Pause ----
    bool isPaused;
    touchgfx::Callback<GameScreenView, const touchgfx::AbstractButtonContainer&> pauseButtonClickedCallback;
    void pauseButtonClicked(const touchgfx::AbstractButtonContainer& src);

    // ---- Continue ----
    bool ignoreNextClick;
    touchgfx::Callback<GameScreenView, const touchgfx::AbstractButtonContainer&> continueButtonClickedCallback;
    void continueButtonClicked(const touchgfx::AbstractButtonContainer& src);

    // ---- Try Again ----
    touchgfx::Callback<GameScreenView, const touchgfx::AbstractButtonContainer&> tryAgainButtonClickedCallback;
	void tryAgainButtonClicked(const touchgfx::AbstractButtonContainer& src);
	void resetGame();

	// ---- Quit ----
	touchgfx::Callback<GameScreenView, const touchgfx::AbstractButtonContainer&> quitButtonClickedCallback;
	void quitButtonClicked(const touchgfx::AbstractButtonContainer& src);

    // ---- Game Over guard ----
    bool gameOverTriggered;
    // ---- Vật cản ----
    static const int MAX_OBSTACLES = 25;
    struct Obstacle
    {
        Image img;
        bool  active;
    };
    Obstacle obstacles[MAX_OBSTACLES];

    void spawnObstacle(int16_t screenX, int16_t screenY);
    void updateObstacles();
    bool checkCollision(const Image& a, const Image& b);
    void onPlayerHit();
    void onLevelClear();

    // Trong GameScreenView.hpp
    static const int MAX_STEPS = 25;
    static const int MAX_STEP_HEIGHT = 4; // số ô tối đa chồng lên nhau

    struct Step
    {
        Image blocks[MAX_STEP_HEIGHT]; // mỗi ô 16x16, vẽ chồng lên nhau
        uint8_t height;                // số ô thực tế (1..MAX_STEP_HEIGHT)
        bool active;
    };

    Step steps[MAX_STEPS];

    void spawnStep(int16_t screenX, uint8_t h);
    bool checkStepCollision(Step& step);
    void updateSteps();

    // ---- Platform ----
    static const int MAX_PLATFORMS = 25;
    struct Platform
    {
        touchgfx::Image img;
        bool active;
    };

    Platform platforms[MAX_PLATFORMS];

    // ---- Level ----
    LevelData levelData;
    int32_t   cameraPixel;     // tổng px đã cuộn (bắt đầu từ 0)
    uint16_t  nextSpawnIndex;  // con trỏ duyệt mảng obstacles[]

    static const int16_t PLAYER_START_CELL = -15;  // ô bắt đầu

    // Platform mà player đang đứng
    Platform* currentPlatform;

    void spawnPlatform(int16_t screenX, int16_t screenY);
    void updatePlatforms();
    bool checkPlatformLanding(Platform& platform);
    int16_t previousPlayerY;

    // Đếm tick
    int16_t tickCounter;
public:
    virtual void handleClickEvent(const touchgfx::ClickEvent& evt); // để nhảy khi chạm màn hình
};

#endif // GAMESCREENVIEW_HPP
