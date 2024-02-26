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
    count_ = 0;
    socket_->connectToHost(address_, port_);
}


void TrafficGenerator::stop()
{
    socket_->disconnectFromHost();
    listener_.ShowMessage("Traffic Generator stopped");
}


void TrafficGenerator::handleConnected()
{
    newBatch();
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
    block_remaining_ -= bytes;

    // Or should the UI message update be done just once per batch?
    std::stringstream ss;
    ss << "Test ongoing, total bytes written: " << count_;
    listener_.ShowMessage(QString::fromStdString(ss.str()));

    if (block_remaining_ > 0) {
        // Not all of the block was yet written, possibly due to buffers and flow control.
        // Continue with sending rest of the block.
        sendBatch();
        return;
    }

    // Start new block after timeout, or terminate if this was the last block
    if (maxcount_ && block_count_ >= maxcount_) {
        socket_->disconnectFromHost();
        listener_.GeneratorFinished("Success");
    } else {
        QTimer::singleShot(interval_, this, &TrafficGenerator::onTimerTick);
    }
}


void TrafficGenerator::sendBatch()
{
    QByteArray block(block_remaining_, 'A' + (block_count_ % 26));
    socket_->write(block);
}


void TrafficGenerator::newBatch()
{
    if (socket_->state() == QAbstractSocket::UnconnectedState ||
            socket_->state() == QAbstractSocket::ClosingState) {
        // If socket is winding down (e.g. interrupted by user), do not send anymore.
        return;
    }

    block_remaining_ = bytes_;
    block_count_++;

    sendBatch();
}


void TrafficGenerator::onTimerTick()
{
    newBatch();
}