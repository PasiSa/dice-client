#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void handleConnectButton();
    void handleDisconnect();
    void handleConnected();
    void displayError(QAbstractSocket::SocketError socketError);

private:
    void startConnection();

    Ui::MainWindow *ui;
    QTcpSocket *tcpSocket_;
};
#endif // MAINWINDOW_H
