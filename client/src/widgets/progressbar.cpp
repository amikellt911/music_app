#include "progressbar.h"
#include "ui_progressbar.h"

ProgressBar::ProgressBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProgressBar)
{
    ui->setupUi(this);
}

ProgressBar::~ProgressBar()
{
    delete ui;
}
