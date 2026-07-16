// level.h
#ifndef LEVEL_H
#define LEVEL_H

#include <stdint.h>
#include <stdbool.h>

// Định nghĩa loại vật cản (khớp với file .gdl)
typedef enum {
    OBJ_STEP = 0,
    OBJ_SPIKE = 1,
    OBJ_PLATFORM = 2
} ObjectType;

// Cấu trúc một vật cản trong map (mỗi object 6 byte)
typedef struct {
    ObjectType type;
    uint8_t height;      // chiều cao (ô)
    uint16_t x;          // vị trí cột (ô)
    uint16_t length;     // số ô liên tiếp
} MapObject;

// Cấu trúc toàn bộ level
#define MAX_OBJECTS 100   // giới hạn số lượng vật cản

typedef struct {
    uint16_t mapLength;         // tổng số ô của map
    uint16_t objCount;          // số lượng vật cản thực tế
    MapObject objects[MAX_OBJECTS];
} Level;

// Hàm đọc file .gdl từ thẻ SD
// Trả về true nếu thành công, false nếu thất bại
bool LoadLevel(const char* filename, Level* level);

#endif // LEVEL_H