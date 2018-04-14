#include "pool.h"
#include <QFile>

Pool::Pool()
{
    generation = 0;
    innovation = RoboCop::Outputs;
    currentSpecies = 0;
    currentGenome = 0;
    currentFrame = 0;
    maxFitness = 0;
}

int Pool::NewInnovation()
{
//    qDebug() << "NewInnovation()";
    return ++this->innovation;
}

int Pool::TotalAverageFitness()
{
//    qDebug() << "TotalAverageFitness()";
    int total = 0;
    foreach(Species *s, species)
    {
        total += s->GetAverageFitness();
    }

    return total;
}

void Pool::RankGlobally()
{
//    qDebug() << "RankGlobally()";
    QVector<Genome*> globals;
    foreach(Species *s, species)
    {
        foreach(Genome *g, s->genomes)
        {
            globals.push_back(g);
        }
    }

    // Sort genomes from lowest fitness to highest fitness
    for(int i = 0; i < globals.size(); i++)
    {
        for(int j = i+1; j < globals.size(); j++)
        {
            Genome* a = globals[i];
            Genome* b = globals[j];

            if(a->GetFitness() > b->GetFitness())
            {
                Genome* temp = a;
                a = b;
                b = temp;
            }
        }
    }

    for(int i = 0; i < globals.size(); i++)
    {
        globals[i]->SetGlobalRank(i);
    }
}

void Pool::NextGenome()
{
//    qDebug() << "NextGenome()";
    currentGenome++;
    if(currentGenome > (species[currentSpecies]->genomes.size() - 1))
    {
        currentGenome = 0;
        currentSpecies++;
        if(currentSpecies > (species.size() - 1))
        {
            NewGeneration();
            currentSpecies = 0;
        }
    }

//    qDebug() << "\tGenome: " << currentGenome << " Species: " << currentSpecies;
}

void Pool::NewGeneration()
{
    qDebug() << "\tNewGeneration()";
    srand(time(0));
    CullSpecies(false);
    RankGlobally();
    RemoveStaleSpecies();
    RankGlobally();

    foreach(Species *s, species)
    {
        s->CalculateAverageFitness();
    }

    RemoveWeakSpecies();

    int sum = TotalAverageFitness();

    QVector<Genome*> children;
    foreach(Species *s, species)
    {
        int breed = std::floor(((float)s->GetAverageFitness() / (float)sum) * RoboCop::Population) - 1;
        for(int i = 0; i < breed; i++)
        {
            children.push_back(s->BreedChild(innovation));
        }
    }

    CullSpecies(true);
    while(children.size() + species.size() < RoboCop::Population)
    {
        Species *s = species[rand() % species.size()];
        children.push_back(s->BreedChild(innovation));
    }

    for(int c = 0; c < children.size(); c++)
    {
        AddToSpecies(children[c]);
    }

    generation++;
    SaveFile(QString("States/backup.%1.RC_1.json").arg(generation));
    qDebug() << "\t--Finished NewGeneration()";
}

QMap<QString, bool> Pool::EvaluateNetwork(QVector<int> inputs)
{
//    qDebug() << "Pool::EvaluateNetwork()";
//    qDebug() << "Species: " << species.size() << " CurrentSpecies: " << currentSpecies << " Current Genome: " << currentGenome;
    return species[currentSpecies]->genomes[currentGenome]->network.EvaluateNetwork(inputs);
}

void Pool::CullSpecies(bool cutToOne)
{
//    qDebug() << "CullSpecies()";
    foreach(Species *s, species)
    {
        if(s->genomes.size() > 0)
        {
            // Sort species from most fit to least fit
            for(int i = 0; i < s->genomes.size(); i++)
            {
                for(int j = i+1; j < s->genomes.size(); j++)
                {
                    Genome* a = s->genomes[i];
                    Genome* b = s->genomes[j];

                    if(a->GetFitness() < b->GetFitness())
                    {
                        Genome *temp = a;
                        a = b;
                        b = temp;
                    }
                }
            }

            int remaining = std::ceil((s->genomes.size()+1) / 2);
            if(cutToOne) { remaining = 1; }

            while((s->genomes.size()) > remaining)
            {
                s->genomes.removeLast();
            }
        }
    }
//    qDebug() << "--Finished CullSpecies()";
}

