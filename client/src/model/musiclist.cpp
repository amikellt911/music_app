#include "musiclist.h"
#include <QMimeDatabase>
#include <QDebug>

MusicList::MusicList(QObject *parent)
    : QObject(parent)
{
}

void MusicList::addMusicByUrl(const QList<QUrl>& urls)
{
    bool hasNewMusic = false;

    for (const auto& url : urls) {
        QMimeDatabase mimeDatabase;
        QMimeType mimeType = mimeDatabase.mimeTypeForFile(url.toLocalFile());
        QString mimeTypeName = mimeType.name();

        if (mimeTypeName.startsWith("audio")) {
            qDebug() << "Audio file:" << url.toLocalFile();
            // 创建Music对象，立即解析元数据
            Music music(url);
            musicList.push_back(music);
            hasNewMusic = true;
        } else {
            qDebug() << "Not an audio file:" << url.toLocalFile();
        }
    }

    // 如果有新的音乐被添加，发送更新信号
    if (hasNewMusic) {
        emit musicListUpdated();
    }
}

const QVector<Music>& MusicList::getMusicList() const
{
    return musicList;
}

int MusicList::size() const
{
    return musicList.size();
}

bool MusicList::isEmpty() const
{
    return musicList.isEmpty();
}

void MusicList::updateMusicListLikes(const QString& id,bool like)
{
    for (auto& music : musicList) {
        if (music.getMusicId() == id) {
            music.setMusicLike(like);
            emit musicListLikeUpdated(id,like);
            break;
        }
    }
}

QUrl MusicList::getMusicUrlById(const QString& id) const
{
    for (const auto& music : musicList) {
        if (music.getMusicId() == id) {
            return music.getMusicUrl();
        }
    }
    return QUrl();
}

Music MusicList::getMusicById(const QString& id) const
{
    for (const auto& music : musicList) {
        if (music.getMusicId() == id) {
            return music;
        }
    }
    return Music();
}

QString MusicList::getMusicIdByUrl(const QUrl& url) const
{
    for (const auto& music : musicList) {
        if (music.getMusicUrl() == url) {
            return music.getMusicId();
        }
    }
    return QString();
}