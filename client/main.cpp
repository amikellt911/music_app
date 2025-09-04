#include "client.h"

#include <QApplication>
#include "music.h"
#include <QLocalServer>
#include <QLocalSocket>
#include <QMessageBox>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    const QString serverName="localUniqueServer";
    QLocalSocket socket;
    socket.connectToServer(serverName);
    if(socket.waitForConnected(500)){
        QMessageBox::information(nullptr,"提示","程序已经运行");
        return 0;
    }
    QLocalServer server;
    server.removeServer(serverName);
    if(!server.listen(serverName))
    {
        QMessageBox::critical(nullptr, "错误", "无法启动服务器，程序可能无法正常运行。");
        return 1;
    }

    //自定义类型必须要注册才能同信号传递
    qRegisterMetaType<Music>("Music"); 
    client w;
    w.show();
    return a.exec();
}
