#include "videoprocessorthread.h"
#include <QtMultimedia/QCameraInfo>
#include <QPainter>
#include <ctime>
#include <time.h>



VideoProcessorThread::VideoProcessorThread(QObject *parent) : QThread(parent)
{
     m_eResolution = JsonCameraSettings::ERROR;
     m_vPictures.clear();
     m_bTakeImage = false;
     m_bSide2Side = false;
     m_nPictureNumber = 0;
}

constexpr double clockToMilliseconds(clock_t ticks){
    // units/(units/time) => time (seconds) * 1000 = milliseconds
    return (ticks/(double)CLOCKS_PER_SEC)*1000.0;
}


void VideoProcessorThread::run()
{
    using namespace cv;

    Size resize(800,800);
    Size size400(400,400), temp(1024,768);
    Mat_<cv::Vec4b> inFrame;


         struct tm* timeinfo;

        const QCameraInfo cameraInfo;
        QString camera_name = cameraInfo.defaultCamera().deviceName();
        qDebug() << camera_name;

            bool bStatus = false;
            int cameraIDX = camera_name.mid(camera_name.length()-1,1).toInt(&bStatus); //extract camera ID from string

            qDebug() << cameraIDX;


            if(!camera_name.isEmpty()){

             bStatus = true;
             m_camera.open(cameraIDX,cv::CAP_ANY);


            // populate settings variables
             m_dExposure  = m_CameraSettings.getExposure();
             m_dGain      = m_CameraSettings.getGain();
             m_nWidth     = m_CameraSettings.getWidth();
             m_nHeight    = m_CameraSettings.getHeight();

             //qDebug() << "Gain: " <<  static_cast<double>(m_camera.get(CAP_PROP_GAIN ));
             //qDebug() << "Exposure: " <<  m_camera.get(CAP_PROP_EXPOSURE);

            // set the camera
             m_camera.set(CAP_PROP_EXPOSURE, m_dExposure);
             m_camera.set(CAP_PROP_GAIN, m_dGain );
             m_camera.set(CAP_PROP_FRAME_WIDTH, m_nWidth);
             m_camera.set(CAP_PROP_FRAME_HEIGHT,m_nHeight);


             qDebug() <<  m_nWidth;
             qDebug() <<  m_nHeight;
             qDebug() <<  m_CameraSettings.getWidth();
             qDebug() <<  m_CameraSettings.getHeight();


            }
            else
            {
                qDebug() << "Camera is not conected";
                bStatus = false;

            }

    if(bStatus)
    {
        double frameCounter = 0.0, scale = 1.0;
        cv::String valueAsString = "FPS: ";
        CascadeClassifier faceCascade;
        faceCascade.load("haarcascade_frontalface_alt2.xml"); // your OpenCV haarcascade

//        clock_t deltaTime = 0;
//        unsigned int frames = 0;
//        double  frameRate = 30;
//        double  averageFrameTimeMilliseconds = 33.333;

        std::array<char, 64> datebuffer; // container for date and time
        datebuffer.fill(0);

        cv::Mat cameraMatrix, distCoeffs;
        cv::Mat grayscale;
        std::vector<Rect> faces;
        std::vector<cv::Vec3d> rvecs, tvecs;
        std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;


        try
        {

            while(m_camera.isOpened() && !isInterruptionRequested())
            {

                int64 start = cv::getTickCount();
                clock_t beginFrame = clock();
                cv::Point dateCoordinate( m_nWidth - 250,m_nHeight - 25);

                m_camera >> inFrame;
                if(inFrame.empty())
                    continue;

                time_t now = time(0);
                timeinfo = localtime(&now);
                strftime(datebuffer.data(),sizeof(datebuffer),"%d-%m-%Y %I:%M:%S",timeinfo);

                cv::putText(inFrame,
                            datebuffer.data(),
                            dateCoordinate, // Coordinates
                            cv::FONT_HERSHEY_COMPLEX_SMALL, // Font
                            1.0, // Scale. 2.0 = 2x bigger
                            cv::Scalar(0,255,0), // Color
                            1, // Thickness
                            cv::LINE_AA);

                //take picture
                if(m_bTakeImage)
                {
                    //qDebug() << "take picture";
                    cv::Mat_<cv::Vec4b> image(inFrame);
                    TakePhoto(image);
                }



                if(m_bSide2Side)
                {
                    if(!m_vPictures.empty())
                    {
                        cv::resize(inFrame,inFrame,size400,0,0,INTER_LANCZOS4);
                        QImage picture(m_vPictures.back().data,m_vPictures.back().cols,m_vPictures.back().rows,m_vPictures.back().step,QImage::Format_RGB888);
                        emit inPicture(QPixmap::fromImage(picture.rgbSwapped()));

                    }
                    else
                    {
                        qDebug() << "Take a picture first";
                    }
                }

                frameCounter = cv::getTickFrequency() / (cv::getTickCount() - start);

                cv::putText(inFrame,
                            valueAsString+QString::number(frameCounter).toStdString(),
                            cv::Point(25,35), // Coordinates
                            cv::FONT_HERSHEY_COMPLEX_SMALL, // Font
                            1.0, // Scale. 2.0 = 2x bigger
                            cv::Scalar(0,255,0), // Color
                            1, // Thickness
                            cv::LINE_AA);

                //detect face


                cv::cvtColor(inFrame,grayscale,COLOR_BGR2GRAY);
                cv::resize(grayscale,grayscale,cv::Size(grayscale.size().width/scale,grayscale.size().height/scale));
                faceCascade.detectMultiScale(grayscale,faces,1.1,3,0,cv::Size(30,30));

                for(Rect area: faces)
                {
                    cv::Scalar drawColor = Scalar(255,0,0);
                    cv::rectangle(inFrame,cv::Point(cvRound(area.x*scale),cvRound(area.y*scale)),
                                  cv::Point(cvRound((area.x +area.width -1)*scale),cvRound((area.y +area.height -1)*scale)),drawColor);
                }


                QImage image(inFrame.data,inFrame.cols,inFrame.rows,inFrame.step,QImage::Format_RGB888);
                emit inDisplay(QPixmap::fromImage(image.rgbSwapped()));


            }

        }
        catch (const std::exception& e)
        {
            qDebug() << "CameraProcess error: " << e.what() <<'\n';
        }





        m_camera.release();
    }
}

