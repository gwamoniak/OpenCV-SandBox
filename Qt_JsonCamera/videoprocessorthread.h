#ifndef VIDEOPROCESSORTHREAD_H
#define VIDEOPROCESSORTHREAD_H

#pragma once

#include <QObject>
#include <QThread>
#include <QPixmap>
#include <QProcess>
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/core/utility.hpp"

#include "jsonsfile.h"
#include "json_camerasettings.h"

class VideoProcessorThread : public QThread
{


    Q_OBJECT
public:
    explicit VideoProcessorThread(QObject *parent = nullptr);
    ~VideoProcessorThread() = default;
    bool LinuxCameraSetup();
    bool LoadCameraSettings();
    bool SaveCameraSettings();
    bool UpdateCameraSettings();
    void GetDefaultSettings();

    void setResize(const bool &Resize){m_bResize = Resize;}
    bool getResize()const {return m_bResize;}
    bool passJSON(const QJsonObject &json);

    void setTakenImage(const bool &TakeImage){m_bTakeImage = TakeImage;}
    void setSide2Side(const bool &Side2Side){m_bSide2Side = Side2Side;}

    bool getTakenImage() const{return m_bTakeImage ;}
    bool getSide2Side()  const{return m_bSide2Side ;}

    void setPictureNumber(const int &PictureNumber){m_nPictureNumber = PictureNumber;}
    int getPictureNumber() const {return m_nPictureNumber;}
    void setSlideShow(const bool &SlideS){m_bSlideS = SlideS;}
    bool getSlideShow() const {return m_bSlideS;}

   typedef std::vector<cv::Mat_<cv::Vec4b>> MatVector;
   MatVector getTakenPictures() const {return m_vPictures;}


signals:
    void inDisplay(QPixmap pixmap);
    void inPicture(QPixmap pixmap);
    void inSlideShow_2(QPixmap pixmap);
public slots:

private:
    void run() override;
    void TakePhoto(const cv::Mat_<cv::Vec4b> &image);

    QString m_sFormatInfo;  // "Format Info"
    int m_nWidth, m_nHeight; // "Resolution Width" "Resolution Height"
    double m_dExposure, m_dGain;// "Camera Exposure" "Camera Gain"
    bool m_bResize;
    bool m_bTakeImage;
    bool m_bSide2Side;
    int m_nPictureNumber;
    bool m_bSlideS; // flag for slide show
    MatVector m_vPictures; // container for pictures

    JsonCameraSettings m_CameraSettings;
    JsonFile m_JsonFile;
    JsonCameraSettings::Resolution m_eResolution;
    cv::VideoCapture m_camera;

};

#endif // VIDEOPROCESSORTHREAD_H
