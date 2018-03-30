#ifndef GENE_H
#define GENE_H


class Gene
{
public:
    Gene();

private:
    int into;
    int out;
    float weight;
    bool enabled;
    int innovation;
};

#endif // GENE_H
