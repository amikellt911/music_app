#include "musiclist.h"
#include <QMimeDatabase>
#include <QDebug>
MusicList::MusicList()
{

}

void MusicList::addMusicByUrl(const QList<QUrl>& urls)
{
    for (auto url : urls) {

        QMimeDatabase mimeDatabase;
        QMimeType mimeType = mimeDatabase.mimeTypeForFile(url.toLocalFile());
        QString mimeTypeName = mimeType.name();
        if (mimeTypeName.startsWith("audio")) {
            qDebug() << "Audio file:" << url.toLocalFile();
        } else {
            qDebug() << "Not an audio file:" << url.toLocalFile();
        }
        Music music(url);
        musicList.push_back(music);
    }
}