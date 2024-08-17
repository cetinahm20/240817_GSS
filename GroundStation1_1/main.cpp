#include "mainwindow.h"

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QFile>
#include <QWebEngineView>
#include <QVBoxLayout>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

  /*QWebEngineView view;
    view.setUrl(QUrl("http://192.168.166.205:5000/"));  // Flask sunucusunun adresi
    view.setWindowTitle("Canlı Yayın");
    view.resize(500, 340);
    view.move(2030,425);
    view.show();

  */

    MainWindow w;


    w.show();
    return a.exec();
}
