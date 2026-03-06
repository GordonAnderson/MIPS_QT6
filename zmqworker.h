#ifndef ZMQWORKER_H
#define ZMQWORKER_H
#define UseZmQ
#ifdef UseZmQ

#include <QObject>
#include <QThread>
#include <QDebug>
#include <zmq.hpp>
#include <string>

class ZmqWorker;

class ZmqReqRep : public QObject
{
    Q_OBJECT
signals:
    void requestTriggered(QString cmd, int mStimeout);
public:
    explicit ZmqReqRep(QObject *parent = nullptr);
    ~ZmqReqRep();
    void sendMessage(QString msg);
    bool begin(QString address);
    void stop(void);
    QString command(QString cmd);
private:
    QThread   *workerThread;
    ZmqWorker *worker;
    QString   messageReceived;
    QString   messageError;
    QString   serverAddress;
    int       mStimeout;
public slots:
    void messageReceiver(QString msg);
    void errorMessage(QString msg);
};

/**
 * @brief ZmqWorker handles Request-Reply with a persistent connection.
 * * Performance is improved by keeping the ZMQ context and socket alive
 * as class members rather than re-initializing them on every request.
 *
 * test address: "tcp://localhost:5555"
 */
class ZmqWorker : public QObject {
    Q_OBJECT

public:
    explicit ZmqWorker(QObject *parent = nullptr)
        : QObject(parent),
        context(1),
        socket(context, ZMQ_REQ),
        isConnected(false) {}

    QString serverAddress;

public slots:
    /**
     * @param message The text to send to the server.
     * @param timeoutMs The max time to wait for a reply (default 5000ms).
     */
    void sendRequest(const QString &message, int timeoutMs = 5000) {
        try {
            // Lazy initialization: Connect only once or if address changes
            if (!isConnected) {
                if (serverAddress.isEmpty()) {
                    emit errorOccurred("Server address is not set.");
                    return;
                }

                // Configure timeouts (these persist for the life of the socket)
                socket.set(zmq::sockopt::rcvtimeo, timeoutMs);
                socket.set(zmq::sockopt::sndtimeo, timeoutMs);

                // Set LINGER to 0 so the socket closes immediately on destruction
                socket.set(zmq::sockopt::linger, 0);

                socket.connect(serverAddress.toStdString().c_str());
                isConnected = true;
            }

            // Prepare Request
            std::string msgStr = message.toStdString();
            zmq::message_t request(msgStr.size());
            memcpy(request.data(), msgStr.c_str(), msgStr.size());

            // Send message
            auto send_result = socket.send(request, zmq::send_flags::none);

            if (!send_result.has_value()) {
                emit errorOccurred("Failed to send request (Send Timeout).");
                handleSocketError(); // Reset socket on failure
                return;
            }

            // Receive Reply (Blocking until timeout)
            zmq::message_t reply;
            auto recv_result = socket.recv(reply, zmq::recv_flags::none);

            // Handle Timeout
            if (!recv_result.has_value()) {
                emit errorOccurred("Server request timed out. No response received.");
                // IMPORTANT: In ZMQ REQ-REP, if a REQ fails or times out,
                // the state machine is stuck. We must recreate the socket.
                handleSocketError();
                return;
            }

            // Success: Emit the reply
            QString response = QString::fromStdString(
                std::string(static_cast<char*>(reply.data()), reply.size())
                );
            emit replyReceived(response);

        } catch (const zmq::error_t& e) {
            emit errorOccurred(QString("ZMQ Exception: %1").arg(e.what()));
            handleSocketError();
        }
    }

private:
    /**
     * @brief Resets the socket.
     * In the REQ-REP pattern, if a sequence is broken (e.g. timeout),
     * the socket enters an error state and must be closed and reopened.
     */
    void handleSocketError() {
        socket = zmq::socket_t(context, ZMQ_REQ);
        isConnected = false;
    }

    zmq::context_t context;
    zmq::socket_t socket;
    bool isConnected;

signals:
    void replyReceived(const QString &reply);
    void errorOccurred(const QString &err);
};

#endif
#endif // ZMQWORKER_H
