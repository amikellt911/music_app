#include "listitembox.h"
#include "ui_listitembox.h"

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
