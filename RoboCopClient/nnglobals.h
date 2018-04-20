#ifndef NNGLOBALS_H
#define NNGLOBALS_H
#include <QString>
#include <QDebug>
#include <random>

namespace RoboCop
{
    static const QString ButtonNames[] = {"JUMP", "SPRINT", "RIGHT", "LEFT", "NONE"};
    static const int BoxRadius = 10;                         //px, used for NN rendering
    static int InputSize = (18*10);
    static int Inputs = InputSize + 1;
    static int Population = 300;
    static int Outputs = 5;                                 // Use only RIGHT, LEFT, SPRINT, and JUMP. For some reason
                                                            // if Outputs = 4, only the first 3 controls (index 0, 1, & 2) are
                                                            // are used. If Outputs = 5, then the first 4 controls (index 0, 1, 2, & 3)
                                                            // are used.

    static const float DeltaDisjoint = 2.0f;                // Disjoint modifier value
    static const float DeltaWeights = 0.4f;                 // Weight modifier value
    static const float DeltaThreshold = 1.0f;               // Threshold to determine if two species are the same
    static const int StaleSpecies = 15;                     // Max number of counts before a species is killed off
    static const float MutationConnectionChance = 0.25f;    // Chance to alter weight of gene
    static const float PerturbChance = 0.9f;                // Chance to alter gene away from its normal path
    static const float CrossoverChance = 0.75f;             //
    static const float LinkMutationChance = 2.0f;           // Chance to add a new node between a random input and output node
    static const float BiasMutationChance = 0.4f;           // Chance to add a new biasing node to an output node.
    static const float NodeMutationChance = 0.5f;           //
    static const float StepSize = 0.1f;                     // Used to alter weight of node if pertrubing
    static const float DisableMutationChance = 0.4f;        // Chance to disable a node
    static const float EnableMutationChance = 0.2f;         // Chance to enable a node
    static const long MaxNodes = 100000L;                   // Max number of nodes to create. WARNING: DO NOT ENTER A VALUE OVER 100000. System may slow down.
    static const float TimeoutConstant = 60.0f; // frames


    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0.0f, 1.0f);
    static std::uniform_int_distribution<> disi(0, RAND_MAX);

    // Returns a random value between 0 and 1
    static float randomf() { float r = dis(gen); /*qDebug() << "randomf() : " << r;*/ return r; }

    // Returns the average of two random values
    // chosen between 0 and RAND_MAX
    // using the mersenne twister.
    static int randomi() { int i = (disi(gen) + disi(gen)) / 2; /*qDebug() << "randomi() : " << i;*/ return i; }
}

#endif // NNGLOBALS_H
