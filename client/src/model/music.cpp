#include "music.h"
#include <QUuid>
#include <QMediaPlayer>
#include <QMediaMetaData>
#include <QCoreApplication>
Music::Music()
    :m_like(false),m_history(false)
{

}

Music::Music(QUrl url)
    :m_like(false),m_history(false),
    m_url(url)
{
    //感觉还是要了解一下这个Uuid，我感觉他可能只在这个域是唯一的，反正感觉不对
    //或者应该由另一个对象去传uuid，比如一个全局的
    m_id = QUuid::createUuid().toString();
    parseMusicMetaData(url);
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

void Music::parseMusicMetaData(const QUrl &url)
{ 
    QMediaPlayer player;
    player.setMedia(url);
    while(!player.isMetaDataAvailable())
    {
        QCoreApplication::processEvents();
    }
    if(player.isMetaDataAvailable())
    {
        m_name = player.metaData(QMediaMetaData::Title).toString();
        m_author = player.metaData(QMediaMetaData::Author).toString();
        //如果author为空，则“匿名”
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
        QString filename= url.fileName();
        qDebug() <<"id:"<<m_id<< "name:" << m_name << "author:" << m_author << "album:" << m_album << "duration:" << m_duration<< "filename:" << filename;
    }
}