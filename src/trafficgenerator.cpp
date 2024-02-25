#include <sstream>
#include "trafficgenerator.h"

TrafficGenerator::TrafficGenerator(
        TrafficGeneratorListener& listener, QHostAddress addr, int port,
        unsigned int bytes, unsigned int interval, unsigned int count)
    : listener_(listener), address_(addr), port_(port), bytes_(bytes),
    interval_(interval), maxcount_(count)
{    
    socket_ = new QTcpSocket(this);
    connect(socket_, &QAbstractSocket::errorOccurred, this, &TrafficGenerator::handleError);
    connect(socket_, &QTcpSocket::connected, this, &TrafficGenerator::handleConnected);
    connect(socket_, &QTcpSocket::disconnected, this, &TrafficGenerator::handleDisconnect);
    connect(socket_, &QIODevice::bytesWritten, this, &TrafficGenerator::handleWritten);
}


TrafficGenerator::~TrafficGenerator()
{
    delete socket_;
}


void TrafficGenerator::start()
{
    socket_->connectToHost(address_, port_);
}


void TrafficGenerator::stop()
{
    socket_->disconnectFromHost();
    listener_.ShowMessage("Traffic Generator stopped");
}


void TrafficGenerator::handleConnected()
{
    sendBatch();
}


void TrafficGenerator::handleError(QAbstractSocket::SocketError socketError)
{
    listener_.GeneratorFinished("Error: " + socket_->errorString());
    listener_.ShowError(socket_->errorString());
}


void TrafficGenerator::handleDisconnect()
{
    qDebug() << "Disconnected";
}


void TrafficGenerator::handleWritten(qint64 bytes)
{
    count_ += bytes;
    std::stringstream ss;
    ss << "Test ongoing, total bytes written: " << count_;
    listener_.ShowMessage(QString::fromStdString(ss.str()));
    if (maxcount_ && count_ >= maxcount_ * bytes_) {
        socket_->disconnectFromHost();
        listener_.GeneratorFinished("Success");
    } else {
        QTimer::singleShot(interval_, this, &TrafficGenerator::onTimerTick);
    }
}


void TrafficGenerator::sendBatch()
{
    unsigned int b = bytes_;

    if (socket_->state() == QAbstractSocket::UnconnectedState ||
            socket_->state() == QAbstractSocket::ClosingState) {
        // If socket is winding down (e.g. interrupted by user), do not send anymore.
        return;
    }

    // It is possible that one of the earlier writes was not able to send
    // full batch because of full send buffer, therefore we need to check
    // the balances.
    if (maxcount_ && maxcount_ * bytes_ - count_ < bytes_) {
        b = (maxcount_ * bytes_) - count_;
    }

    QByteArray block(b, 'A' + (count_ % 26));
    socket_->write(block);
}


void TrafficGenerator::onTimerTick()
{
    sendBatch();
}