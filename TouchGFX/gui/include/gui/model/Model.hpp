#ifndef MODEL_HPP
#define MODEL_HPP

#include <stdint.h>

class ModelListener;

class Model
{
public:
    Model();

    void bind(ModelListener* listener)
    {
        modelListener = listener;
    }

    void tick();

    // Thông số game
    int16_t attemptCount;
    int16_t jumpCount;
    int16_t playTime;
    int16_t distance;

    void resetGameStats();
protected:
    ModelListener* modelListener;
};

#endif // MODEL_HPP
