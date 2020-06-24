#ifndef SETTINGS_H
#define SETTINGS_H

#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonValue>
#include<QString>
#include<QDataStream>
#include<QIODevice>
#include<QFile>
#include<QDir>
#include<QDebug>


//todo: make this a singleton
namespace MungPlexSettings
{
static QJsonDocument settings;
static QJsonObject settingsObject;

//reads settings from disk
static void loadSettings()
{
    QString content;
    QFile file(QDir::currentPath().append("/settings.json"));
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) { qDebug() << "file not found"; return; }
    QTextStream in (&file);

    content = in.readAll();
    settings = QJsonDocument::fromJson(content.toUtf8());
    settingsObject = settings.object();
    file.close();
    qDebug() << "settings loaded";
}

//updates settings object
template<typename dType> static void updateSettings(char* key, dType& val)
{
    settingsObject[key] = val;
    settings.setObject(settingsObject);
}

//writes settings to disk
static void saveSettings()
{
    QFile file(QDir::currentPath().append("/settings.json"));
    if(!file.open(QFile::WriteOnly | QFile::Text)){ qDebug() << "could not save settings"; return;  }
    QTextStream out (&file);
    out << settings.toJson();
    file.flush();
    file.close();
}

static void printWorkingDir()
{
    qDebug() << QDir::currentPath();
}


}

#endif // SETTINGS_H
