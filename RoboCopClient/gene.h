#ifndef GENE_H
#define GENE_H

#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

class Gene
{
public:
    Gene();
    Gene(const Gene &g);

    void SaveGene(QJsonObject &obj);
    void LoadGene(const QJsonObject &obj);

//private:
    int into;
    int out;
    float weight;
    bool enabled;
    int innovation;
};

#endif // GENE_H
