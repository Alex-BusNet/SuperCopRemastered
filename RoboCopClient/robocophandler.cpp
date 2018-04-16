#include "robocophandler.h"
#include <QDebug>
#include <QDir>

Q_GLOBAL_STATIC(RoboCopHandler, rch)

RoboCopHandler::RoboCopHandler()
{
    filepath = "States";
    frameUpdate = false;
    gameRunning = false;
    reset = false;
    playerPosX = 0;
    rightmost = 0;

    srand(time(0));

    if(!QDir(filepath).exists())
        QDir().mkdir(filepath);

    for(int i = 0; i < RoboCop::Outputs; i++)
    {
        if(!controls.contains(RoboCop::ButtonNames[i]))
            controls.insert(RoboCop::ButtonNames[i], false);
        else
            controls[RoboCop::ButtonNames[i]] = false;
    }
}

/*
 * Copy c'tor
 */
RoboCopHandler::RoboCopHandler(const RoboCopHandler &rch) : QThread()
{
    pool = rch.pool;
    inputs = rch.inputs;
    controls = rch.controls;
    timeout = rch.timeout;
    playerPosX = rch.playerPosX;
    rightmost = rch.rightmost;
    filepath = rch.filepath;
    playerPosX = 0;
    rightmost = 0;
    frameUpdate = false;
    gameRunning = false;
    reset = false;
}

RoboCopHandler::~RoboCopHandler()
{

}

RoboCopHandler *RoboCopHandler::instance()
{
    return rch;
}

void RoboCopHandler::GameLoop()
{
    while(gameRunning)
    {
        if(frameUpdate)
        {
            frameUpdate = false;

            Species *s = pool->species[pool->GetCurrentSpecies()];
            Genome *g = s->genomes[pool->GetCurrentGenome()];

            if(pool->GetCurrentFrame() % 5 == 0)
                EvaluateCurrent();

            SetControls(controls);

            if (playerPosX > rightmost)
            {
                rightmost = playerPosX;
                qDebug() << "Player greater than rightmost";
                timeout = RoboCop::TimeoutConstant;
            }

            timeout--;

            float timeoutBonus = 0;// (float)pool->GetCurrentFrame() / 4.0f;

            emit resetStat(reset);

            if((timeout + timeoutBonus <= 0) || reset)
            {
                if(reset) { reset = false; }

                float fitness = (float)rightmost - (float)pool->GetCurrentFrame() / 2.0f;

                if(rightmost > (70 * 205)) // Need to check what rightmost max should be
                    fitness += 1000;

                if(fitness == 0)
                    fitness = -1;

                g->SetFitness(fitness);

                if(fitness > pool->GetMaxFitness())
                {
                    pool->SetMaxFitness(fitness);
//                    SaveFile(filepath + "/backup." + pool->GetGeneration() + ".RC_1.json");
                }

                pool->SetCurrentSpecies(0);
                pool->SetCurrentGenome(0);

                while(pool->FitnessAlreadyMeasured())
                    pool->NextGenome();

                InitializeRun(false);
            }

            //--------------------------
            // NN State info goes here

            emit SpeciesUpdate(pool->GetCurrentSpecies(), pool->species.size());
            emit GenomeUpdate(pool->GetCurrentGenome());
            emit GenerationUpdate(pool->GetGeneration());
            emit FitnessUpdate(std::floor((float)rightmost - (float)pool->GetCurrentFrame() / 2.0f - (timeout + timeoutBonus) * 2.0f / 3.0f));
            emit MaxFitnessUpdate(std::floor(pool->GetMaxFitness()));
            emit timeoutval(timeout + timeoutBonus);

            //--------------------------

            pool->SetCurrentFrame(pool->GetCurrentFrame() + 1);
        }

    }
}

void RoboCopHandler::run()
{
    InitializePool();
    gameRunning = true;
    GameLoop();
}

int RoboCopHandler::end(int retcode)
{
    gameRunning = false;
    this->SaveFile(QString("RC_1_Gen%1.json").arg(pool->GetGeneration()));
    this->exit(retcode);
    return 0;
}

void RoboCopHandler::ClearControls()
{
//    qDebug() << "ClearControls()";
    for(int i = 0; i < RoboCop::Outputs; i++)
    {
        controls[RoboCop::ButtonNames[i]] = false;
    }

    SetControls(controls);
}