void VideoProcessorThread::TakePhoto(const cv::Mat_<cv::Vec4b> &image)
{
    m_vPictures.push_back(image); // can save pictures latter
    m_bTakeImage = false;
    QImage qimage(image.data,image.cols,image.rows,image.step,QImage::Format_RGB888);


    QString imagePath = "example.jpg";
    qimage = qimage.rgbSwapped();
    qimage.save(imagePath);

}

bool VideoProcessorThread::LoadCameraSettings()
{
    bool bStatus = true;

    const QString filepath = "camera_settings.json";
    QJsonObject json;

    m_JsonFile.load(json,filepath);
    if(m_CameraSettings.read(json))
    {
        qDebug() << "Reading camera settings successful";
        bStatus = true;
    }
    else
    {
        qDebug() << "Reading camera settings unsuccessful";
        bStatus = false;
    }

    return bStatus;

}

bool VideoProcessorThread::SaveCameraSettings()
{
    bool bStatus = true;

    const QString filepath = "camera_settings.json";
    QJsonObject json;
    qDebug() << m_dExposure;
    qDebug() << m_dGain;
    qDebug() << m_nWidth;
    qDebug() << m_nHeight;
    qDebug() <<  m_CameraSettings.getExposure();

    if(!m_CameraSettings.IsUpdated())
        GetDefaultSettings();

    if(m_CameraSettings.write(json))
    {
         m_JsonFile.save(json,filepath);
         qDebug() << "Saving camera settings successful";
         bStatus = true;
    }
    else
    {
        qDebug() << "Saving camera settings unsuccessful";
        bStatus = false;

    }

    return bStatus;

}

bool VideoProcessorThread::UpdateCameraSettings()
{
    bool bStatus = true;
    m_dExposure = m_CameraSettings.getExposure();
    m_dGain     = m_CameraSettings.getGain();
    m_nWidth    = m_CameraSettings.getWidth();
    m_nHeight   = m_CameraSettings.getHeight();

    m_CameraSettings.print();

    return bStatus;
}

