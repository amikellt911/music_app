#include "commonpage.h"
#include "ui_commonpage.h"
#include "listitembox.h"
CommonPage::CommonPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommonPage)
{
    ui->setupUi(this);
}

CommonPage::~CommonPage()
{
    delete ui;
}

void CommonPage::setCommonPageUi(const QString &text, const QString &imagePath)
{
    ui->commonText->setText(text);
    ui->commonText->setIndent(5);  // 向右缩进 5px,让文字与图片对齐
    ui->commonText->setFont(QFont("Microsoft YaHei", 13));
    ui->cover->setPixmap(QPixmap(imagePath));
    ui->cover->setScaledContents(true);

    //测试添加
    ListItemBox *listItemBox = new ListItemBox(this);
    QListWidgetItem *item = new QListWidgetItem(ui->musicList);
    item->setSizeHint(QSize(listItemBox->width(), listItemBox->height()));
    ui->musicList->setItemWidget(item, listItemBox);
}