void RoboCopHandler::InitializeRun(bool playerDied)
{
    qDebug() << "InitializeRun()";
    pool->SetCurrentFrame(0);
    rightmost = 0;
    timeout = RoboCop::TimeoutConstant;

    if(!playerDied)
        emit NewSpecies(); // Send reset command back to RoboCop

    ClearControls();

    pool->species[pool->GetCurrentSpecies()]->genomes[pool->GetCurrentGenome()]->GenerateNetwork();
    EvaluateCurrent();
    qDebug() << "--Finished InitializeRun()";
}

void RoboCopHandler::InitializePool()
{
//    qDebug() << "InitializePool()";
    pool = new Pool();
    timeout = 0;
    playerPosX = 0;

    for(int i = 0; i < RoboCop::Population; i++)
    {
        Genome *b = Genome::BasicGenome();
        pool->AddToSpecies(b);
    }

    InitializeRun(false);

    //    qDebug() << "--Finished InitializePool()";
}

void RoboCopHandler::LevelReset()
{
    reset = true;
}

void RoboCopHandler::SetInputs(int **in)
{
//    qDebug() << "SetInputs()";
    for(int y = 0; y < 10; y++)
    {
        for(int x = 0; x < 18; x++)
        {
            if(inputs.size() <= ((y * 18) + x))
                inputs.push_back(in[y][x]);
            else
                inputs.replace((y * 18) + x, in[y][x]);
        }
    }
//    qDebug() << "--Finished SetInputs()";
}

void RoboCopHandler::SetPosition(int x)
{
//    qDebug() << "SetPositions()";
    this->playerPosX = x;
}

void RoboCopHandler::EvaluateCurrent()
{
//    qDebug() << "EvaluateCurrent()";
    controls = pool->EvaluateNetwork(inputs);

    if(controls["LEFT"] && controls["RIGHT"])
    {
        controls["LEFT"] = controls["RIGHT"] = false;
    }

    SetControls(controls);
}

void RoboCopHandler::SetControls(QMap<QString, bool> cState)
{
//    qDebug() << "SetControls()";
    controls = cState;
    uint8_t keyPressState = 0b0000;

    if(cState["JUMP"])
        keyPressState |= 0b0100;

    if(cState["RIGHT"])
        keyPressState |= 0b0010;

    if(cState["LEFT"])
        keyPressState |= 0b0001;

    if(cState["SPRINT"])
        keyPressState |= 0b1000;

    emit keyStateUpdate(keyPressState);
}

Pool* RoboCopHandler::GetPool()
{
//    qDebug() << "GetPool()";
    return pool;
}

QVector<int> RoboCopHandler::GetInputs()
{
//    qDebug() << "GetInputs()";
    return inputs;
}

int RoboCopHandler::GetPlayerPosX()
{
//    qDebug() << "GetPlayerPosX()";
    return playerPosX;
}

int RoboCopHandler::GetRightMost()
{
//    qDebug() << "GetRightMost()";
    return rightmost;
}

int RoboCopHandler::GetTimeout()
{
//    qDebug() << "GetTimeout()";
    return timeout;
}

QMap<QString, bool> RoboCopHandler::GetControls()
{
//    qDebug() << "GetControls()";
    return controls;
}

void RoboCopHandler::SaveFile(QString filename)
{
    QFile saveState(filename);

    if(saveState.open(QIODevice::WriteOnly))
    {
        QJsonDocument doc;
        QJsonObject obj;

        pool->SavePool(obj);

        doc.setObject(obj);

        saveState.write(doc.toJson());

        saveState.flush();
        saveState.close();
    }
}

void RoboCopHandler::LoadFile(QString filename)
{
    QFile saveState(filename);

    if(saveState.exists())
    {
        if(saveState.open(QIODevice::ReadOnly))
        {
            QJsonDocument doc = QJsonDocument::fromJson(saveState.readAll());
            QJsonObject obj = doc.object();
            pool->LoadPool(obj);
        }
    }

    while(pool->FitnessAlreadyMeasured())
    {
        pool->NextGenome();
    }

    InitializeRun(false);
    pool->SetCurrentFrame(pool->GetCurrentFrame() + 1);
}

void RoboCopHandler::FrameUpdated()
{
    frameUpdate = true;
}

