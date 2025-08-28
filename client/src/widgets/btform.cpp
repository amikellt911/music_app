#include "btform.h"
#include "ui_btform.h"

BtForm::BtForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BtForm)
{
    ui->setupUi(this);
}

BtForm::~BtForm()
{
    delete ui;
}

void BtForm::setIconAndText(const QString &icon,const QString &text)
{
    //设置图标
    ui->btIcon->setPixmap(QPixmap(icon));
    //ui->btIcon->setScaledContents(true);
    ui->btText->setText(text);
}
void BtForm::setIcon(const QString &icon)
{
    ui->btIcon->setPixmap(QPixmap(icon));
    //ui->btIcon->setScaledContents(true);
}
void BtForm::mousePressEvent(QMouseEvent *event)
{
    //上一个点击的id通过信号和槽让client发送或者他来处理
    //可以通过发送信号来让client处理
    //1.还原上一个点击的图标，修改上一个text颜色为黑色，还原上一个点击的颜色（可以用一个int->控件的map来记录（在client方便）,在BtForm里面怎么搞？用指针？），所以需要记录上一个点击的按钮，以及最初始的rgb（240,242,246）

    //2.修改点击按钮的颜色（252,61,76），修改textext颜色为白色，修改图标（也让client来处理），但是修改图标感觉要对应才能修改吧
    ui->btStyle->setStyleSheet("#btStyle QLabel {background-color: transparent;}#btStyle{background-color:rgb(252,61,76);}");
    ui->btText->setStyleSheet("QLabel{color:white;}");
    //3.切换页面

    emit btClicked(id);
}

void BtForm::setId(int id)
{
    this->id = id;
}

void BtForm::revertBtForm(const QString &icon)
{
    ui->btStyle->setStyleSheet(
        "#btStyle {"
        "    background-color: rgb(240,242,246);"
        "}"
        "#btStyle:hover {"
        "    background-color: rgba(240, 240, 240, 0.7);"
        "    border: 2px solid #c0c0c0;"
        "}"
        "#btStyle QLabel {"
        "    background-color: transparent;"
        "}"
    );
    ui->btText->setStyleSheet("QLabel{color:black;}");
    setIcon(icon);
}