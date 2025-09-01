#include "mediaplayermanager.h"
#include <QDebug>
#include <QMediaContent>

// 【构造函数 - 核心设置点】
MediaPlayerManager::MediaPlayerManager(QObject* parent)
    : QObject(parent)                    // 指定 Qt 父对象
    , m_player(new QMediaPlayer(this))     // 创建播放器，并设置为自己的子对象
    , m_playlist(new QMediaPlaylist(this)) // 创建播放列表
    , m_activePageType(PageType::uninitialized)    // 默认本地音乐页面
{
    // 初始化默认值
    m_player->setPlaylist(m_playlist);      // 可选择性使用播放列表
    m_player->setVolume(0);               // 默认音量 0%

    // 基础连接设置
    setupConnections();
}

MediaPlayerManager::~MediaPlayerManager()
{
    // Qt 父子关系会自动清理资源
    // 你不需要手动 delete
}

// 【信号连接设置 - 核心实现】
void MediaPlayerManager::setupConnections()
{
    // 【信号转换原理】
    // 将 QMediaPlayer 的信号转发到管理层，避免界面层直接监听底层信号

    connect(m_player, &QMediaPlayer::stateChanged,
            this, &MediaPlayerManager::stateChanged);

    connect(m_player, &QMediaPlayer::positionChanged,
            this, [this](qint64 pos) {
                emit positionChanged(pos);
                // 进度条更新接口（预留）
                // TODO: 进度条功能实现时在这里添加处理逻辑
            });

    connect(m_player, &QMediaPlayer::durationChanged,
            this, &MediaPlayerManager::durationChanged);

    connect(m_player, &QMediaPlayer::volumeChanged,
            this, &MediaPlayerManager::volumeChanged);

    // 【错误处理 - 统一化】
    connect(m_player, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error),
            this, [this](QMediaPlayer::Error error) {
                qDebug() << "播放错误:" << error << m_player->errorString();
                emit MediaPlayerManager::error(error);  // 使用类名限定，避免名字冲突
                emit errorMessage(m_player->errorString());

                // 可以在这里添加错误恢复逻辑
                // 比如网络错误时的重试机制
            });

    // 【进度条相关信号预留】
    // TODO: 当进度条功能实现时，连接相应的信号处理
    // connect(m_player, &QMediaPlayer::positionChanged, this, &MediaPlayerManager::onPositionChanged);
    // connect(m_player, &QMediaPlayer::durationChanged, this, &MediaPlayerManager::onDurationChanged);
}

// 【播放方法 - 核心播放逻辑】
void MediaPlayerManager::play(const QUrl& url)
{
    // 处理不同播放场景
    if (!url.isEmpty()) {
        // 如果提供了有效的 URL，播放特定歌曲
        m_player->setMedia(QMediaContent(url));
        m_player->play();
        emit recentPlaySignal(url);
    } else {
        // 如果没有提供 URL，且有播放列表，则播放当前曲目
        if (!m_playlist->isEmpty()) {
            m_player->play();
            emit recentPlaySignal(m_playlist->currentMedia().request().url());
        } else {
            qDebug() << "播放失败：无播放内容";
        }
    }

    // 可以在这里添加：
    // - 播放统计
    // - 播放历史记录
    qDebug() << "开始播放：" << (url.isEmpty() ? "播放列表中当前歌曲" : url.toString());
}

// 【暂停方法】
void MediaPlayerManager::pause()
{
    if (m_player->state() == QMediaPlayer::PlayingState) {
        m_player->pause();
        qDebug() << "播放已暂停";
    }
}

// 【恢复播放方法】
void MediaPlayerManager::resume()
{
    if (m_player->state() == QMediaPlayer::PausedState) {
        m_player->play();
        qDebug() << "播放已恢复";
    }
}

// 【播放上一首】
void MediaPlayerManager::playPrevious()
{
    if (!m_playlist->isEmpty()) {
        m_playlist->previous();
        qDebug() << "播放上一首";
    }
}

