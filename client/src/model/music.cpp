#include "music.h"
#include <QUuid>
#include <QMediaPlayer>
#include <QMediaMetaData>
#include <QCoreApplication>
#include <QThread>
Music::Music()
    :m_like(false),m_history(false)
{

}

Music::Music(QUrl url, bool parseMetaData)
    :m_like(false),m_history(false),
    m_url(url)
{
    //感觉还是要了解一下这个Uuid，我感觉他可能只在这个域是唯一的，反正感觉不对
    //或者应该由另一个对象去传uuid，比如一个全局的
    m_id = QUuid::createUuid().toString();

    // 立即解析元数据可能会导致阻塞，暂时取消
    // 可以在需要显示音乐信息时再解析
    if (parseMetaData) {
        parseMusicMetaData(url);
    } else {
        // 使用默认值或文件名
        QString filename = url.fileName();
        int lastDot = filename.lastIndexOf('.');
        if (lastDot > 0) {
            filename = filename.left(lastDot);
        }
        m_name = filename;
        m_author = "未知艺术家";
        m_album = "未知专辑";
        m_duration = 0;
    }
}


void Music::setMusicUrl(QUrl url)
{
    m_url = url;
}

void Music::setMusicName(const QString &name)
{
    m_name = name;
}

void Music::setMusicAuthor(const QString &author)
{
    m_author = author;
}

void Music::setMusicAlbum(const QString &album)
{
    m_album = album;
}

void Music::setMusicDuration(qint64 duration)
{
    m_duration = duration;
}

void Music::setMusicLike(bool like)
{
    m_like = like;
}

void Music::setMusicHistory(bool history)
{
    m_history = history;
}

QUrl Music::getMusicUrl() const
{
    return m_url;
}

QString Music::getMusicName() const
{
    return m_name;
}

QString Music::getMusicAuthor() const
{
    return m_author;
}

QString Music::getMusicAlbum() const
{
    return m_album;
}

qint64 Music::getMusicDuration() const
{
    return m_duration;
}

bool Music::getMusicLike() const
{
    return m_like;
}

bool Music::getMusicHistory() const
{
    return m_history;
}

QString Music::getMusicId() const
{
    return m_id;
}

// 修复了死循环问题的元数据解析函数
void Music::parseMusicMetaData(const QUrl &url)
{
    QMediaPlayer player;
    player.setMedia(url);

    // 设置合理的时间限制，避免死循环（最多等待5秒）
    const int MAX_WAIT_TIME = 5000;  // 5秒
    const int CHECK_INTERVAL = 100;  // 每100ms检查一次
    int waitTime = 0;

    // 添加超时机制的循环
    while (!player.isMetaDataAvailable() && waitTime < MAX_WAIT_TIME)
    {
        QCoreApplication::processEvents();
        waitTime += CHECK_INTERVAL;
        QThread::msleep(CHECK_INTERVAL);  // 防止CPU占用过高
    }

    // 检查是否成功获取到元数据
    if (player.isMetaDataAvailable())
    {
        m_name = player.metaData(QMediaMetaData::Title).toString();
        m_author = player.metaData(QMediaMetaData::Author).toString();
        //如果author为空，则"匿名"
        if(m_author.isEmpty())
        {
            m_author = "匿名";
        }
        m_album = player.metaData(QMediaMetaData::AlbumTitle).toString();
        if(m_album.isEmpty())
        {
            m_album = "未知专辑";
        }
        m_duration = player.metaData(QMediaMetaData::Duration).toLongLong();
    }
    else
    {
        // 如果无法获取元数据，使用默认值或文件名
        QString filename = url.fileName();
        int lastDot = filename.lastIndexOf('.');
        if (lastDot > 0) {
            filename = filename.left(lastDot);
        }

        m_name = filename;  // 使用文件名作为歌曲名
        m_author = "未知艺术家";
        m_album = "未知专辑";
        m_duration = 0;  // 未知时长设为0

        qDebug() << "Failed to parse metadata for:" << url.toLocalFile()
                 << "using filename as title";
    }

    QString filename = url.fileName();
    qDebug() <<"id:"<<m_id<< "name:" << m_name << "author:" << m_author
             << "album:" << m_album << "duration:" << m_duration
             << "filename:" << filename;
}