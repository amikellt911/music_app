#ifndef MUSIC_H
#define MUSIC_H
#include <QUrl>

class Music
{
public:
    Music();
    Music(QUrl url);
    //set方法
    void setMusicName(const QString &name);
    void setMusicAuthor(const QString &author);
    void setMusicAlbum(const QString &album);
    void setMusicDuration(qint64 duration);
    void setMusicLike(bool like);
    void setMusicHistory(bool history);
    void setMusicUrl(QUrl url);
    //get方法
    QString getMusicName() const;
    QString getMusicAuthor() const;
    QString getMusicAlbum() const;
    qint64 getMusicDuration() const;
    bool getMusicLike() const;
    bool getMusicHistory() const;
    QUrl getMusicUrl() const;
    QString getMusicId() const;

private:
    //解析音乐媒体元数据
    void parseMusicMetaData(const QUrl &url);
private:
    //音乐唯一性id,uuid
    QString m_id;
    //音乐名称
    QString m_name;
    //音乐作者
    QString m_author;
    //音乐专辑
    QString m_album;
    //音乐时长,qint64=long long
    qint64 m_duration;
    //是否喜欢
    bool m_like;
    //是否为历史播放（后期会改，先暂时看他的）
    bool m_history;
    //音乐路径
    QUrl m_url;
};

#endif // MUSIC_H
