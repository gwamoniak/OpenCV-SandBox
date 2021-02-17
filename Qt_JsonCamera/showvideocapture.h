#ifndef SHOWVIDEOCAPTURE_H
#define SHOWVIDEOCAPTURE_H

#include "videoprocessorthread.h"
#include "jsonCamerasettings.h"
#include <QMainWindow>
#include <QKeyEvent>
#include <QProcess>
#include <QBasicTimer>
#include <QDebug>


namespace Ui {
class ShowVideoCapture;
} // namespace Ui

class ShowVideoCapture : public QMainWindow
{
    Q_OBJECT

public:
    explicit ShowVideoCapture(QWidget *parent = nullptr);
    ~ShowVideoCapture() override;

    typedef std::vector<cv::Mat_<cv::Vec4b>> MatVector;



private slots:

    void on_radioDefault_clicked();

    void on_ButtonStart_clicked();

    void on_ButtonStop_clicked();

    void on_ButtonExit_clicked();

    void on_radioXGA_clicked();

    void on_radioHD_clicked();

    void on_radioFullHD_clicked();

    void on_ButtonSave_clicked();

    void on_SpinBoxExposure_valueChanged(double arg1);

    void on_SpinBoxGain_valueChanged(double arg1);

    void on_ButtonDefaultJSON_clicked();

    void on_radioResetView_clicked();

    void on_radioSide2Side_clicked();

    void on_ButtonCapture_clicked();

    void on_spinBox_valueChanged(int arg1);

    void on_ButtonSavePicture_clicked();

private:
    Ui::ShowVideoCapture *ui;
    VideoProcessorThread m_CameraProcess;
    JsonCameraSettings m_CameraSettings;


};



#endif // SHOWVIDEOCAPTURE_H
