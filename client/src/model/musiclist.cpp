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