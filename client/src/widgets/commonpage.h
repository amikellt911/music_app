#ifndef COMMONPAGE_H
#define COMMONPAGE_H

#include <QWidget>
#include <QListWidgetItem>
#include <memory>
#include "musiclist.h"

// 需要 ListItemBox 的前向声明避免循环依赖
class ListItemBox;
namespace Ui {
class CommonPage;
}
enum class PageType {
        like,
        local,
        recent
};
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
    void connectMusicListSignals();
    void setPageType(PageType type);
    PageType getPageType() const;

private slots:
    void on_addLocalBtn_clicked();

    /**
     * @brief 当音乐列表更新时更新UI显示
     * 清空当前列表并重新显示所有音乐项
     */
    void onMusicListUpdated();

    void onMusicLikeUpdated(const QString& musicId, bool isLiked);

signals:
    //void updateMusicListLikes();

private:
    /**
     * @brief 更新音乐列表的UI显示
     * 根据当前的音乐列表数据生成对应的ListItemBox控件
     */
    void updateMusicListDisplay();

    /**
     * @brief 清空当前显示的音乐项
     * 释放之前创建的所有ListItemBox控件和对应的QListWidgetItem
     */
    void clearMusicListDisplay();
    /**
     * @brief 添加音乐项到喜欢列表
     * @param musicId 音乐项的ID
     */
    void addMusicItemToLikeList(const QString& musicId);
    void removeMusicItemFromLikeList(const QString& musicId);

    Ui::CommonPage *ui;
    std::shared_ptr<MusicList> m_musicList;

    // 存储当前显示的音乐项控件，用于清理时释放
    QList<QListWidgetItem*> m_musicListItems;
    QList<ListItemBox*> m_listItemBoxes;
    PageType m_pageType;
    
};

#endif // COMMONPAGE_H
