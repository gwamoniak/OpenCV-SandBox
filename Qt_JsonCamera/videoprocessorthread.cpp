#include "videoprocessorthread.h"
#include <QtMultimedia/QCameraInfo>
#include <QPainter>
#include <ctime>
#include <time.h>


VideoProcessorThread::VideoProcessorThread(QObject *parent) : QThread(parent)
{
     m_eResolution = JsonCameraSettings::ERROR;
     m_vPictures.clear();
     m_bResize = false;
     m_bTakeImage = false;
     m_bSide2Side = false;
     m_bSlideS = false;
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
    Size size400(400,400), temp(640,480), resizeHVGA(480,320);
    Mat_<cv::Vec4b> inFrame;



    struct tm* timeinfo;

    const QCameraInfo cameraInfo;
    QString camera_name = cameraInfo.defaultCamera().deviceName();

            bool bStatus;
            int cameraIDX = camera_name.mid(camera_name.length()-1,1).toInt(&bStatus); //extract camera ID from string

            if(bStatus){

            #ifdef PLATFORM_MIRA
                m_camera.open(cameraIDX+CV_CAP_GSTREAMER_QUEUE_LENGTH);
            #elif PLATFORM_DESKTOP
                m_camera.open(cameraIDX);
            #endif

            // populate settings variables
             m_dExposure  = m_CameraSettings.getExposure();
             m_dGain      = m_CameraSettings.getGain();
             m_nWidth     = m_CameraSettings.getWidth();
             m_nHeight    = m_CameraSettings.getHeight();

             qDebug() << "Gain: " <<  static_cast<double>(m_camera.get(CV_CAP_PROP_GAIN ));
             qDebug() << "Exposure: " <<  m_camera.get(CV_CAP_PROP_EXPOSURE);

            // set the camera
             m_camera.set(CV_CAP_PROP_EXPOSURE, m_dExposure);
             m_camera.set(CV_CAP_PROP_GAIN, m_dGain );
             m_camera.set(CV_CAP_PROP_FRAME_WIDTH, m_nWidth);
             m_camera.set(CV_CAP_PROP_FRAME_HEIGHT,m_nHeight);


             qDebug() <<  m_nWidth;
             qDebug() <<  m_nHeight;
             qDebug() <<  m_CameraSettings.getWidth();
             qDebug() <<  m_CameraSettings.getHeight();


            }

    double frameCounter = 0.0;
    cv::String valueAsString = "FPS: ";

    clock_t deltaTime = 0;
    unsigned int frames = 0;
    double  frameRate = 30;
    double  averageFrameTimeMilliseconds = 33.333;

    std::array<char, 64> datebuffer; // container for date and time
    datebuffer.fill(0);



    while(m_camera.isOpened() && !isInterruptionRequested())
    {

        int64 start = cv::getTickCount();
        clock_t beginFrame = clock();
        cv::Point dateCoordinate( m_nWidth - 260,m_nHeight - 25);

        m_camera >> inFrame;
        if(inFrame.empty())
            continue;

          time_t now = time(0);
          timeinfo = localtime(&now);
          strftime(datebuffer.data(),sizeof(datebuffer),"%d-%m-%Y %I:%M:%S",timeinfo);

       if(m_bResize == true && m_bSide2Side == false)
        {
           if(m_nWidth >= 400 && m_nHeight >= 400)
           {
             Rect roi((m_nWidth-400)/2,(m_nHeight-400)/2, 400, 400);
             /* Crop the original image to the defined ROI */
             inFrame = inFrame(roi);
             cv::resize(inFrame,inFrame,resize,0,0,INTER_LANCZOS4);
             dateCoordinate = cv::Point(800 -260, 800-25);
           }

        }

       cv::putText(inFrame,
                    datebuffer.data(),
                    dateCoordinate, // Coordinates
                    cv::FONT_HERSHEY_COMPLEX_SMALL, // Font
                     1.0, // Scale. 2.0 = 2x bigger
                     cv::Scalar(0,255,0), // Color
                     1, // Thickness
                     CV_AA);

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
               cv::resize(inFrame,inFrame,resizeHVGA,0,0,INTER_LANCZOS4);
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
                   CV_AA);

    if(!m_bSlideS)
    {
           QImage image(inFrame.data,inFrame.cols,inFrame.rows,inFrame.step,QImage::Format_RGB888);
           emit inDisplay(QPixmap::fromImage(image.rgbSwapped()));


    }



//         deltaTime += endFrame - beginFrame;

//         ++frames;

//       //if you really want FPS
//       if( clockToMilliseconds(deltaTime)>100.0){
//           averageFrameTimeMilliseconds  = 1000.0/(frameRate==0?0.001:frameRate);

//            qDebug()<<"FPS was:"<<averageFrameTimeMilliseconds/frames << '\n';
//            deltaTime -= CLOCKS_PER_SEC;
//            frames = 0;
//       }


    }

    m_camera.release();

}

void VideoProcessorThread::TakePhoto(const cv::Mat_<cv::Vec4b> &image)
{
    m_vPictures.push_back(image); // can save pictures latter
    m_bTakeImage = false;
    //qDebug() << "size vector: " << m_vPictures.size();
    QImage qimage(image.data,image.cols,image.rows,image.step,QImage::Format_RGB888);


    QString imagePath = "/home/phytec-vizaar/yocto/prj_workspace/Qt/build-Qt_JsonCamera-Desktop_Qt_5_6_3_GCC_64bit-Release/example.jpg";
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
  m_nWidth      = 640;
  m_nHeight     = 480;
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


      switch(m_eResolution)
      {
       case JsonCameraSettings::VGA:
             qDebug() << "Setting VGA";
             terminal_cmd->start("sh",QStringList()<<"-c"<<"media-ctl -V ''2:'0[fmt:'SGRBG8'/'640x480'('32,66')/'2560x1920']'");
             terminal_cmd->waitForStarted();
             terminal_cmd->startDetached("sh",QStringList()<<"-c"<<"media-ctl -V '\"ipu0-csi0-sd\":0[fmt:'SGRBG8'/'640x480']'");
             terminal_cmd->startDetached("sh",QStringList()<<"-c"<<"media-ctl -V '\"ipu0-csi0-sd\":1[fmt:'SGRBG8'/'640x480']'");
         break;
       case JsonCameraSettings::SVGA:
             qDebug() << "Setting SVGA";
             terminal_cmd->start("sh",QStringList()<<"-c"<<"media-ctl -V ''2:'0[fmt:'SGRBG8'/'800x600'('512,426')/'1600x1200']'");
             terminal_cmd->waitForStarted();
             terminal_cmd->startDetached("sh",QStringList()<<"-c"<<"media-ctl -V '\"ipu0-csi0-sd\":0[fmt:'SGRBG8'/'800x600']'");
             terminal_cmd->startDetached("sh",QStringList()<<"-c"<<"media-ctl -V '\"ipu0-csi0-sd\":1[fmt:'SGRBG8'/'800x600']'");
         break;
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
             terminal_cmd->start("sh",QStringList()<<"-c"<<"media-ctl -V ''2:'0[fmt:'SGRBG8'/'640x480'('32,66')/'2560x1920']'");
             terminal_cmd->waitForStarted();
             terminal_cmd->startDetached("sh",QStringList()<<"-c"<<"media-ctl -V '\"ipu0-csi0-sd\":0[fmt:'SGRBG8'/'640x480']'");
             terminal_cmd->startDetached("sh",QStringList()<<"-c"<<"media-ctl -V '\"ipu0-csi0-sd\":1[fmt:'SGRBG8'/'640x480']'");
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

