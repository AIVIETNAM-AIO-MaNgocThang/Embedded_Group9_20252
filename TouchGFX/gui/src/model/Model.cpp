#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

Model::Model()
    : modelListener(0),
      attemptCount(1),
      jumpCount(0),
      playTime(0),
      distance(0)
{

}

void Model::tick()
{

}

void Model::resetGameStats()
{
    jumpCount = 0;
    playTime = 0;
    distance = 0;
}
