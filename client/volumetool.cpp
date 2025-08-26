#include "volumetool.h"
#include "ui_volumetool.h"
#include <QGraphicsDropShadowEffect>
VolumeTool::VolumeTool(QWidget *parent) : QWidget(parent),
                                          ui(new Ui::VolumeTool)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::Popup);
    setAttribute(Qt::WA_TranslucentBackground);
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setOffset(0, 0);
    shadowEffect->setColor("#646464");
    shadowEffect->setBlurRadius(10);
    setGraphicsEffect(shadowEffect);
}

VolumeTool::~VolumeTool()
{
    delete ui;
}