void Pool::RemoveStaleSpecies()
{
//    qDebug() << "RemoveStaleSpecies()";
    QVector<Species*> survived;
    foreach(Species *s, species)
    {
        if(s->genomes.size() > 0)
        {
            // Sort species from most fit to least fit
            for(int i = 0; i < s->genomes.size(); i++)
            {
                for(int j = i+1; j < s->genomes.size(); j++)
                {
                    Genome *a = s->genomes[i];
                    Genome *b = s->genomes[j];

                    if(a->GetFitness() < b->GetFitness())
                    {
                        Genome *temp = a;
                        a = b;
                        b = temp;
                    }
                }
            }

//            qDebug() << s->genomes[0]->GetFitness() << s->GetTopFitness() << maxFitness << s->GetStaleness() << RoboCop::StaleSpecies;

            if(s->genomes[0]->GetFitness() > s->GetTopFitness())
            {
                s->SetTopFitness(s->genomes[0]->GetFitness());
                s->SetStaleness(0);
            }
            else
                s->SetStaleness(s->GetStaleness() + 1);

            if((s->GetStaleness() < RoboCop::StaleSpecies) || (s->GetTopFitness() >= maxFitness))
            {
                survived.push_back(s);
            }
        }
    }

    qDebug() << "Survived size: " << survived.size();
    this->species = survived;

//    qDebug() << "--Finished RemoveStaleSpecies()";

}

void Pool::RemoveWeakSpecies()
{
//    qDebug() << "RemoveWeakSpecies()";

    QVector<Species*> survived;
    int sum = TotalAverageFitness();
    foreach(Species *s, species)
    {
        int breed = std::floor(((float)s->GetAverageFitness() / (float)sum) * RoboCop::Population);
        if(breed >= 1)
            survived.push_back(s);
    }

    this->species = survived;
//    qDebug() << "--Finished RemoveWeakSpecies()";
}

void Pool::SetCurrentFrame(int frame)
{
//    qDebug() << "SetCurrentFrame()";
    this->currentFrame = frame;
}

void Pool::SetMaxFitness(int mf)
{
//    qDebug() << "SetMaxFitness()";
    this->maxFitness = mf;
}

void Pool::SetCurrentSpecies(int cs)
{
//    qDebug() << "SetCurrentSpecies()";
    this->currentSpecies = cs;
}

void Pool::SetCurrentGenome(int gm)
{
//    qDebug() << "SetCurrentGenome()";
    this->currentGenome = gm;
}

bool Pool::FitnessAlreadyMeasured()
{
//    qDebug() << "FitnessAlreadyMeasured()";
    Species *s = species[currentSpecies];
    Genome *g = s->genomes[currentGenome];
    return g->GetFitness() != 0;
}

void Pool::AddToSpecies(Genome *child)
{
//    qDebug() << "AddToSpecies()";
    bool foundSpecies = false;
    foreach(Species *s, species)
    {
        if(!foundSpecies && s->genomes[0]->SameSpecies(*child, innovation-1))
        {
            s->genomes.push_back(child);
            foundSpecies = true;
        }
    }

    if(!foundSpecies)
    {
        Species *s = new Species();
        s->genomes.push_back(child);
        this->species.push_back(s);
    }
}

int Pool::GetCurrentGenome()
{
//    qDebug() << "GetCurrentGenome()";
    return this->currentGenome;
}

int Pool::GetInnovation()
{
//    qDebug() << "GetInnovation()";
    return this->innovation;
}

int Pool::GetGeneration()
{
//    qDebug() << "GetGeneration()";
    return this->generation;
}

int Pool::GetCurrentSpecies()
{
//    qDebug() << "GetCurrentSpecies()";
    return this->currentSpecies;
}

int Pool::GetCurrentFrame()
{
//    qDebug() << "GetCurrentFrame()";
    return this->currentFrame;
}

int Pool::GetMaxFitness()
{
//    qDebug() << "GetMaxFitness()";
    return this->maxFitness;
}

void Pool::SavePool(QJsonObject &obj)
{
    obj["generation"] = generation;
    obj["maxfitness"] = maxFitness;
    QJsonArray arr;

    foreach(Species *s, species)
    {
        QJsonObject sObj;
        s->SaveSpecies(sObj);
        arr.push_back(sObj);
    }

    obj["species"] = arr;
}

void Pool::LoadPool(QJsonObject &obj)
{
    generation = obj["generation"].toInt();
    maxFitness = obj["maxfitness"].toInt();
    QJsonArray arr = obj["species"].toArray();

    for(int i = 0; i < arr.size(); i++)
    {
        QJsonObject sObj = arr.at(i).toObject();
        Species *s = new Species();
        s->LoadSpecies(sObj);
        species.push_back(s);
    }
}

void Pool::SaveFile(QString filename)
{
    QFile saveState(filename);

    if(saveState.open(QIODevice::WriteOnly))
    {
        QJsonDocument doc;
        QJsonObject obj;

        SavePool(obj);

        doc.setObject(obj);
        saveState.write(doc.toJson());
        saveState.flush();
        saveState.close();
    }
}

void Pool::LoadFile(QString filename)
{

}
