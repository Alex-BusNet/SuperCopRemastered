#include "genome.h"

Genome::Genome()
{
    fitness = 0;
    adjustedFitness = 0;
    maxNeuron = 0;
    globalRank = 0;
    mutationRates.BiasMutationChance = RoboCop::BiasMutationChance;
    mutationRates.DisableMutationChance = RoboCop::DisableMutationChance;
    mutationRates.EnableMutationChance = RoboCop::EnableMutationChance;
    mutationRates.LinkMutationChance = RoboCop::LinkMutationChance;
    mutationRates.MutateConnectionsChance = RoboCop::MutationConnectionChance;
    mutationRates.NodeMutationChance = RoboCop::NodeMutationChance;
    mutationRates.StepSize = RoboCop::StepSize;
}
