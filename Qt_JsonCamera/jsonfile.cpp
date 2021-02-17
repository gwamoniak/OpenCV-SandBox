#include "jsonsfile.h"
#include <QFile>



void JsonFile::load(QJsonObject &json,const QString &filepath)
{
    if(!filepath.isEmpty())
    {

        QFile file(filepath);
        if(!file.open(QIODevice::ReadOnly))
        {
            qDebug() << "error opening/loading file: " << file.error();
            return;
        }

        QJsonDocument json_doc;
        json_doc = QJsonDocument::fromJson(file.readAll());
        file.close();
        json = json_doc.object();
    }
    else
    {
        qDebug() << "file path is not correct" << filepath;
        return;
    }
}

void JsonFile::save(const QJsonObject &json,const QString &filepath)
{
    if(!filepath.isEmpty())
    {
        QFile file(filepath);
        if(!file.open(QFile::WriteOnly))
        {
            qDebug() << "error opening/saving file: " << file.error();
            return;
        }
        QJsonDocument json_doc(json);
        file.write(json_doc.toJson());
        file.close();
    }
    else
    {
        qDebug() << "file path is not correct" << filepath;
        return;
    }
}







