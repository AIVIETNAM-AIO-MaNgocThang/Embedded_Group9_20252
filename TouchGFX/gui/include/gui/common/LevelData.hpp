#ifndef LEVELDATA_HPP
#define LEVELDATA_HPP

#include <stdint.h>

// ===== Loại chướng ngại vật =====
enum ObstacleType : uint8_t
{
    OBS_STEP     = 0,
    OBS_SPIKE    = 1,
    OBS_PLATFORM = 2
};

// ===== Dữ liệu 1 chướng ngại vật (đã parse) =====
struct ObstacleEntry
{
    ObstacleType type;
    uint8_t  height;    // số ô cao (chỉ ý nghĩa với Step)
    uint16_t x;         // vị trí ô trên Ox [0, N-1]
    uint16_t length;    // số ô liên tiếp
    int16_t  adjustY;   // điều chỉnh Y pixel (nâng lên khi chồng Step)
    bool     removed;   // đánh dấu bỏ sau resolveOverlaps
};

// ===== Giới hạn =====
static const uint16_t MAX_LEVEL_OBSTACLES = 128;

// ===== Dữ liệu level đã parse =====
struct LevelData
{
    uint8_t  version;
    uint16_t mapWidth;                              // N (số ô)
    uint16_t obstacleCount;                         // P (gốc)
    ObstacleEntry obstacles[MAX_LEVEL_OBSTACLES];   // đã parse + resolve
};

#endif // LEVELDATA_HPP