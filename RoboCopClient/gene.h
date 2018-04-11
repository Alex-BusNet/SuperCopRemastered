#ifndef GENE_H
#define GENE_H

#include <QDebug>

class Gene
{
public:
    Gene();
    Gene(const Gene &g);

//private:
    int into;
    int out;
    float weight;
    bool enabled;
    int innovation;
};

#endif // GENE_H
