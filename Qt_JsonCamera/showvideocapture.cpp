#include "showvideocapture.h"
#include "ui_showvideocapture.h"
#include <QStringList>


ShowVideoCapture::ShowVideoCapture(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ShowVideoCapture)
{

     ui->setupUi(this);
     connect(ui->SpinBoxExposure, SIGNAL(valueChanged(double)),this, SLOT(setValue(double)));
     connect(ui->SpinBoxGain, SIGNAL(valueChanged(double)),this, SLOT(setValue(double)));
     connect(&m_CameraProcess,SIGNAL(inDisplay(QPixmap)),
            ui->inVideo,SLOT(setPixmap(QPixmap)));
     connect(&m_CameraProcess,SIGNAL(inPicture(QPixmap)),
            ui->inPicture,SLOT(setPixmap(QPixmap)));   

}

ShowVideoCapture::~ShowVideoCapture()
{
    m_CameraProcess.requestInterruption();
    m_CameraProcess.wait();
    delete ui;
}




void ShowVideoCapture::on_radioDefault_clicked()
{
    if(ui->radioDefault->isChecked())
    {
        m_CameraProcess.UpdateCameraSettings();

    }
}

void ShowVideoCapture::on_ButtonStart_clicked()
{
    qDebug() << "Start";

    if(m_CameraProcess.LoadCameraSettings())
    {

        QMessageBox msgBox;
        msgBox.setText("Please connect a camera.");
        msgBox.exec();
        m_CameraProcess.start();
    }
}

void ShowVideoCapture::on_ButtonStop_clicked()
{
    qDebug() << "Stop";
    m_CameraProcess.requestInterruption();
    //tCameraProcess.terminate();
}


void ShowVideoCapture::on_ButtonExit_clicked()
{
    QApplication::quit();
}


void ShowVideoCapture::on_radioXGA_clicked()
{
    if(ui->radioXGA->isChecked())
    {
        m_CameraSettings.setResolution(JsonCameraSettings::XGA);
        m_CameraSettings.SettingsUpdated();
    }
}

void ShowVideoCapture::on_radioHD_clicked()
{
    if(ui->radioHD->isChecked())
    {
        m_CameraSettings.setResolution(JsonCameraSettings::HD);
        m_CameraSettings.SettingsUpdated();
    }
}

void ShowVideoCapture::on_radioFullHD_clicked()
{
    if(ui->radioFullHD->isChecked())
    {
        m_CameraSettings.setResolution(JsonCameraSettings::FullHD);
        m_CameraSettings.SettingsUpdated();
    }
}

void ShowVideoCapture::on_ButtonSave_clicked()
{
    QJsonObject json;
    if(m_CameraSettings.IsUpdated())
   {
        m_CameraSettings.setEnum2Resolution();
        if(m_CameraSettings.write(json))
        {
            if(m_CameraProcess.passJSON(json))
            {
                m_CameraProcess.UpdateCameraSettings();
                m_CameraProcess.SaveCameraSettings();
            }
            else
            {
                m_CameraProcess.GetDefaultSettings();
            }
         }
        else
        {
            m_CameraProcess.GetDefaultSettings();
        }
    }
    else
    {
        qDebug()<< "No changes has been made, save as default";
        m_CameraProcess.GetDefaultSettings();
        m_CameraProcess.SaveCameraSettings();

    }
}

void ShowVideoCapture::on_SpinBoxExposure_valueChanged(double arg1)
{
    m_CameraSettings.setExposure(arg1);
    qDebug() << "changed exp: " << m_CameraSettings.getExposure();
     m_CameraSettings.SettingsUpdated();
}

void ShowVideoCapture::on_SpinBoxGain_valueChanged(double arg1)
{
    m_CameraSettings.setGain(arg1);
    qDebug() << "changed gain: " << m_CameraSettings.getGain();
     m_CameraSettings.SettingsUpdated();
}

void ShowVideoCapture::on_ButtonDefaultJSON_clicked()
{
    m_CameraProcess.GetDefaultSettings();
    m_CameraProcess.SaveCameraSettings();
}

void ShowVideoCapture::on_radioResetView_clicked()
{
    if(ui->radioResetView->isChecked())
    {
//        disconnect(&m_CameraProcess,SIGNAL(inPicture(QPixmap)),
//               ui->inPicture,SLOT(setPixmap(QPixmap)));
        ui->inPicture->clear();
        m_CameraProcess.setSide2Side(false);

    }

}

void ShowVideoCapture::on_radioSide2Side_clicked()
{

    m_CameraProcess.setSide2Side(true);
}

void ShowVideoCapture::on_ButtonCapture_clicked()
{
    m_CameraProcess.setTakenImage(true);

}


void ShowVideoCapture::on_spinBox_valueChanged(int arg1)
{
    MatVector tempVector = m_CameraProcess.getTakenPictures();
    if(arg1 <= tempVector.size())
        m_CameraProcess.setPictureNumber(arg1);

}

void ShowVideoCapture::on_ButtonSavePicture_clicked()
{

}
