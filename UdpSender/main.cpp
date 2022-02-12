#include <QCoreApplication>
#include <QDataStream>
#include <QThread>
#include <QUdpSocket>

int main( int argc, char * argv[] )
{
    QCoreApplication a( argc, argv );

    QUdpSocket socket;

    for ( int i = 0; i < 1000000000; ++i ) {
        QByteArray buffer;
        QDataStream s( &buffer, QIODevice::WriteOnly );
        s << i;
        socket.writeDatagram( buffer, QHostAddress::LocalHost, PORT );
        QThread::currentThread()->usleep( 1 );
    }

    return a.exec();
}
