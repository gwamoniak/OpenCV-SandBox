#ifndef JSONSERIALIZABLE_H
#define JSONSERIALIZABLE_H
#pragma once
#include "jsonsfile.h"
#include <QJsonObject>

class JsonSerialize
{
public:

    virtual ~JsonSerialize() = default;
    virtual bool read(const QJsonObject &jsonObj) = 0;
    virtual bool write(QJsonObject &jsonObj) const = 0;

};

#endif // JSONSERIALIZABLE_H
