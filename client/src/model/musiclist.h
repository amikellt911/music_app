#ifndef MUSICLIST_H
#define MUSICLIST_H

#include <QVector>
#include <QObject>
#include "music.h"
#include <QUrl>
#include <QList>

/**
 * @brief 音乐列表管理类
 * 负责音乐文件的添加和管理，提供信号机制通知UI更新
 */
class MusicList : public QObject
{
    Q_OBJECT

public:
    explicit MusicList(QObject *parent = nullptr);

    /**
     * @brief 通过URL列表添加音乐文件
     * @param urls 要添加的音乐文件URL列表
     */
    void addMusicByUrl(const QList<QUrl>& urls);

    /**
     * @brief 获取当前音乐列表
     * @return 返回音乐列表的const引用
     */
    const QVector<Music>& getMusicList() const;

    /**
     * @brief 获取音乐数量
     * @return 当前音乐列表的大小
     */
    int size() const;

    /**
     * @brief 检查音乐列表是否为空
     * @return 如果为空返回true，否则返回false
     */
    bool isEmpty() const;

signals:
    /**
     * @brief 音乐列表更新信号
     * 当音乐列表发生变化时发出此信号
     */
    void musicListUpdated();

private:
    QVector<Music> musicList;
};

#endif // MUSICLIST_H
