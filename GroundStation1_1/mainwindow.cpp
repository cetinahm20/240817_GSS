#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <QFile>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QUrl>
#include <QMediaContent>
#include <QMediaRecorder>
#include <QUdpSocket>
#include <QVariant>
#include <QtCore>
#include <QtGui>
#include <QRandomGenerator>
#include <QWebEngineView>
#include <QDateTime>
#include <QScreen>
#include <QGuiApplication>

void MainWindow::readData()
{
    while (socket4->hasPendingDatagrams()) {
        QByteArray datagram(socket4->pendingDatagramSize(), Qt::Uninitialized);
        socket4->readDatagram(datagram.data(), datagram.size());

        qDebug() << "Datagram raw data:" << datagram.toHex() << "," << datagram.size();

        int index = 0;
        auto extractData = [&](auto& value, int size) {
            value = qFromLittleEndian<typename std::remove_reference<decltype(value)>::type>(
                reinterpret_cast<const uchar*>(datagram.mid(index, size).constData()));
            index += size;
        };

        extractData(telemetri.pakNo, 2);
        telemetri.state = static_cast<unsigned char>(datagram[index++]);
        telemetri.hataKodu = static_cast<unsigned char>(datagram[index++]);
        extractData(telemetri.zaman, 8);
        extractData(telemetri.bas1, 4);
        extractData(telemetri.bas2, 4);
        extractData(telemetri.yuk1, 4);
        extractData(telemetri.yuk2, 4);
        extractData(telemetri.fark, 4);
        extractData(telemetri.hiz, 4);
        extractData(telemetri.sic, 4);
        extractData(telemetri.ger, 4);
        extractData(telemetri.lat, 4);
        extractData(telemetri.lon, 4);
        extractData(telemetri.alt, 4);
        extractData(telemetri.pitch, 4);
        extractData(telemetri.roll, 4);
        extractData(telemetri.yaw, 4);
        extractData(telemetri.rhrh, 2);
        extractData(telemetri.iot, 4);
        extractData(telemetri.takNo, 4);
    }


}

//UDP prokolüyle veri atma
void MainWindow::writeData(){


    QByteArray datagram;
    datagram.append('1');
    QHostAddress receiverAddress("192.168.166.205");

    socket2->writeDatagram(datagram,datagram.size(),receiverAddress, 65433);



    //qDebug()<<datagram.size();
}

void MainWindow::rgbData()
{
    QByteArray datagram;
    QDataStream out(&datagram, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);
    out << telemetri.rhrh;
    QHostAddress receiverAddress("192.168.166.205");

    socket1->writeDatagram(datagram, receiverAddress, 65432);
  //  qDebug()<<datagram.size()<<","<<datagram;

}

void MainWindow::iotData()
{
    float iot=31.31;
    QHostAddress receiverAddress("192.168.166.205");
    QByteArray datagram;
    datagram.resize(sizeof(float));

    // float değeri QByteArray'e yazma
    qToLittleEndian(iot, reinterpret_cast<uchar*>(datagram.data()));

    // Datagram boyutunu ve içeriğini kontrol etme
    //qDebug() << datagram.size() << "," << datagram.toHex();

    // Datagram'ı UDP ile gönderme
    socket5->writeDatagram(datagram, receiverAddress, 65434);
}

