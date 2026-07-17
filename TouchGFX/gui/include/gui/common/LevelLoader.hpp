#ifndef LEVELLOADER_HPP
#define LEVELLOADER_HPP

#include <gui/common/LevelData.hpp>

class LevelLoader
{
public:
    /// Parse mảng nhị phân → LevelData. Trả về true nếu thành công.
    static bool load(const uint8_t* data, uint32_t size, LevelData& out);

private:
    static uint16_t readU16LE(const uint8_t* p);

    /// Xử lý chồng chéo cùng toạ độ
    static void resolveOverlaps(LevelData& level);
};

#endif // LEVELLOADER_HPP