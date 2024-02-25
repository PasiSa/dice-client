#ifndef TRAFFICGENERATOR_H
#define TRAFFICGENERATOR_H

#include <QObject>
#include <QTimer>
#include <QTcpSocket>


/** Abstract interface implemented by the user interface.
 * Traffic generator uses this interface to deliver notifications and messages.
 */
class TrafficGeneratorListener
{
public:
    /** TrafficGenerator calls this function when it has finished its task.
     * @param message shown on the user interface.
     */
    virtual void GeneratorFinished(QString message) = 0;
 
    /** Show message on the UI. This is for status updates, etc.
     * @param message Message to be shown
     */
    virtual void ShowMessage(QString message) = 0;

    /** Show error message. This could pop up a dialog or something more 
     * visible than just status update.
     * @param message Message to be shown on dialog
     */
    virtual void ShowError(QString message) = 0;
};


/** Simple traffic generator that sends constant size batches to server with
 * given intervals. Maximum number of batches can be given, or the generator
 * can keep sending idefinitely.
*/
class TrafficGenerator : public QObject
{
    Q_OBJECT

public:
    /**
     * Create a new traffic generator.
     * @param listener Listener instance where status updates are sent to
     * @param addr Server address we are connecting to
     * @param port TCP port we are connecting to
     * @param bytes Number of bytes sent in single TCP send batch
     * @param interval Number of milliseconds we wait between batches
     * @param count Number of batches to send before finishing.
     *          If 0, will keep sending indefinitely.
    */
    TrafficGenerator(TrafficGeneratorListener& listener, QHostAddress addr, int port,
            unsigned int bytes, unsigned int interval, unsigned int count);
    ~TrafficGenerator();

    /**
     * Start generating traffic according to parameters.
    */
    void start();

public slots:
    void onTimerTick();

    // Events on socket
    void handleDisconnect();
    void handleConnected();
    void handleError(QAbstractSocket::SocketError socketError);
    void handleWritten(qint64 bytes);

private:
    void sendBatch();

    TrafficGeneratorListener& listener_;  // UI listener object for notifications
    QHostAddress address_;
    int port_;
    QTcpSocket *socket_;  // Client socket used for sending data
    unsigned int bytes_;  // Number of bytes in a single write batch
    unsigned int interval_;  // Interval between batches in milliseconds
    unsigned int maxcount_;  // How many iterations we are to send in total
    unsigned int count_;  // How many bytes we have sent so far
};

#endif  // TRAFFICGENERATOR_H
