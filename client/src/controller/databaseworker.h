// databaseworker.h
#pragma once
#include <QObject>
#include <QString>
#include <QVariant> // 可以用来传递各种类型的数据
#include <music.h>
class DatabaseWorker : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseWorker(QObject *parent = nullptr);
    ~DatabaseWorker();

public slots:
    // 这个槽函数就是我们任务队列的处理核心
    // 当收到信号时，它会在自己的线程（后台线程）中被执行
    void executeSqlTask(const QString &sql, const QVariant &param);
    void setupDatabase(); // 用于在后台线程中初始化数据库连接
    void onCheckMusic(const QString &mid,const QUrl &url);
signals:
    // 可以定义一些信号，用于通知主线程任务完成了或者出错了
    void taskFinished(bool success, const QString &result);
    void errorOccurred(const QString &error);
    void ifCheckMusic(bool hasNewMusic,const QUrl &url);
    void initMusicInfo(const Music &music);
private:
    // // 数据库连接应该属于这个 Worker，并在其所在线程中创建和使用
    // QSqlDatabase db;
    void getMusicInfo();
};