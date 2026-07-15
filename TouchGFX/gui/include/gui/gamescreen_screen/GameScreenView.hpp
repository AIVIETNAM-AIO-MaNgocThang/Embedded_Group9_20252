#ifndef GAMESCREENVIEW_HPP
#define GAMESCREENVIEW_HPP

#include <gui_generated/gamescreen_screen/GameScreenViewBase.hpp>
#include <gui/gamescreen_screen/GameScreenPresenter.hpp>
#include <touchgfx/Callback.hpp>

// Định nghĩa loại chướng ngại (phải khớp với bên map)
enum ObjectType {
    OBJ_STEP = 0,
    OBJ_SPIKE = 1,
    OBJ_PLATFORM = 2
};

// Cấu trúc một đối tượng trong map (dùng chung)
struct MapObject {
    ObjectType type;
    uint8_t height;
    uint16_t x;
    uint16_t length;
};

// Cấu trúc toàn bộ level
struct Level {
    uint16_t mapLength;
    uint16_t objCount;
    MapObject objects[100]; // tối đa 100 object
};

class GameScreenView : public GameScreenViewBase
{
public:
    GameScreenView();
    virtual ~GameScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();

    // Khởi tạo game với dữ liệu level
    void initFromLevel(const Level& level);

protected:
    // ---- Màn hình 320x240 ----
    static const int16_t SCREEN_WIDTH  = 320;
    static const int16_t SCREEN_HEIGHT = 240;

    // ---- Nền cuộn ----
    Image bg1;
    Image bg2;
    static const int16_t BG_WIDTH    = 1600;
    static const int16_t SCROLL_SPEED = 2;   // px/frame

    // ---- Nhân vật ----
    Image player;
    static const int16_t PLAYER_SIZE = 16;
    static const int16_t GROUND_Y    = SCREEN_HEIGHT - PLAYER_SIZE - 20;
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

    // ---- Quản lý chướng ngại tổng hợp (thay thế obstacles & platforms) ----
    static const int MAX_GAME_OBJECTS = 50;

    struct GameObject {
        Image img;
        ObjectType type;
        bool active;
        int16_t x, y;
        uint16_t width, height;
    };

    GameObject gameObjects[MAX_GAME_OBJECTS];
    int objectCount; // số object thực tế trong level

    // ---- Xử lý va chạm ----
    bool checkCollision(const Image& a, const Image& b);
    void handleStepCollision(GameObject& obj);
    void handlePlatformCollision(GameObject& obj);
    void onPlayerHit();

    // ---- Dữ liệu platform để kiểm tra đáp ----
    int16_t previousPlayerY;

public:
    virtual void handleClickEvent(const touchgfx::ClickEvent& evt);
};

#endif // GAMESCREENVIEW_HPP
