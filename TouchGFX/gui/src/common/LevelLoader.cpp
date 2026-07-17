#include <gui/common/LevelLoader.hpp>

bool LevelLoader::load(const uint8_t* data, uint32_t size, LevelData& out)
{
    // --- Kiểm tra kích thước tối thiểu ---
    if (size < 9) return false;

    // --- Magic "GDLE" = 0x47 0x44 0x4C 0x45 ---
    if (data[0] != 0x47 || data[1] != 0x44 ||
        data[2] != 0x4C || data[3] != 0x45)
        return false;

    // --- Version ---
    out.version = data[4];
    if (out.version != 1) return false;

    // --- N, P ---
    out.mapWidth       = readU16LE(&data[5]);
    out.obstacleCount  = readU16LE(&data[7]);

    // --- Kiểm tra dung lượng ---
    uint32_t expectedSize = 9 + (uint32_t)out.obstacleCount * 6;
    if (size < expectedSize) return false;
    if (out.obstacleCount > MAX_LEVEL_OBSTACLES) return false;

    // --- Parse từng CNV ---
    const uint8_t* p = &data[9];
    for (uint16_t i = 0; i < out.obstacleCount; i++)
    {
        ObstacleEntry& e = out.obstacles[i];
        e.type    = (ObstacleType)p[0];
        e.height  = p[1];
        e.x       = readU16LE(&p[2]);
        e.length  = readU16LE(&p[4]);
        e.adjustY = 0;
        e.removed = false;
        p += 6;
    }

    // --- Post-process ---
    resolveOverlaps(out);

    return true;
}

uint16_t LevelLoader::readU16LE(const uint8_t* p)
{
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

void LevelLoader::resolveOverlaps(LevelData& level)
{
    // ========== BƯỚC A: EXPAND ==========
    // Biến entry length>1 thành nhiều entry length=1
    ObstacleEntry expanded[MAX_LEVEL_OBSTACLES];
    uint16_t expandedCount = 0;

    for (uint16_t i = 0; i < level.obstacleCount; i++)
    {
        ObstacleEntry& e = level.obstacles[i];
        for (uint16_t k = 0; k < e.length; k++)
        {
            if (expandedCount >= MAX_LEVEL_OBSTACLES) break;
            expanded[expandedCount]        = e;
            expanded[expandedCount].x      = e.x + k;
            expanded[expandedCount].length  = 1;
            expanded[expandedCount].adjustY = 0;
            expanded[expandedCount].removed = false;
            expandedCount++;
        }
    }

    // Copy expanded vào level
    for (uint16_t i = 0; i < expandedCount; i++)
        level.obstacles[i] = expanded[i];
    level.obstacleCount = expandedCount;

    // ========== BƯỚC B: SẮP XẾP THEO X ==========
    // Insertion sort (đơn giản, P nhỏ)
    for (uint16_t i = 1; i < level.obstacleCount; i++)
    {
        ObstacleEntry key = level.obstacles[i];
        int16_t j = i - 1;
        while (j >= 0 && level.obstacles[j].x > key.x)
        {
            level.obstacles[j + 1] = level.obstacles[j];
            j--;
        }
        level.obstacles[j + 1] = key;
    }

    // ========== BƯỚC C: RESOLVE TỪNG Ô ==========
    uint16_t i = 0;
    while (i < level.obstacleCount)
    {
        uint16_t x = level.obstacles[i].x;

        // Tìm nhóm entry cùng ô x
        uint16_t groupStart = i;
        while (i < level.obstacleCount && level.obstacles[i].x == x)
            i++;
        uint16_t groupEnd = i; // exclusive

        // Trong nhóm [groupStart, groupEnd):
        // --- Cùng loại ---
        int16_t bestStepIdx = -1;
        uint8_t bestStepH   = 0;
        bool    hasPlatform  = false;
        int16_t firstPlatIdx = -1;
        bool    hasSpike     = false;
        int16_t firstSpikeIdx = -1;

        for (uint16_t k = groupStart; k < groupEnd; k++)
        {
            ObstacleEntry& e = level.obstacles[k];
            switch (e.type)
            {
                case OBS_STEP:
                    if (e.height > bestStepH)
                    {
                        if (bestStepIdx >= 0)
                            level.obstacles[bestStepIdx].removed = true;
                        bestStepIdx = k;
                        bestStepH   = e.height;
                    }
                    else
                    {
                        e.removed = true; // step thấp hơn → bỏ
                    }
                    break;

                case OBS_SPIKE:
                    if (hasSpike)
                        e.removed = true; // spike trùng → bỏ
                    else
                    {
                        hasSpike     = true;
                        firstSpikeIdx = k;
                    }
                    break;

                case OBS_PLATFORM:
                    if (hasPlatform)
                        e.removed = true; // platform trùng → bỏ
                    else
                    {
                        hasPlatform  = true;
                        firstPlatIdx = k;
                    }
                    break;
            }
        }

        // --- Khác loại ---

        // Platform + Spike → bỏ Spike
        if (hasPlatform && hasSpike)
        {
            level.obstacles[firstSpikeIdx].removed = true;
            hasSpike = false;
        }

        // Step + Spike (còn lại) → nâng Spike
        if (bestStepIdx >= 0 && hasSpike)
        {
            level.obstacles[firstSpikeIdx].adjustY =
                -(int16_t)(bestStepH * 16);
        }

        // Step + Platform → nâng Platform
        if (bestStepIdx >= 0 && hasPlatform)
        {
            level.obstacles[firstPlatIdx].adjustY =
                -(int16_t)(bestStepH * 16);
        }
    }
}
