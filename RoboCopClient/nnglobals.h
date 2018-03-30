#ifndef NNGLOBALS_H
#define NNGLOBALS_H
#include <QString>

namespace RoboCop
{
    static QString ButtonNames[] = {"RIGHT", "LEFT", "SPRINT", "LEFT", "UP", "NONE"};
    static int BoxRadius = 35; //px
    static int Inputs = 18 * 10;
    static int InputSize = (18*10)+1;
    static int Outputs = 6;
    static int Population;

    static float DeltaDisjoint = 2.0f;
    static float DeltaWeights = 0.4f;
    static float DeltaThreshold = 1.0f;
    static int StaleSpecies = 15;
    static float MutationConnectionChance = 0.25f;
    static float PerturbChance = 0.9f;
    static float CrossoverChance = 0.75f;
    static float LinkMutationChance = 2.0f;
    static float BiasMutationChance = 0.4f;
    static float NodeMutationChance = 0.5f;
    static float StepSize = 0.1f;
    static float DisableMutationChance = 0.4f;
    static float EnableMutationChance = 0.2f;
    static long MaxNodes = 1000000L;
    static int TimeoutConstant = 20; // seconds? milliseconds?

}

#endif // NNGLOBALS_H
