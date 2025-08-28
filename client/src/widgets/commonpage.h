#ifndef COMMONPAGE_H
#define COMMONPAGE_H

#include <QWidget>
#include <memory>
#include "musiclist.h"
namespace Ui {
class CommonPage;
}

class CommonPage : public QWidget
{
    Q_OBJECT

public:
    explicit CommonPage(QWidget *parent = nullptr);
    ~CommonPage();

    void setCommonPageUi(const QString &text, const QString &imagePath);
    void setAddLocalIcon(const QString &imagePath);
    void setAddLocalIconUnused();
    void setAddLocalIconHover();
    void setMusicList(const std::shared_ptr<MusicList>& musicList);
private slots:
    void on_addLocalBtn_clicked();

private:
    Ui::CommonPage *ui;
    std::shared_ptr<MusicList> m_musicList;
};

#endif // COMMONPAGE_H
