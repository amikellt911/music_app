#include "recboxitem.h"
#include "ui_recboxitem.h"
#include <QPropertyAnimation>

RecBoxItem::RecBoxItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecBoxItem)
{
    ui->setupUi(this);

    ui->musicImageBox->installEventFilter(this);
}

RecBoxItem::~RecBoxItem()
{
    delete ui;
}

bool RecBoxItem::eventFilter(QObject *watched, QEvent *event)
{
    if(watched==ui->musicImageBox)
    {
        if(QEvent::Enter==event->type()){
            QPropertyAnimation* animation = new QPropertyAnimation(ui->musicImageBox, "geometry");
            animation->setDuration(150);
            animation->setStartValue(QRect(9,9,ui->musicImageBox->width(),ui->musicImageBox->height()));
            animation->setEndValue(QRect(9,0,ui->musicImageBox->width(),ui->musicImageBox->height()));
            animation->start();
            connect(animation, SIGNAL(finished()), animation, SLOT(deleteLater()));

        }else if(QEvent::Leave==event->type()){
            QPropertyAnimation* animation = new QPropertyAnimation(ui->musicImageBox, "geometry");
            animation->setDuration(150);
            animation->setStartValue(QRect(9,0,ui->musicImageBox->width(),ui->musicImageBox->height()));
            animation->setEndValue(QRect(9,9,ui->musicImageBox->width(),ui->musicImageBox->height()));
            animation->start();
            connect(animation, SIGNAL(finished()), animation, SLOT(deleteLater()));
        }
    }
    return QObject::eventFilter(watched, event);
}

void RecBoxItem::setRecImage(const QString& imagePath)
{
       QString style="background-image: url("+imagePath+");"
                     "background-repeat: no-repeat;"
                     "background-position: center;"
                     "background-size: cover;";
        ui->recMusicImage->setStyleSheet(style);
        //好像没起作用
        ui->recMusicImage->setScaledContents(true);
}
void RecBoxItem::setRecText(const QString& text)
{
    ui->recBoxItemText->setText(text);
}