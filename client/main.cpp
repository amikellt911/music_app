#include "client.h"

#include <QApplication>
#include "music.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //自定义类型必须要注册才能同信号传递
    qRegisterMetaType<Music>("Music"); 
    client w;
    w.show();
    return a.exec();
}
