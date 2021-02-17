#ifndef JSON_CAMERASETTINGS_H
#define JSON_CAMERASETTINGS_H
#pragma once

#include "jsonserializable.h"

#include <QObject>
#include <QString>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QVariantMap>
#include <QDebug>
#include <QMetaEnum>


// print the enum as QString
template<typename QEnum>
QString QtEnumToString (const QEnum value)
{
  return QString(QMetaEnum::fromType<QEnum>().valueToKey(value));
}



class JsonCameraSettings :public QObject, JsonSerialize
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap Json_map READ getJson_map WRITE setJson_map)
    Q_PROPERTY(QString FormatInfo READ getFormatInfo WRITE setFormatInfo)
    Q_PROPERTY(int Width READ getWidth WRITE setWidth)
    Q_PROPERTY(int Height READ getHeight WRITE setHeight)
    Q_PROPERTY(double Exposure READ getExposure WRITE setExposure)
    Q_PROPERTY(double Gain READ getGain WRITE setGain)

public:
    enum Resolution // resolution
    {
        XGA     = 1, //1024 x 768
        HD      = 2, //1280 x 720
        FullHD  = 3,  //1920 x 1080
        ERROR   = 4 // not specified

    };

    Q_ENUM(Resolution)



private:
        QString m_sFormatInfo;  // "Format Info"
        int m_nWidth, m_nHeight; // "Resolution Width" "Resolution Height"
        double m_dExposure, m_dGain;// "Camera Exposure" "Camera Gain"
        QVariantMap m_vmJson_map;
        Resolution m_eResolution;
        bool m_bStatus;



public:
    explicit JsonCameraSettings(QObject* parent = nullptr);
     ~JsonCameraSettings();

    bool read(const QJsonObject &jsonObj) override;
    bool write(QJsonObject &jsonObj) const override;

    void print();

    void setJson_map(const QVariantMap &Json_map){m_vmJson_map = Json_map;}
    QVariantMap getJson_map()const {return m_vmJson_map;}

    void setFormatInfo(const QString &FormatInfo){m_sFormatInfo= FormatInfo;}
    QString getFormatInfo()const {return m_sFormatInfo;}

    void setWidth(const int &Width){m_nWidth = Width;}
    int getWidth()const {return m_nWidth;}

    void setHeight(const int &Height){m_nHeight = Height;}
    int getHeight()const {return m_nHeight;}

    void setExposure(const double &Exposure){m_dExposure = Exposure;}
    double getExposure()const{return m_dExposure;}

    void setGain(const double &Gain){m_dGain = Gain;}
    double getGain() const {return m_dGain;}

    void setResolution(Resolution resolution){m_eResolution = resolution;}
    Resolution getResolution() const {return m_eResolution;}

    bool setResolution2Enum();
    bool setEnum2Resolution();
    bool SettingsUpdated(){  m_bStatus = true;
                             return m_bStatus; }
    bool IsUpdated();





    void Reset2DefaultValues(){    m_vmJson_map = {};
                                   m_sFormatInfo = "SGRBG8";
                                   m_nWidth      = 1024;
                                   m_nHeight     = 760;
                                   m_dGain       = 0.00090;
                                   m_dExposure   = 0.00185;
                                   m_eResolution = XGA;
                                   m_bStatus = false;}

};


#endif // JSON_CAMERASETTINGS_H



