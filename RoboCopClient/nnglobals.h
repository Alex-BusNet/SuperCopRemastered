#ifndef NNGLOBALS_H
#define NNGLOBALS_H
#include <QString>
#include <random>

namespace RoboCop
{
    static const QString ButtonNames[] = {"RIGHT", "LEFT", "SPRINT", "JUMP", "NONE"};
    static const int BoxRadius = 35; //px
    static int InputSize = (18*10);
    static int Inputs = InputSize + 1;
    static int Outputs = 5;
    static int Population = 300;

    static const float DeltaDisjoint = 2.0f;
    static const float DeltaWeights = 0.4f;
    static const float DeltaThreshold = 1.0f;
    static const int StaleSpecies = 15;
    static const float MutationConnectionChance = 0.25f;
    static const float PerturbChance = 0.9f;
    static const float CrossoverChance = 0.75f;
    static const float LinkMutationChance = 2.0f;
    static const float BiasMutationChance = 0.4f;
    static const float NodeMutationChance = 0.5f;
    static const float StepSize = 0.1f;
    static const float DisableMutationChance = 0.4f;
    static const float EnableMutationChance = 0.2f;
    static const long MaxNodes = 1000000L;
    static const float TimeoutConstant = 60.0f; // frames

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0.0f, 1.0f);

    // Returns a random value between 0 and 1
    static float random() { return dis(gen); }
}

#endif // NNGLOBALS_H
