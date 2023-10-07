#include <QtNetwork>
#include <QMessageBox>
#include <QIntValidator>

#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), tcpSocket_(new QTcpSocket(this))
{
    ui->setupUi(this);
    connect(ui->connectButton, SIGNAL(clicked()), this, SLOT(handleConnectButton()));
    connect(tcpSocket_, &QAbstractSocket::errorOccurred, this, &MainWindow::displayError);
    connect(tcpSocket_, &QTcpSocket::connected, this, &MainWindow::handleConnected);
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::handleConnectButton()
{
    qDebug() << "handleConnectButton";
    startConnection();
}


void MainWindow::handleDisconnect()
{
    QTcpSocket *sock = qobject_cast<QTcpSocket*>(sender());
    qDebug() << "handleDisconnect called but no matching socket found";
}


void MainWindow::startConnection()
{
    int port = 0;
    bool ok;
    ui->portEdit->setValidator(new QIntValidator(1, 65535, this));

    if (ui->portEdit->text().length() <= 0 || ui->addressEdit->text().length() <= 0) {
        QMessageBox::critical(this, tr("Dice Client"), tr("Invalid input"));
        return;
    }
    tcpSocket_->connectToHost(QHostAddress(ui->addressEdit->text()), ui->portEdit->text().toInt());
}


void MainWindow::handleConnected()
{
    ui->activityLabel->setText("Connection is active");
    ui->activityLabel->setStyleSheet("color: green;");

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << "jepeti jee";
    tcpSocket_->write(block);
}


void MainWindow::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Dice Client"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Dice Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(this, tr("Dice Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(tcpSocket_->errorString()));
    }
}
