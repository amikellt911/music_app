#include "recboxitem.h"
#include "ui_recboxitem.h"

RecBoxItem::RecBoxItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecBoxItem)
{
    ui->setupUi(this);
}

RecBoxItem::~RecBoxItem()
{
    delete ui;
}