void VideoProcessorThread::GetDefaultSettings()
{
  m_sFormatInfo = "SGRBG8";
  m_nWidth      = 1024;
  m_nHeight     = 768;
  m_dGain       = 0.00090;
  m_dExposure   = 0.00185;
}

bool VideoProcessorThread::passJSON(const QJsonObject &json)
{

        bool bStatus = true;
        if(m_CameraSettings.read(json))
        {
            qDebug() << "Camera settings has been passed successfuly";
            bStatus = true;
        }
        else
        {
            qDebug() << "Camera settings has not been passed";
            bStatus = false;
        }
        return bStatus;
}

bool VideoProcessorThread::LinuxCameraSetup()
{
      bool bStatus = true;

      QProcess *terminal_cmd = new QProcess;

      m_eResolution = m_CameraSettings.getResolution();

       // linux emmbeded setup
      switch(m_eResolution)
      {
      case JsonCameraSettings::XGA:
                   qDebug() << "Setting XGA";
                   terminal_cmd->start("sh",QStringList()<<"-c"<<"media-ctl -V ''2:'0[fmt:'SGRBG8'/'1024x768'('288,258')/'2048x1536']'");
                   terminal_cmd->waitForStarted();
                   terminal_cmd->startDetached("sh",QStringList()<<"-c"<<"media-ctl -V '\"ipu0-csi0-sd\":0[fmt:'SGRBG8'/'1024x768']'");
                   terminal_cmd->startDetached("sh",QStringList()<<"-c"<<"media-ctl -V '\"ipu0-csi0-sd\":1[fmt:'SGRBG8'/'1024x768']'");
              break;
      case JsonCameraSettings::HD:
             qDebug() << "Setting HD";
             terminal_cmd->start("sh",QStringList()<<"-c"<<"media-ctl -V ''2:'0[fmt:'SGRBG8'/'1280x720'('32,306')/'2560x1440']'");
             terminal_cmd->waitForStarted();
             terminal_cmd->startDetached("sh",QStringList()<<"-c"<<"media-ctl -V '\"ipu0-csi0-sd\":0[fmt:'SGRBG8'/'1280x720']'");
             terminal_cmd->startDetached("sh",QStringList()<<"-c"<<"media-ctl -V '\"ipu0-csi0-sd\":1[fmt:'SGRBG8'/'1280x720']'");
        break;
      case JsonCameraSettings::FullHD:
            qDebug() << "Setting FullHD";
             terminal_cmd->start("sh",QStringList()<<"-c"<<"media-ctl -V ''2:'0[fmt:'SGRBG8'/'1920x1080'('352,486')/'1920x1080']'");
             terminal_cmd->waitForStarted();
             terminal_cmd->startDetached("sh",QStringList()<<"-c"<<"media-ctl -V '\"ipu0-csi0-sd\":0[fmt:'SGRBG8'/'1920x1080']'");
             terminal_cmd->startDetached("sh",QStringList()<<"-c"<<"media-ctl -V '\"ipu0-csi0-sd\":1[fmt:'SGRBG8'/'1920x1080']'");
        break;
      case JsonCameraSettings::ERROR:
             qDebug() << "Resolution unsupported or not specified, reseting to default";
             terminal_cmd->start("sh",QStringList()<<"-c"<<"media-ctl -V ''2:'0[fmt:'SGRBG8'/'1024x768'('288,258')/'2048x1536']'");
             terminal_cmd->waitForStarted();
             terminal_cmd->startDetached("sh",QStringList()<<"-c"<<"media-ctl -V '\"ipu0-csi0-sd\":0[fmt:'SGRBG8'/'1024x768']'");
             terminal_cmd->startDetached("sh",QStringList()<<"-c"<<"media-ctl -V '\"ipu0-csi0-sd\":1[fmt:'SGRBG8'/'1024x768']'");
        break;
       }


    if (!terminal_cmd->waitForFinished())
        {
            qDebug() << "Process error: " << terminal_cmd->errorString();
            bStatus = false;
        }
    else
      {
            terminal_cmd->terminate();
            //qDebug() << "Process terminated ";
            bStatus = true;
      }

    delete terminal_cmd;

    return bStatus;
}



