#ifndef MUSICLIST_H
#define MUSICLIST_H

#include <QVector>
#include "music.h"
#include <QUrl>
#include <QList>


class MusicList
{
public:
    MusicList();

    void addMusicByUrl(const QList<QUrl>& urls);
private:
    QVector<Music> musicList;
};

#endif // MUSICLIST_H
