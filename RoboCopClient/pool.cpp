#include "pool.h"

Pool::Pool()
{
    generation = 0;
    innovation = RoboCop::Outputs;
    currentSpecies = 0;
    currentGenome = 0;
    currentFrame = 0;
    maxFitness = 0;
}
