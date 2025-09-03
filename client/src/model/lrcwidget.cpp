#include "lrcwidget.h"
#include "ui_lrcwidget.h"

LrcWidget::LrcWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LrcWidget)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);
    ui->hideBtn->setIcon(QIcon(":/images/hideBtn.png"));
}

LrcWidget::~LrcWidget()
{
    delete ui;
}

void LrcWidget::on_hideBtn_clicked()
{
    this->hide();
}
