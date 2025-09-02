#ifndef MEDIAPLAYERMANAGER_H
#define MEDIAPLAYERMANAGER_H

#include <QObject>
#include <QMediaPlayer>      // Qt 的媒体播放器
#include <QMediaPlaylist>     // 播放列表（未来扩展用）
#include <QUrl>              // 资源定位器
#include <memory>            // 智能指针库

// 使用 CommonPage.h 中的 PageType，保持类型一致性
#include "commonpage.h"

class MediaPlayerManager : public QObject
{
    Q_OBJECT

public:
    // 【构造函数教学】
    // explicit 防止隐式类型转换，避免意外构造
    // parent 参数用于 Qt 的父子关系管理
    explicit MediaPlayerManager(QObject* parent = nullptr);
    
    // 【析构函数教学】
    // 必须定义，甚至可能是 virtual，清理资源
    ~MediaPlayerManager();
    
    // 【播放控制接口 - 教学重点】
    // 只暴露最核心的功能，隐藏实现细节
    //如果真的传入url，并且url不为空，那么就播放url歌曲，否则则播放内置的playlist的播放
    void play(const QUrl& url = QUrl());  // 支持不传参数的暂停恢复
    //void stop();stop是完全的停止，即释放资源，我们的项目不需要
    //暂停
    void pause();
    //从暂停恢复，play是重新开始播放
    void resume();
    void setVolume(int volume);          // 音量控制

    // 进度控制 - 设置播放位置
    void setPosition(qint64 position);    // 设置播放进度位置

    //设置音乐列表(先设置列表，然后再把列表传入playlist),通常是双击的时候或者是点击播放全部，会改变列表，当做槽函数
    //如果是 playBtn那么index默认为0，否则就要传入index
    void setPlaylist(const QVector<QUrl>& playlist,int index=0);
    // 同时设置页面类型，用于智能判断
    void setPlaylistWithPageType(const QVector<QUrl>& playlist, int index, PageType pageType);

    //只设置播放索引，不改变播放列表(用于同一页面内切换歌曲)
    void setCurrentIndex(int index);

    // 播放控制 - 上一首/下一首
    void playPrevious();    // 播放上一首
    void playNext();       // 播放下一首

    // 播放模式控制 - 切换和获取
    void setPlaybackMode(const QString& mode);  // 设置播放模式
    void togglePlaybackMode();                  // 循环切换播放模式 (无参)
    QString getPlaybackMode() const;            // 获取当前播放模式

    // 时间相关 - 获取播放时间
    qint64 getCurrentPlaybackTime() const;      // 获取当前播放时间 (更明确的方法)

    // 页面类型管理
    void setActivePageType(PageType type);
    PageType getActivePageType() const;

    // 智能播放方法：根据页面类型自动判断是否重建列表
    void playSongIndexed(PageType pageType, int index, const QVector<QUrl>& playlist = QVector<QUrl>());

    // 【状态查询接口 - 教学重点】
    // 提供所有必要的状态查询，界面层不需了解内部实现
    QMediaPlayer::State state() const;     // 播放状态
    qint64 duration() const;               // 总时长
    qint64 position() const;               // 当前播放位置
    int volume() const;                    // 当前音量

    // 【高级控制接口 - 教学重点】
    // 必要时提供直接访问，但注释说明这是紧急出口
    // 不是常用接口，主要是为特殊情况预留
    QMediaPlayer* player();                // 返回内部播放器对象

    // 辅助方法 - 获取当前播放列表信息
    QVector<QUrl> getActiveList() const;
    int currentIndex() const;
    int playlistSize() const;
    
signals:
    // 【信号设计教学】
    // 将内部信号转换为管理层合伙
    // 避免界面层直接监听底层的混乱信号
    void stateChanged(QMediaPlayer::State state);
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void volumeChanged(int volume);
    
    // 【错误处理教学】
    // 统一错误处理和传递
    void error(QMediaPlayer::Error error);
    void errorMessage(const QString& message);

    void recentPlaySignal(const QUrl& url);

private:
    // 【私有成员教学】
    // 使用智能指针保证生命周期安全
    QMediaPlayer* m_player;       // 主播放器
    QMediaPlaylist* m_playlist;   // 播放列表，预留未来扩展
    //当前活跃的音乐列表
    QVector<QUrl> m_activateList;
    // 当前活跃的页面类型
    PageType  m_activePageType ;
    
    // 【私有方法教学】
    // 分割复杂逻辑，保持接口简洁
    void setupConnections();      // 设置信号槽连接

    
    // 【设计哲学】
    // 1. 所有成员变量都是私有的
    // 2. 只通过封装的方法访问内部对象
    // 3. 信号统一化，避免多个界面监听原始信号
};

#endif // MEDIAPLAYERMANAGER_H
