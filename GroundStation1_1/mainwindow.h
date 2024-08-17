#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QTextStream>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QMediaRecorder>
#include <QUdpSocket>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVariant>
#include <QtCore>
#include <QtGui>
#include <QtQuick>



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#pragma pack(push,1)
typedef struct tTelemetri{


    short int pakNo =0;
    unsigned char state =0;
    unsigned char hataKodu = 0;
    int64_t zaman=0;
    float bas1 =0;
    float bas2 =0;
    float yuk1 =0;
    float yuk2 =0;
    float fark =0;
    float hiz =0;
    float sic =0;
    float ger =0;
    float lat =39.159;
    float lon =33.291;
    float alt =0;
    float pitch =0;
    float roll =0;
    float yaw =0;
    short int rhrh=0;
    float iot=0;
    int takNo =0;


}Telemetri;
#pragma pack(pop)

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    Telemetri telemetri;


signals:
        void newMessage(QString);
        void setCenter(QVariant,QVariant);
        void setLocMarker(QVariant,QVariant);
        void eulerFunction(QVariant, QVariant, QVariant);

private slots:



    void on_baslat_clicked(bool checked);
    void on_ayril_clicked();
    void on_rgb_clicked();
    void baslat_transfer();
    void timerFunc();
    void writeData();
    void rgbData();
    void iotData();
    void readData();




private:
    Ui::MainWindow *ui;
    QTimer *timer;
    QUdpSocket *socket1;
    QUdpSocket *socket2;
    QUdpSocket *socket4;
    QUdpSocket *socket5;
    QUdpSocket *socket6;
    QString datetimeStr;
    QString year ;
    QString month ;
    QString day ;
    QString hour ;
    QString minute ;
    QString second ;
    QString formattedDateTime;
    QString data;
    QMediaPlayer *sound = new QMediaPlayer;
    /*int  x;
    double  y1;
    double  y2;
    double  y3;
    double  y4;
    double  y5;
    double  y6;
    double  y7;
    double  y8;

*/
};
#endif // MAINWINDOW_H

