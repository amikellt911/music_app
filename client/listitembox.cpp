#include "listitembox.h"
#include "ui_listitembox.h"
#include <QStyleOption>
#include <QPainter>

ListItemBox::ListItemBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ListItemBox)
{
    ui->setupUi(this);
}

ListItemBox::~ListItemBox()
{
    delete ui;
}

void ListItemBox::enterEvent(QEvent *event)
{
    (void)event;
    setStyleSheet("background-color: #efefef");
}

void ListItemBox::leaveEvent(QEvent *event)
{
    (void)event;
    setStyleSheet("background-color: transparent");
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