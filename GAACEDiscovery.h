#pragma once

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QHostAddress>
#include <QStringList>

// Populated from each device's UDP discovery reply.
struct GAACEDeviceInfo
{
    QString      name;       // Device name (e.g. "Gauge-A")
    QString      type;       // Device type (e.g. "vacuum_gauge", "valve_controller")
    QString      version;    // Firmware version string
    QHostAddress ip;         // Resolved IP address
    int          port;       // TCP command port (default 23)
};

//
// GAACEDiscovery
//
// Call start() to broadcast a discovery packet and collect replies for
// kTimeoutMs milliseconds. For each new device found, newDeviceFound() is
// emitted. discoveryComplete() is emitted when the timeout expires.
//
// Intended usage: instantiate once, call start() on a 60-second QTimer.
// Already-connected devices are tracked in the known-device set via
// markConnected() / markDisconnected() so repeat discoveries only emit
// newDeviceFound() for genuinely new devices.
//
class GAACEDiscovery : public QObject
{
    Q_OBJECT

public:
    explicit GAACEDiscovery(QObject *parent = nullptr);
    ~GAACEDiscovery();

    // Broadcast a discovery packet and listen for kTimeoutMs milliseconds.
    // Safe to call while a previous discovery is still running — the previous
    // one is cleanly stopped first.
    void start();

    // Call after successfully opening a TCP connection to a device.
    // Prevents repeat discoveries from re-emitting newDeviceFound() for it.
    void markConnected(const QString &name);

    // Call when a TCP connection to a device drops.
    // Allows the device to be re-discovered and reconnected on the next cycle.
    void markDisconnected(const QString &name);

signals:
    // Emitted once per new device found during this discovery window.
    void newDeviceFound(const GAACEDeviceInfo &info);

    // Emitted when the discovery window closes (timeout expired).
    // deviceCount is the number of new devices found in this cycle.
    void discoveryComplete(int deviceCount);

private slots:
    void onReadyRead();
    void onTimeout();

private:
    static const int    kDiscoveryPort = 6969;
    static const int    kTimeoutMs     = 2000;   // window to collect replies
    static const char  *kDiscoverMsg;

    QUdpSocket         *m_socket;
    QTimer             *m_timer;
    QSet<QString>       m_connectedNames;   // devices already connected
    QSet<QHostAddress>  m_repliedIPs;       // IPs seen in this discovery cycle
    int                 m_newCount;

    void        stop();
    GAACEDeviceInfo parseReply(const QByteArray &data, const QHostAddress &ip);
};
