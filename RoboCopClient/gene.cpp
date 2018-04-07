#include "gene.h"

/**
 * @brief Gene default c'tor
 */
Gene::Gene()
{
    into = 0;
    out = 0;
    weight = 0.0f;
    enabled = true;
    innovation = 0;
}

/**
 * @brief Gene copy c'tor
 * @param g
 */
Gene::Gene(const Gene &g)
{
    this->into = g.into;
    this->out = g.out;
    this->weight = g.weight;
    this->enabled = g.enabled;
    this->innovation = g.innovation;
}
