#include <QtNetwork>
#include <QMessageBox>
#include <QIntValidator>

#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), is_active_(false), generator_(nullptr)
{
    ui->setupUi(this);
    connect(ui->connectButton, SIGNAL(clicked()), this, SLOT(handleConnectButton()));
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::handleConnectButton()
{
    if (!is_active_) {
        startConnection();
    } else {
        stopConnection();
    }
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
    ui->bytesEdit->setValidator(new QIntValidator(0, 10000000, this));
    if (ui->bytesEdit->text().length() <= 0) {
        QMessageBox::critical(this, tr("Dice Client"), tr("Invalid input"));
        return;
    }
    ui->intervalEdit->setValidator(new QIntValidator(0, 3600 * 1000, this));
    if (ui->intervalEdit->text().length() <= 0) {
        QMessageBox::critical(this, tr("Dice Client"), tr("Invalid input"));
        return;
    }
    ui->countEdit->setValidator(new QIntValidator(0, 10000000, this));
    if (ui->countEdit->text().length() <= 0) {
        QMessageBox::critical(this, tr("Dice Client"), tr("Invalid input"));
        return;
    }

    ui->activityLabel->setText("Traffic Generator is active");
    ui->activityLabel->setStyleSheet("color: green;");
    ui->connectButton->setText("Stop");
    is_active_ = true;

    if (generator_) {
        // just delete earlier generator if there was one already from previous run
        delete generator_;
    }
    generator_ = new TrafficGenerator(
            *this,
            QHostAddress(ui->addressEdit->text()),
            ui->portEdit->text().toInt(),
            ui->bytesEdit->text().toInt(),
            ui->intervalEdit->text().toInt(),
            ui->countEdit->text().toInt());
    generator_->start();
}


void MainWindow::stopConnection()
{
    generator_->stop();
    ui->connectButton->setText("Start");
    is_active_ = false;
}


void MainWindow::GeneratorFinished(QString message)
{
    ui->activityLabel->setText("Generator Finished: " + message);
    ui->activityLabel->setStyleSheet("color: red;");
}


void MainWindow::ShowMessage(QString message)
{
    ui->activityLabel->setText(message);
}


void MainWindow::ShowError(QString message)
{
    QMessageBox::information(this, tr("Dice Client"),
        tr("The following error occurred: %1.").arg(message));
}