void MainWindow::baslat_transfer()
{
   /* QByteArray datagram;
    datagram.append('1');
    QHostAddress receiverAddress("192.168.166.205");

    socket6->writeDatagram(datagram,datagram.size(),receiverAddress, 65435);
*/

}



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Set up map and 3D views
    ui->harita->setSource(QUrl(QStringLiteral("qrc:/map.qml")));
    ui->harita->show();
    ui->threed->setSource(QUrl(QStringLiteral("qrc:/durus.qml")));
    ui->threed->show();

    // Set up web engine view
    auto view = new QWebEngineView(this);
    view->setUrl(QUrl("http://192.168.166.205:5000/"));
    view->setGeometry(2030, 420, 500, 340);
    view->show();

    // Connect signals to QML slots
    auto haritaObj = ui->harita->rootObject();
    connect(this, SIGNAL(setCenter(QVariant, QVariant)), haritaObj, SLOT(setCenter(QVariant, QVariant)));
    connect(this, SIGNAL(setLocMarker(QVariant, QVariant)), haritaObj, SLOT(setLocMarker(QVariant, QVariant)));
    emit setCenter(telemetri.lat, telemetri.lon);
    emit setLocMarker(telemetri.lat, telemetri.lon);

    auto threedObj = ui->threed->rootObject();
    connect(this, SIGNAL(eulerFunction(QVariant, QVariant, QVariant)), threedObj, SLOT(eulerFunction(QVariant, QVariant, QVariant)));

    // Set up logo
    ui->logo->setPixmap(QPixmap(":/img/img/TayfLogo.png"));

    // Set up UDP sockets
    auto createSocket = [&](int port) {
        auto socket = new QUdpSocket(this);
        socket->bind(QHostAddress::Any, port);
        return socket;
    };
    socket2 = createSocket(65433);
    socket1 = createSocket(65432);
    socket4 = createSocket(23456);
    socket5 = createSocket(65434);
    socket6 = createSocket(65435);

    // Set up timer
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::timerFunc);

    // Define scatter style and font for graphs
    QCPScatterStyle scatterStyle(QCPScatterStyle::ssDisc, Qt::black, 2);

    QFont plotFont = font();
    plotFont.setPointSize(6);

    // Common graph setup function
    auto setupGraph = [&](QCustomPlot* plot, const QString& xLabel, const QString& yLabel, const QPen& pen, const QCPScatterStyle& scatterStyle, int xRangeMin, int xRangeMax, int yRangeMin, int yRangeMax) {
        plot->addGraph();
        plot->graph(0)->setScatterStyle(scatterStyle);
        plot->graph(0)->setLineStyle(QCPGraph::lsLine);
        plot->graph(0)->setPen(pen);
        plot->setBackground(QBrush(Qt::transparent));
        plot->xAxis->setLabel(xLabel);
        plot->yAxis->setLabel(yLabel);
        plot->xAxis->setLabelFont(plotFont);
        plot->xAxis->setTickLabelFont(plotFont);
        plot->yAxis->setLabelFont(plotFont);
        plot->yAxis->setTickLabelFont(plotFont);
        plot->xAxis->setTickLabelColor(Qt::white);
        plot->xAxis->setLabelColor(Qt::white);
        plot->xAxis->setSubTickPen(QPen(Qt::white));
        plot->xAxis->setBasePen(QPen(Qt::white));
        plot->yAxis->setTickLabelColor(Qt::white);
        plot->yAxis->setLabelColor(Qt::white);
        plot->yAxis->setSubTickPen(QPen(Qt::white));
        plot->yAxis->setBasePen(QPen(Qt::white));
        plot->xAxis->setRange(xRangeMin, xRangeMax);
        plot->yAxis->setRange(yRangeMin, yRangeMax);
    };

    // Set up all graphs
    QPen pen(QColor(169, 169, 169), 4);
    QPen penB(QColor(169, 169, 169), 13);
    setupGraph(ui->bas1p, "Paket Numarası", "Basınç(Pa)", pen, scatterStyle, -55, -5, 1000, 1200);
    setupGraph(ui->bas2p, "Paket Numarası", "Basınç(Pa)", pen, scatterStyle, -55, 5, 1000, 1200);
    setupGraph(ui->yuk1p, "Paket Numarası", "Yükseklik(m)", penB, scatterStyle, -55, 5, 0, 700);
    setupGraph(ui->yuk2p, "Paket Numarası", "Yükseklik(m)", penB, scatterStyle, -55, 5, 0, 700);
    setupGraph(ui->sicp, "Paket Numarası", "Sıcaklık(C°)", pen, scatterStyle, -55, 5, 0, 40);
    setupGraph(ui->inisp, "Paket Numarası", "İniş Hızı(m/s)", pen, scatterStyle, -55, 5, 0, 20);
    setupGraph(ui->farkp, "Paket Numarası", "Fark (m)", pen, scatterStyle, -55, 5, 0, 250);
    setupGraph(ui->voltp, "Paket Numarası", "Gerilim (Volt)", pen, scatterStyle, -55, 5, 0, 10);
    setupGraph(ui->nemp, "Paket Numarası", "Nem (g/m^3)", pen, scatterStyle, 30, 30, 0, 20);

    // Write CSV header
    QFile file("TMUY2024_ 1592571 _TLM.csv");
    if (file.open(QIODevice::Append | QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        stream << "Pak No,State,Hata Kodu,Saat,Basinc1,Basinc2,Yukseklik1,Yukseklik2,Fark,Hiz,Sicaklik,Gerilim,Enlem,Boylam,GPS Yukseklik,Pitch,Roll,Yaw,RHRH,Nem,Takim No,\n";
        file.close();
    }

    // Play video and set initial UI elements

    QPixmap pix(":/img/img/yesil.png");
    for (auto label : {ui->kod1, ui->kod2, ui->kod3, ui->kod4, ui->kod5}) {
        label->setPixmap(pix.scaled(35, 35, Qt::KeepAspectRatio));
    }

    // Set up sound
    sound->setMedia(QUrl::fromLocalFile("C:/Users/aali_/OneDrive/Belgeler/GroundStation1_1/img/buzzer.mp3"));
    sound->setVolume(50);

    QPixmap sta_0(":/img/img/ucusa_hazir.png");
    ui->durum->setAlignment(Qt::AlignCenter);
    ui->durum->setPixmap(sta_0.scaled(ui->durum->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

}

MainWindow::~MainWindow()
{


   delete ui;
}
//Ayrıl butonunun fonksiyonunun belirlendiği kısım
void MainWindow::on_ayril_clicked()
{

   writeData();

}


//Başlat butonunun fonksiyonunun belirlendiği kısım
void MainWindow::on_baslat_clicked(bool checked)
{

    if(checked==true){

    timer->start(1000);
    baslat_transfer();

    }

 else{

    timer->stop();


    }

}

void MainWindow::on_rgb_clicked()
{

    telemetri.rhrh = 0;

    QString rhrh_string = ui->rhrh_kod->text();

    // Check second and fourth characters for 'G', 'B', 'R'
    auto setBits = [&](int index, int shift) {
        QChar ch = rhrh_string.at(index);
        if (ch == 'G') telemetri.rhrh |= (0x01 << shift);
        else if (ch == 'B') telemetri.rhrh |= (0x02 << shift);
        else if (ch == 'R') telemetri.rhrh |= (0x04 << shift);
    };

    setBits(1, 0);
    setBits(3, 8);

    // Set the bits for the first and third characters as digits
    telemetri.rhrh |= (rhrh_string.at(0).digitValue() << 3) | (rhrh_string.at(2).digitValue() << 11);

    rgbData();


}


void MainWindow::timerFunc(){

    telemetri.pakNo++;
    readData();
    iotData();

    emit setCenter(telemetri.lat, telemetri.lon);
    emit setLocMarker(telemetri.lat, telemetri.lon);
    emit eulerFunction(telemetri.roll, telemetri.yaw, telemetri.pitch);

    //telemetri.pitch += 10;
    //telemetri.roll += 10;
    //telemetri.yaw += 10;
    // Update telemetry data and randomize bas1
    //telemetri.bas1 = QRandomGenerator::global()->bounded(1000, 1200);
    //telemetri.hataKodu = 1;
    //telemetri.state += 1;
    // Helper function to update the graph

    auto updateGraph = [&](auto graph, int xRangeMin, int xRangeMax, int yRangeMin, int yRangeMax, auto data) {
        graph->addGraph();
        graph->graph(0)->addData(telemetri.pakNo, data);
        graph->xAxis->setRange(xRangeMin, xRangeMax);
        graph->yAxis->setRange(yRangeMin, yRangeMax);
        graph->replot();
        graph->update();
    };


    // Update all graphs
    updateGraph(ui->bas1p, telemetri.pakNo - 55, telemetri.pakNo + 5, 1000, 1200, telemetri.bas1);
    updateGraph(ui->bas2p, telemetri.pakNo - 55, telemetri.pakNo + 5, 1000, 1200, telemetri.bas2);
    updateGraph(ui->yuk1p, telemetri.pakNo - 55, telemetri.pakNo + 5, 0, 700, telemetri.yuk1);
    updateGraph(ui->yuk2p, telemetri.pakNo - 55, telemetri.pakNo + 5, 0, 700, telemetri.yuk2);
    updateGraph(ui->sicp, telemetri.pakNo - 55, telemetri.pakNo + 5, 20, 40, telemetri.sic);
    updateGraph(ui->inisp, telemetri.pakNo - 55, telemetri.pakNo + 5, 0, 20, telemetri.hiz);
    updateGraph(ui->farkp, telemetri.pakNo - 55, telemetri.pakNo + 5, 0, 250, telemetri.fark);
    updateGraph(ui->voltp, telemetri.pakNo - 55, telemetri.pakNo + 5, 0, 10, telemetri.ger);
    updateGraph(ui->nemp, telemetri.pakNo - 55, telemetri.pakNo + 5, 0, 20, telemetri.iot);

    // Format date and time
    auto formatDateTime = [&](const QString& datetimeStr) {
        return datetimeStr.mid(6, 2) + "/" + datetimeStr.mid(4, 2) + "/" + datetimeStr.mid(0, 4) + " " +
               datetimeStr.mid(8, 2) + ":" + datetimeStr.mid(10, 2) + ":" + datetimeStr.mid(12, 2);
    };

     formattedDateTime = formatDateTime(QString::number(telemetri.zaman));

    // Prepare data string for CSV and list widget
     data = QString::number(telemetri.pakNo) + ", " + QString::number(telemetri.state) + ", " +
                   QString::number(telemetri.hataKodu) + ", " + formattedDateTime + ", " +
                   QString::number(telemetri.bas1) + ", " + QString::number(telemetri.bas2) + ", " +
                   QString::number(telemetri.yuk1) + ", " + QString::number(telemetri.yuk2) + ", " +
                   QString::number(telemetri.fark) + ", " + QString::number(telemetri.hiz) + ", " +
                   QString::number(telemetri.sic) + ", " + QString::number(telemetri.ger) + ", " +
                   QString::number(telemetri.lat) + ", " + QString::number(telemetri.lon) + ", " +
                   QString::number(telemetri.alt) + ", " + QString::number(telemetri.pitch) + ", " +
                   QString::number(telemetri.roll) + ", " + QString::number(telemetri.yaw) + ", " +
                   QString::number(telemetri.rhrh) + ", " + QString::number(telemetri.iot) + ", " +
                   QString::number(telemetri.takNo);

     // Add data to list widget and scroll to bottom
     if (ui->telemetry->count() >= 26) {
         delete ui->telemetry->takeItem(0);  // Remove the oldest item
     }

     ui->telemetry->addItem(data);
     ui->telemetry->scrollToBottom();

    // Update altitude difference
    ui->irtifa->setText("İRTİFA FARKI: " + QString::number(telemetri.fark));

    // Append data to CSV file
    QFile file("TMUY2024_ 1592571 _TLM.csv");
    if (file.open(QIODevice::Append | QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        stream << data + "\n";
        file.close();
    }

    // Update error indicators with pixmaps
    auto updateErrorIndicator = [&](int condition, QLabel* label, const QPixmap& pixmapOn, const QPixmap& pixmapOff) {
        label->setPixmap(((condition) ? pixmapOn : pixmapOff).scaled(35, 35, Qt::KeepAspectRatio));
        if (condition) sound->play();
    };

    QPixmap dur_1(":/img/img/yesil.png"), dur_2(":/img/img/kirmizi.png");
    updateErrorIndicator((telemetri.hataKodu & 0x01), ui->kod1, dur_2, dur_1);
    updateErrorIndicator((telemetri.hataKodu & 0x02) >> 1, ui->kod2, dur_2, dur_1);
    updateErrorIndicator((telemetri.hataKodu & 0x04) >> 2, ui->kod3, dur_2, dur_1);
    updateErrorIndicator((telemetri.hataKodu & 0x08) >> 3, ui->kod4, dur_2, dur_1);
    updateErrorIndicator((telemetri.hataKodu & 0x10) >> 4, ui->kod5, dur_2, dur_1);


    // Update error indicators with pixmaps for 5 states
    auto updateSatState = [&](int condition, QLabel* label, const QPixmap& sta_0, const QPixmap& sta_1, const QPixmap& sta_2, const QPixmap& sta_3, const QPixmap& sta_4, const QPixmap& sta_5) {
        label->setPixmap((condition == 0 ? sta_0 :
                              condition == 1 ? sta_1 :
                              condition == 2 ? sta_2 :
                              condition == 3 ? sta_3 :
                              condition == 4 ? sta_4 : sta_5).scaled(435, 340, Qt::KeepAspectRatio));
    };
   ui->durum->setAlignment(Qt::AlignCenter);
    // QPixmap images
    QPixmap sta_0(":/img/img/ucusa_hazir.png"),
        sta_1(":/img/img/yukselme.png"),
        sta_2(":/img/img/model_uydu_inis.png"),
        sta_3(":/img/img/ayrilma.png"),
        sta_4(":/img/img/gorev_yuku_inis.png"),
        sta_5(":/img/img/kurtarma.png");

    // Update labels based on different conditions
    updateSatState((telemetri.state), ui->durum, sta_0, sta_1, sta_2, sta_3, sta_4, sta_5);
    updateSatState((telemetri.state), ui->durum, sta_0, sta_1, sta_2, sta_3, sta_4, sta_5);
    updateSatState((telemetri.state), ui->durum, sta_0, sta_1, sta_2, sta_3, sta_4, sta_5);
    updateSatState((telemetri.state), ui->durum, sta_0, sta_1, sta_2, sta_3, sta_4, sta_5);
    updateSatState((telemetri.state), ui->durum, sta_0, sta_1, sta_2, sta_3, sta_4, sta_5);



}