// 【播放下一首】
void MediaPlayerManager::playNext()
{
    if (!m_playlist->isEmpty()) {
        m_playlist->next();
        qDebug() << "播放下一首";
    }
}

// 【设置播放模式】
void MediaPlayerManager::setPlaybackMode(const QString& mode)
{
    if (mode == "顺序播放") {
        m_playlist->setPlaybackMode(QMediaPlaylist::Sequential);
        qDebug() << "设置播放模式：顺序播放";
    } else if (mode == "随机播放") {
        m_playlist->setPlaybackMode(QMediaPlaylist::Random);
        qDebug() << "设置播放模式：随机播放";
    } else if (mode == "单曲循环") {
        m_playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
        qDebug() << "设置播放模式：单曲循环";
    } else if (mode == "列表循环") {
        m_playlist->setPlaybackMode(QMediaPlaylist::Loop);
        qDebug() << "设置播放模式：列表循环";
    }
}

// 【循环切换播放模式 - 无参，自动切换到下一个模式】
void MediaPlayerManager::togglePlaybackMode()
{
    QMediaPlaylist::PlaybackMode currentMode = m_playlist->playbackMode();
    QString nextModeName;

    // 循环切换：顺序播放 → 随机播放 → 单曲循环 → 列表循环 → 顺序播放
    switch (currentMode) {
        case QMediaPlaylist::Sequential:
            m_playlist->setPlaybackMode(QMediaPlaylist::Random);
            nextModeName = "随机播放";
            break;
        case QMediaPlaylist::Random:
            m_playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
            nextModeName = "单曲循环";
            break;
        case QMediaPlaylist::CurrentItemInLoop:
            m_playlist->setPlaybackMode(QMediaPlaylist::Loop);
            nextModeName = "列表循环";
            break;
        default: // 其他模式（包括 Loop）都切换到顺序播放
            m_playlist->setPlaybackMode(QMediaPlaylist::Sequential);
            nextModeName = "顺序播放";
            break;
    }

    qDebug() << "循环切换播放模式：" << getPlaybackMode() << " → " << nextModeName;
}

// 【获取当前播放模式】
QString MediaPlayerManager::getPlaybackMode() const
{
    QMediaPlaylist::PlaybackMode mode = m_playlist->playbackMode();
    switch (mode) {
        case QMediaPlaylist::Sequential:
            return "顺序播放";
        case QMediaPlaylist::Random:
            return "随机播放";
        case QMediaPlaylist::CurrentItemInLoop:
            return "单曲循环";
        case QMediaPlaylist::Loop:
            return "列表循环";
        default:
            return "顺序播放";
    }
}

// 【获取当前播放时间 - 明确的方法名】
qint64 MediaPlayerManager::getCurrentPlaybackTime() const
{
    return m_player->position();
}

// 【音量控制】
void MediaPlayerManager::setVolume(int volume)
{
    // 确保音量值在合理范围内
    volume = qBound(0, volume, 100);
    m_player->setVolume(volume);
}

// 【设置播放列表 - 核心功能】
void MediaPlayerManager::setPlaylist(const QVector<QUrl>& playlist, int index)
{
    if (playlist.isEmpty()) {
        qDebug() << "播放列表为空，无法设置";
        return;
    }

    // 更新活跃播放列表
    m_activateList = playlist;

    // 清空当前的播放列表
    m_playlist->clear();

    // 添加新的音乐到播放列表
    for (const QUrl& url : playlist) {
        m_playlist->addMedia(QMediaContent(url));
    }

    // 设置当前播放的索引（如果在合理的范围内）
    if (index >= 0 && index < playlist.size()) {
        m_playlist->setCurrentIndex(index);
        qDebug() << "设置播放列表成功，当前播放索引：" << index << "，总歌曲数：" << playlist.size();
    } else {
        m_playlist->setCurrentIndex(0); // 默认播放第一首
        qDebug() << "设置播放列表成功，开始播放第一首歌曲，总歌曲数：" << playlist.size();
    }

    // 设置播放模式为顺序播放（可以后续扩展为其他模式）
    m_playlist->setPlaybackMode(QMediaPlaylist::Sequential);
}

