#include "recbox.h"
#include "ui_recbox.h"
#include "recboxitem.h"
#include <QDebug>
RecBox::RecBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecBox),
    row(1),
    col(5),
    currentPage(0)
{
    ui->setupUi(this);
    //当命名符合规范时，setupui会自动连接槽函数，所以此时再次手动connect
    //那么点击按钮，槽函数会执行两次
    // connect(ui->btLeft,&QPushButton::clicked,this,&RecBox::on_btLeft_clicked);
    // connect(ui->btRight,&QPushButton::clicked,this,&RecBox::on_btRight_clicked);
}

RecBox::~RecBox()
{
    delete ui;
}

void RecBox::initRecBoxUi(QJsonArray data, int row)
{
    if(row==2)
    {
        this->row = row;
        //col=8;//???,他之后好像会解释
        //col=4;//我感觉他是个脑瘫，没感觉出任何优点，纯纯更加麻烦了
    } else
    {
        ui->recListDown->hide();
    }
    imageList=data;
    totalPage=imageList.size()/(col*row);
    //qDebug()<<"totalPage:"<<totalPage;
    updateCarousel();
}

void RecBox::updateCarousel()
{ 
    QLayoutItem* child=nullptr;
    while((child=ui->recListUpLayout->takeAt(0))!=nullptr){
        if(child->widget())
        {
            child->widget()->deleteLater();
        }
        delete child;
    }
    while((child=ui->recListDownLayout->takeAt(0))!=nullptr){
        if(child->widget())
        {
            child->widget()->deleteLater();
        }
        delete child;
    }
    for(int j=0;j<row;++j)
    {
        for(int i=0;i<col;++i)
        {
            RecBoxItem* item=new RecBoxItem();
            QJsonObject obj=imageList[currentPage*col*row+i+j*col].toObject();
            item->setRecImage(obj["path"].toString());
            item->setRecText(obj["text"].toString());
            if(j==0)
            {
                ui->recListUpLayout->addWidget(item);
            }//必须两行不一样，因为qt的每个实例控件是唯一的，不能重复使用
            else//如果继续addWidget同一个item，会把上面up的item给remove掉
            {//然后加入down的
                ui->recListDownLayout->addWidget(item);
            }
        }
    }
    ui->recListUpLayout->addStretch();
    if(row==2)
    ui->recListDownLayout->addStretch();
}


void RecBox::on_btLeft_clicked()
{
    if(currentPage==0)
    {
        currentPage=totalPage-1;
    } else
    {
        --currentPage;
    }
    //qDebug()<<"currentPage:"<<currentPage;
    updateCarousel();
}

void RecBox::on_btRight_clicked()
{
    if(currentPage==totalPage-1)
    {
        currentPage=0;
    } else
    {
        ++currentPage;
    }
    //qDebug()<<"currentPage:"<<currentPage;
    updateCarousel();
}
