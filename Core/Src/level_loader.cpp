// level_loader.cpp
#include "level.h"
#include "ff.h"          // FATFS
#include <string.h>

bool LoadLevel(const char* filename, Level* level) {
    FIL file;
    FRESULT res;

    // Mở file
    res = f_open(&file, filename, FA_READ);
    if (res != FR_OK) {
        return false;
    }

    UINT br;

    // 1. Đọc magic number (4 byte)
    uint32_t magic;
    f_read(&file, &magic, 4, &br);
    if (br != 4 || magic != 0x47444C45) {  // "GDLE"
        f_close(&file);
        return false;
    }

    // 2. Đọc version (1 byte)
    uint8_t version;
    f_read(&file, &version, 1, &br);
    if (br != 1 || version != 1) {
        f_close(&file);
        return false;
    }

    // 3. Đọc mapLength (2 byte, little-endian)
    f_read(&file, &level->mapLength, 2, &br);
    if (br != 2) {
        f_close(&file);
        return false;
    }

    // 4. Đọc objCount (2 byte)
    f_read(&file, &level->objCount, 2, &br);
    if (br != 2) {
        f_close(&file);
        return false;
    }

    // Kiểm tra giới hạn
    if (level->objCount > MAX_OBJECTS) {
        f_close(&file);
        return false;
    }

    // 5. Đọc từng object (mỗi object 6 byte)
    for (uint16_t i = 0; i < level->objCount; i++) {
        uint8_t type_u8;
        uint8_t height;
        uint16_t x;
        uint16_t length;

        f_read(&file, &type_u8, 1, &br);
        f_read(&file, &height, 1, &br);
        f_read(&file, &x, 2, &br);
        f_read(&file, &length, 2, &br);

        if (br != 6) {
            f_close(&file);
            return false;
        }

        level->objects[i].type   = (ObjectType)type_u8;
        level->objects[i].height = height;
        level->objects[i].x      = x;
        level->objects[i].length = length;
    }

    f_close(&file);
    return true;
}