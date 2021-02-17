#ifndef JSONFILE_H
#define JSONFILE_H

#pragma once
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

class JsonFile
{
public:
    JsonFile() = default;
   ~JsonFile() = default;
    void save(const QJsonObject &json, const QString &filepath) ;
    void load(QJsonObject &json, const QString &filepath) ;
};

#endif // JSONFILE_H