// 【设置当前播放索引 - 只切换歌曲，不重建列表】
void MediaPlayerManager::setCurrentIndex(int index)
{
    // 检查播放列表是否存在
    if (m_playlist->isEmpty()) {
        qDebug() << "播放列表为空，无法设置播放索引";
        return;
    }

    // 检查索引的有效性
    if (index < 0 || index >= m_playlist->mediaCount()) {
        qDebug() << "播放索引超出范围：" << index << "，列表大小：" << m_playlist->mediaCount();
        return;
    }

    // 设置初始的索引
    int oldIndex = m_playlist->currentIndex();
    m_playlist->setCurrentIndex(index);

    qDebug() << "切换播放索引：" << oldIndex << " → " << index;

    // 如果播放器处于播放状态，立即切换到新歌曲
    if (m_player->state() == QMediaPlayer::PlayingState) {
        qDebug() << "正在播放，自动切换到新歌曲";
        // QMediaPlayer 会在设置 new index 后自动开始播放新歌曲
    }
}

// 【状态查询接口实现】
QMediaPlayer::State MediaPlayerManager::state() const
{
    return m_player->state();
}

qint64 MediaPlayerManager::duration() const
{
    return m_player->duration();
}

qint64 MediaPlayerManager::position() const
{
    return m_player->position();
}

int MediaPlayerManager::volume() const
{
    return m_player->volume();
}

// 【进度条相关方法预留 - 将来实现】
// TODO: 进度条功能实现时完善这些方法

// 进度条拖拽功能（预留接口）
// void MediaPlayerManager::setPosition(qint64 position);
// 进度条显示更新（预留槽函数）
// void MediaPlayerManager::onProgressSliderValueChanged(int value);
// void MediaPlayerManager::onDurationChanged(qint64 duration);

// 【页面类型管理】
void MediaPlayerManager::setActivePageType (PageType  type)
{
    m_activePageType  = type;
    qDebug() << "设置活跃页面类型为：" << static_cast<int>(type);
}

PageType  MediaPlayerManager::getActivePageType () const
{
    return m_activePageType ;
}

// 【智能播放方法 - 根据页面类型自动判断策略】
void MediaPlayerManager::playSongIndexed(PageType  PageType , int index, const QVector<QUrl>& playlist)
{
    // 检查页面类型是否变化
    bool Changed = (PageType  != m_activePageType );

    if (Changed) {
        // 页面类型变化：需要重建设置播放列表
        if (!playlist.isEmpty()) {
            setPlaylist(playlist, index);
            m_activePageType  = PageType ;  // 更新活跃页面类型
            play();  // 开始播放
            qDebug() << "页面类型变化，重建设置播放列表并播放";
        } else {
            qDebug() << "页面类型变化但播放列表为空，跳过操作";
        }
    } else {
        // 页面类型相同：只需要切换索引
        setCurrentIndex(index);
        qDebug() << "页面类型相同，只切换索引";
    }
}

// 【公共访问器 - 紧急出口】
// 尽量避免使用这个方法，应该通过封装接口访问
QMediaPlayer* MediaPlayerManager::player()
{
    return m_player;
}

// 【播放列表相关的辅助方法（内部使用）】
QVector<QUrl> MediaPlayerManager::getActiveList() const
{
    return m_activateList;
}

int MediaPlayerManager::currentIndex() const
{
    return m_playlist->currentIndex();
}

int MediaPlayerManager::playlistSize() const
{
    return m_playlist->mediaCount();
}

// 【带页面类型的播放列表设置】
void MediaPlayerManager::setPlaylistWithPageType(const QVector<QUrl>& playlist, int index, PageType pageType)
{
    // 设置播放列表
    setPlaylist(playlist, index);

    // 更新页面类型
    setActivePageType(pageType);

    qDebug() << "设置播放列表并更新页面类型：" << static_cast<int>(pageType);
}
