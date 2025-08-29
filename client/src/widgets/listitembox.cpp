#include "listitembox.h"
#include "ui_listitembox.h"
#include <QStyleOption>
#include <QPainter>

ListItemBox::ListItemBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ListItemBox),
    m_isLike(false)
{
    ui->setupUi(this);
}

ListItemBox::~ListItemBox()
{
    delete ui;
}

// void ListItemBox::enterEvent(QEvent *event)
// {
//     (void)event;
//     setStyleSheet("background-color: #ffffff");
// }

// void ListItemBox::leaveEvent(QEvent *event)
// {
//     (void)event;
//     setStyleSheet("background-color: rgb(247,249,252)");
// }

/**
 * @brief 设置音乐项的数据信息
 * 将音乐的各个信息填充到对应的UI元素中
 */
void ListItemBox::setMusicInfo(const QString &name, const QString &author,
                              const QString &album, qint64 duration, bool isLike)
{
    ui->nameLabel->setText(name);
    ui->singerLabel->setText(author);
    ui->albumLabel->setText(album);
    m_isLike = isLike;
    // 设置喜欢按钮状态
    if (isLike) {
        ui->likeBtn->setIcon(QIcon(":/images/likes.png"));
    } else {
        ui->likeBtn->setIcon(QIcon(":/images/like.png"));
    }

    // 可以根据需要显示时长信息
    // 这里暂时不处理时长显示，可以根据UI设计决定在哪里显示
}

/**
 * @brief 设置音乐项的背景色（用于区分选中状态）
 * @param selected 是否显示选中状态
 */
void ListItemBox::setSelected(bool selected)
{
    if (selected) {
        setStyleSheet("#ListItemBox { background-color: #ffffff; border: 1px solid #1890ff; }");
    } else {
        setStyleSheet("#ListItemBox { background-color: transparent; border: none; }");
    }
}

//widget是一个透明的画布，用来承载容器，但是他本身没有画笔，所以需要我们给他提供
void ListItemBox::paintEvent(QPaintEvent *event)
{
    //同理void(event)，不使用的意思
    Q_UNUSED(event);
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
void ListItemBox::on_likeBtn_clicked()
{
    m_isLike=!m_isLike;
    if(m_isLike)
    {
        ui->likeBtn->setIcon(QIcon(":/images/likess.png"));
    }
    else
    {
        ui->likeBtn->setIcon(QIcon(":/images/like.png"));
    }
    emit likeBtnClicked(m_isLike);
}

void ListItemBox::setId(const QString &id)
{
    m_id=id;
}

QString ListItemBox::getId() const
{
    return m_id;
}

void ListItemBox::setLikeState(bool isLiked)
{
    m_isLike = isLiked;  // 更新内部状态
    
    // 更新UI显示（比如按钮的样式）
    if (ui->likeBtn) {  // 假设您有一个点赞按钮
        // 更新按钮的图标或样式来反映新的点赞状态
        if (isLiked) {
            ui->likeBtn->setIcon(QIcon(":/images/likess.png"));
        } else {
            ui->likeBtn->setIcon(QIcon(":/images/like.png"));
        }
    }
}

bool ListItemBox::getIsLike() const
{
    return m_isLike;
}