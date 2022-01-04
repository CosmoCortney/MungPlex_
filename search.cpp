#include "search.h"

using namespace std;

Search::Search(Hook* hook)
{
    this->hook = hook;
}

void Search::cancel()
{
    iterations = 0;
    firstSearchDone(true);
    freeDumps();
}

void Search::writeFileToDisk(char* data, QIODevice* file, long long size)
{
    if(!file->open(QIODevice::WriteOnly))
    { qDebug() << "Error: cannot write file"; }

    QDataStream writeFile(file);
    writeFile.setByteOrder(QDataStream::LittleEndian);
    writeFile.setVersion(QDataStream::Qt_5_12);
    qDebug() << QString::number(writeFile.writeRawData(data, size), 16);
    file->close();
}

void Search::readFileFromDisk(QIODevice* file, char* map, long long size)
{
    if(!file->open(QIODevice::ReadOnly)) { qDebug() << "read error"; }
    QDataStream readFile(file);
    readFile.setByteOrder(QDataStream::LittleEndian);
    readFile.readRawData(map, size);

    file->close();
}
