#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include "trafficgenerator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow, TrafficGeneratorListener
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void displayError(QAbstractSocket::SocketError socketError);

    // from TrafficGeneratorListener
    virtual void GeneratorFinished(QString message);
    virtual void ShowMessage(QString message);
    virtual void ShowError(QString message);

public slots:
    void handleConnectButton();

private:
    void startConnection();

    Ui::MainWindow *ui;
    TrafficGenerator *generator_;
};
#endif // MAINWINDOW_H
