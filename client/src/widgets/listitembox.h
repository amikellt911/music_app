#ifndef LISTITEMBOX_H
#define LISTITEMBOX_H

#include <QWidget>

namespace Ui {
class ListItemBox;
}

class ListItemBox : public QWidget
{
    Q_OBJECT

public:
    explicit ListItemBox(QWidget *parent = nullptr);
    ~ListItemBox();

    /**
     * @brief 设置音乐项的数据信息
     * @param name 音乐名称
     * @param author 音乐作者/歌手
     * @param album 专辑名称
     * @param duration 时长（毫秒）
     * @param isLike 是否为喜欢歌曲
     */
    void setMusicInfo(const QString &name, const QString &author,
                     const QString &album, qint64 duration, bool isLike = false);

    /**
     * @brief 设置音乐项的背景色（用于区分选中状态）
     * @param selected 是否选中
     */
    void setSelected(bool selected);

    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::ListItemBox *ui;
};

#endif // LISTITEMBOX_H
