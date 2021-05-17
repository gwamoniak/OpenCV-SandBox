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
    QMessageBox msgBox;


#ifdef Q_OS_LINUX
    if(!m_CameraProcess.LinuxCameraSetup())
    {
        qWarning() << "Linux Setup error " << '\n';
        msgBox.setText("Linux Camera Setup error");
        msgBox.exec();
    }
#endif

    if(m_CameraProcess.LoadCameraSettings())
    {


        msgBox.setText("Please connect a camera.");
        msgBox.exec();
        try
        {
               m_CameraProcess.start();
        }
        catch (const std::exception& e)
        {

            qWarning() << "CameraProcess error: " << e.what() <<'\n';
            msgBox.setText("Connection with the camera interrupted");
            msgBox.exec();

        }

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

void ShowVideoCapture::on_ButtonHistogram_clicked()
{
   qDebug() << "Histogram clicked";
   QtCharts::QBarSet *set0 = new QtCharts::QBarSet("Altuve");
      QtCharts::QBarSet *set1 = new QtCharts::QBarSet("Martinez");
     QtCharts::QBarSet *set2 = new QtCharts::QBarSet("Segura");
     QtCharts::QBarSet *set3 = new QtCharts::QBarSet("Simmons");

      // Assign values for each bar
      *set0 << 283 << 341 << 313 << 338 << 346 << 335;
      *set1 << 250 << 315 << 282 << 307 << 303 << 330;
      *set2 << 294 << 246 << 257 << 319 << 300 << 325;
      *set3 << 248 << 244 << 265 << 281 << 278 << 313;

      // Add all sets of data to the chart as a whole
      // 1. Bar Chart
      QtCharts::QBarSeries *series = new QtCharts::QBarSeries();

      // 2. Stacked bar chart
      // QHorizontalStackedBarSeries *series = new QHorizontalStackedBarSeries();
      series->append(set0);
      series->append(set1);
      series->append(set2);
      series->append(set3);

      // Used to define the bar chart to display, title
      QtCharts::QChart *chart = new QtCharts::QChart();

      // Add the chart
      chart->addSeries(series);

      // Set title
      chart->setTitle("Batting Avg by Year");

      // Define starting animation
      // NoAnimation, GridAxisAnimations, SeriesAnimations
      chart->setAnimationOptions(QtCharts::QChart::AllAnimations);

      // Holds the category titles
      QStringList categories;
      categories << "2013" << "2014" << "2015" << "2016" << "2017" << "2018";

      // Adds categories to the axes
      QtCharts::QBarCategoryAxis *axis = new QtCharts::QBarCategoryAxis();
      axis->append(categories);
      chart->createDefaultAxes();

      // 1. Bar chart
      chart->setAxisX(axis, series);

      // 2. Stacked Bar chart
      // chart->setAxisY(axis, series);

      // Define where the legend is displayed
      chart->legend()->setVisible(true);
      chart->legend()->setAlignment(Qt::AlignBottom);

      // Used to display the chart
      QtCharts::QChartView *chartView = new QtCharts::QChartView(chart);
      chartView->setRenderHint(QPainter::Antialiasing);

      // Used to change the palette
      QPalette pal = qApp->palette();

      // Change the color around the chart widget and text
      pal.setColor(QPalette::Window, QRgb(0xffffff));
      pal.setColor(QPalette::WindowText, QRgb(0x404044));

      // Apply palette changes to the application
      qApp->setPalette(pal);

      chartView->show();



}
