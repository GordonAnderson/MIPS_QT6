#include "GAACEDiscovery.h"
#include <QNetworkDatagram>
#include <QNetworkInterface>

#ifdef Q_OS_WIN
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

const char *GAACEDiscovery::kDiscoverMsg = "GAACE-DISCOVER";

GAACEDiscovery::GAACEDiscovery(QObject *parent)
    : QObject(parent)
    , m_socket(new QUdpSocket(this))
    , m_timer(new QTimer(this))
    , m_newCount(0)
{
    m_timer->setSingleShot(true);
    m_timer->setInterval(kTimeoutMs);

    connect(m_timer,  &QTimer::timeout,        this, &GAACEDiscovery::onTimeout);
    connect(m_socket, &QUdpSocket::readyRead,  this, &GAACEDiscovery::onReadyRead);
}

GAACEDiscovery::~GAACEDiscovery()
{
    stop();
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void GAACEDiscovery::start()
{
    stop();
    m_repliedIPs.clear();
    m_newCount = 0;

    // Find the first active non-loopback IPv4 interface with a broadcast address
    QHostAddress bindAddr    = QHostAddress::AnyIPv4;
    QHostAddress broadcastAddr = QHostAddress::Broadcast;

    for (const QNetworkInterface &iface : QNetworkInterface::allInterfaces())
    {
        if (iface.flags() & QNetworkInterface::IsLoopBack) continue;
        if (!(iface.flags() & QNetworkInterface::IsUp))    continue;
        if (!(iface.flags() & QNetworkInterface::IsRunning)) continue;
        if (!(iface.flags() & QNetworkInterface::CanBroadcast)) continue;

        for (const QNetworkAddressEntry &entry : iface.addressEntries())
        {
            if (entry.ip().protocol() != QAbstractSocket::IPv4Protocol) continue;
            if (entry.ip().isLoopback()) continue;
            if (entry.broadcast().isNull()) continue;

            bindAddr     = entry.ip();
            broadcastAddr = entry.broadcast();
            qDebug("GAACEDiscovery: using interface %s  bind=%s  broadcast=%s",
                   qPrintable(iface.name()),
                   qPrintable(bindAddr.toString()),
                   qPrintable(broadcastAddr.toString()));
            goto found;   // break out of both loops
        }
    }
found:

    if (!m_socket->bind(bindAddr, 0,
                        QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint))
    {
        qWarning("GAACEDiscovery: bind failed: %s",
                 qPrintable(m_socket->errorString()));
        emit discoveryComplete(0);
        return;
    }

    // Set SO_BROADCAST on the native socket
    int broadcastEnabled = 1;
    setsockopt(m_socket->socketDescriptor(), SOL_SOCKET, SO_BROADCAST,
               &broadcastEnabled, sizeof(broadcastEnabled));

    qint64 sent = m_socket->writeDatagram(
        kDiscoverMsg,
        static_cast<qint64>(strlen(kDiscoverMsg)),
        broadcastAddr,
        kDiscoveryPort
        );

    if (sent < 0)
    {
        qWarning("GAACEDiscovery: broadcast failed: %s",
                 qPrintable(m_socket->errorString()));
        stop();
        emit discoveryComplete(0);
        return;
    }

    qDebug("GAACEDiscovery: broadcast sent to %s:%d",
           qPrintable(broadcastAddr.toString()), kDiscoveryPort);

    m_timer->start();
}

void GAACEDiscovery::markConnected(const QString &name)
{
    m_connectedNames.insert(name);
}

void GAACEDiscovery::markDisconnected(const QString &name)
{
    m_connectedNames.remove(name);
}

// ---------------------------------------------------------------------------
// Private slots
// ---------------------------------------------------------------------------

void GAACEDiscovery::onReadyRead()
{
    if (m_socket->state() != QAbstractSocket::BoundState) return;
    if (m_socket->bytesAvailable() <= 0) return;

    while (m_socket->hasPendingDatagrams())
    {
        QNetworkDatagram datagram = m_socket->receiveDatagram();
        if (!datagram.isValid()) continue;

        QHostAddress senderIP = datagram.senderAddress();

        // Deduplicate: ignore a second reply from the same IP in one cycle.
        // (A device could reply twice if it has multiple network interfaces.)
        if (m_repliedIPs.contains(senderIP)) continue;
        m_repliedIPs.insert(senderIP);

        // Parse the reply into a device info struct.
        GAACEDeviceInfo info = parseReply(datagram.data(), senderIP);

        // Validate: must at minimum have a name and a valid port.
        if (info.name.isEmpty() || info.port <= 0 || info.port > 65535)
        {
            qWarning("GAACEDiscovery: ignoring malformed reply from %s",
                     qPrintable(senderIP.toString()));
            continue;
        }

        // Skip devices we are already connected to.
        if (m_connectedNames.contains(info.name)) continue;

        m_newCount++;

        // ----------------------------------------------------------------
        // TODO: open TCP connection to info.ip : info.port here.
        //
        // Example (fill in with your MIPS connection manager call):
        //
        //   MyTcpConnection *conn = new MyTcpConnection(this);
        //   conn->connectToDevice(info.ip, info.port);
        //
        //   // Once connected, call:
        //   markConnected(info.name);
        //
        //   // When the connection drops, call:
        //   markDisconnected(info.name);
        //
        // The info struct contains everything needed:
        //   info.name     — device name    (e.g. "Gauge-A")
        //   info.type     — device type    (e.g. "vacuum_gauge")
        //   info.version  — firmware ver   (e.g. "1.2.0")
        //   info.ip       — QHostAddress   (resolved, ready to connect)
        //   info.port     — int            (TCP command port, default 23)
        // ----------------------------------------------------------------

        emit newDeviceFound(info);
    }
}

void GAACEDiscovery::onTimeout()
{
    stop();
    emit discoveryComplete(m_newCount);
}

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

void GAACEDiscovery::stop()
{
    m_timer->stop();
    m_socket->close();
}

//
// parseReply
//
// Expected reply format (newline-delimited, as sent by the firmware):
//
//   GAACE-DEVICE
//   NAME,Gauge-A
//   TYPE,vacuum_gauge
//   VERSION,1.2.0
//   PORT,23
//
GAACEDeviceInfo GAACEDiscovery::parseReply(const QByteArray  &data,
                                           const QHostAddress &ip)
{
    GAACEDeviceInfo info;
    info.ip   = ip;
    info.port = 23;   // default; overridden if PORT line is present

    const QList<QByteArray> lines = data.split('\n');

    // First line must be the header — reject anything that doesn't look right.
    if (lines.isEmpty() || lines.first().trimmed() != "GAACE-DEVICE")
        return info;   // name will be empty; caller validates

    for (int i = 1; i < lines.size(); ++i)
    {
        const QByteArray line = lines.at(i).trimmed();
        if (line.isEmpty()) continue;

        const int comma = line.indexOf(',');
        if (comma < 0) continue;

        const QString key   = QString::fromUtf8(line.left(comma)).toUpper();
        const QString value = QString::fromUtf8(line.mid(comma + 1));

        if      (key == "NAME")    info.name    = value;
        else if (key == "TYPE")    info.type    = value;
        else if (key == "VERSION") info.version = value;
        else if (key == "PORT")    info.port    = value.toInt();
    }

    return info;
}
