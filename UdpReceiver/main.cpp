#include <QCoreApplication>
#include <QDataStream>
#include <QNetworkDatagram>
#include <QThread>
#include <QUdpSocket>
#include <iostream>
#include <sys/socket.h>

int main( int argc, char * argv[] )
{
    QCoreApplication a( argc, argv );

    int sequence = -1;
    int received = 0;

    QThread thread;
    QObject worker;
    worker.moveToThread( &thread );
    thread.start();

    QUdpSocket socket;
    socket.bind( QHostAddress::LocalHost, 7777 );

    //    int n = 40000;
    //    if ( setsockopt( socket.socketDescriptor(), SOL_SOCKET, SO_RCVBUF, &n, sizeof( n ) ) == -1 ) {
    //        std::cout << strerror( errno );
    //    }

    QObject::connect( &socket, &QUdpSocket::readyRead, [&]() {
        while ( socket.hasPendingDatagrams() ) {
            auto datagram = socket.receiveDatagram();
            QMetaObject::invokeMethod( &worker, [&, datagram]() {
                ++received;
                QDataStream s( datagram.data() );
                while ( !s.atEnd() ) {
                    int seq;
                    s >> seq;
                    if ( sequence != -1 && seq != sequence + 1 ) {
                        std::cout << "Data loss at " << sequence << std::endl;
                        std::cout << seq - sequence << " packets lost" << std::endl;
                        std::cout << received << " packets received" << std::endl;
                        throw std::runtime_error( "Data loss" );
                    } else {
                        sequence = seq;
                    }
                }
            } );
        }
    } );

    return a.exec();
}
