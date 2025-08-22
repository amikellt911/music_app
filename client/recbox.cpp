#include "recbox.h"
#include "ui_recbox.h"
#include "recboxitem.h"
RecBox::RecBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecBox),
    row(1),
    col(4)
{
    ui->setupUi(this);
    
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
        col=8;//???,他之后好像会解释
    } else
    {
        ui->recListDown->hide();
    }
    imageList=data;
    createRecBoxItem();
}

void RecBox::createRecBoxItem()
{ 
    for(int i=0;i<col;++i){
        RecBoxItem* item=new RecBoxItem();
        QJsonObject obj=imageList[i].toObject();
        item->setRecImage(obj["path"].toString());
        item->setRecText(obj["text"].toString());
        ui->recListUpLayout->addWidget(item);
    }
}