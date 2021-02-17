#include "jsonCamerasettings.h"
#include <QFile>
#include <QMessageBox>

JsonCameraSettings::JsonCameraSettings(QObject*)
{
   // default values
    Reset2DefaultValues();

}

JsonCameraSettings::~JsonCameraSettings()
{
   m_vmJson_map.clear();

}

bool JsonCameraSettings::read(const QJsonObject &json)
{
    bool bStatus = true;

    if(!json.isEmpty())
    {
        m_vmJson_map = json.toVariantMap();
    }
    else
    {
        bStatus = false;
    }

    if(!m_vmJson_map.isEmpty())
    {
        QVariantMap captureInfo = m_vmJson_map["Capture Info"].toMap();
        m_sFormatInfo  = captureInfo["Image format"].toString();
        m_nWidth       = captureInfo["Resolution Width"].toInt();
        m_nHeight      = captureInfo["Resolution Height"].toInt();
        m_dGain        = captureInfo["Camera Gain"].toDouble();
        m_dExposure    = captureInfo["Camera Exposure"].toDouble();

        if(setResolution2Enum())
        {
            bStatus = true;
        }
        else
        {
            bStatus = false;
        }

    }
    else
    {
        bStatus = false;
    }

    return bStatus;

}


bool JsonCameraSettings::write(QJsonObject &json) const
{
   bool bStatus = true;

   QJsonObject json_obj2; // temp value

   json_obj2["Image format"]      = m_sFormatInfo;
   json_obj2["Resolution Width"]  = m_nWidth;
   json_obj2["Resolution Height"] = m_nHeight;
   json_obj2["Camera Exposure"]   = m_dExposure;
   json_obj2["Camera Gain"]       = m_dGain;
   json["Capture Info"]           = json_obj2;


   if(json_obj2.size() != 5) {
       bStatus = false;
}

   return bStatus;

}

bool JsonCameraSettings::setResolution2Enum()
{
    bool bStatus = false;
    if(m_nWidth != 0 && m_nHeight != 0)
    {
        bStatus = true;

        if(m_nWidth == 1024 && m_nHeight == 768)
        {
            m_eResolution = XGA;
        }
        else if(m_nWidth == 1280 && m_nHeight == 720)
        {
            m_eResolution = HD;
        }
        else if(m_nWidth == 1920 && m_nHeight == 1080)
        {
            m_eResolution = FullHD;
        }
        else
        {
            m_eResolution = ERROR;
            bStatus = false;
            qDebug() << "Resolution not specified";
            QMessageBox msgBox;
            msgBox.setText("Resolution not specified.");
            msgBox.exec();
        }

    }
    else
    {
        bStatus = false;
    }
    return bStatus;
}

bool JsonCameraSettings::setEnum2Resolution()
{
    bool bStatus = true;
    if(m_eResolution != ERROR)
    {
        bStatus = true;

        if(m_eResolution == XGA)
        {
            m_nWidth  = 1024;
            m_nHeight = 768;
        }
        else if(m_eResolution == HD)
        {
            m_nWidth  = 1280 ;
            m_nHeight = 720;
        }
        else if(m_eResolution == FullHD)
        {
            m_nWidth  = 1920;
            m_nHeight = 1080;
        }
        else
        {
            bStatus = false;
            qDebug() << "Resolution Error";
        }

    }
    else if(m_eResolution == ERROR)
    {
        //set to default
        m_nWidth = 1024;
        m_nHeight = 768;
        m_eResolution = XGA;
        bStatus = true;
    }
    else
    {
        bStatus = false;
    }
    return bStatus;
}

bool JsonCameraSettings::IsUpdated()
{
    bool bStatus = false;
    if(m_bStatus == true)
    {
        bStatus = true;
    }

    return bStatus;
}

void JsonCameraSettings::print()
{
        qDebug() << m_sFormatInfo;
        qDebug() << m_nWidth;
        qDebug() << m_nHeight;
        qDebug() << m_dExposure;
        qDebug() << m_dGain;
        qDebug() << QtEnumToString(m_eResolution);
}




