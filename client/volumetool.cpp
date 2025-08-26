#include "volumetool.h"
#include "ui_volumetool.h"
#include <QPainter>
#include <QPainterPath>
#include <math.h>
#include <QDebug>
#include <QGraphicsDropShadowEffect>
VolumeTool::VolumeTool(QWidget *parent) 
    : QWidget(parent), ui(new Ui::VolumeTool)
{
    ui->setupUi(this);
    
    // 设置窗口属性
    // setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::Popup);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);  // 显示时不激活窗口
    // 移除 QGraphicsDropShadowEffect，改用自定义绘制
    // QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    // shadowEffect->setOffset(0, 0);
    // shadowEffect->setColor("#646464");
    // shadowEffect->setBlurRadius(10);
    // setGraphicsEffect(shadowEffect);
    
    // 为阴影预留空间
    int shadowMargin = 10;
    setContentsMargins(shadowMargin, shadowMargin, shadowMargin, shadowMargin);
    // ui->outSlider->setAttribute(Qt::WA_TransparentForMouseEvents); // 不拦鼠标
    // ui->outSlider->setAttribute(Qt::WA_TranslucentBackground);     // 背景透明
    initSliderBtnShadow();

}

void VolumeTool::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    
    // === 阴影参数 ===
    const int shadowBlur = 8;       // 模糊半径（小控件用较小值）
    const int shadowOffset = 2;     // 阴影偏移
    const int cornerRadius = 6;     // 圆角半径
    const QColor shadowColor(100, 100, 100, 40);  // 灰色阴影，透明度40
    
    // === 计算绘制区域 ===
    QRect widgetRect = rect();
    QRect contentRect = widgetRect.adjusted(shadowBlur, shadowBlur, 
                                           -shadowBlur, -shadowBlur);
    QRect shadowRect = contentRect.adjusted(shadowOffset, shadowOffset, 
                                           shadowOffset, shadowOffset);
    
    // === 绘制阴影 ===
    for (int i = 0; i < shadowBlur; ++i) {
        // 使用平方根衰减，让外层更淡
        qreal factor = 1.0 - sqrt((qreal)i / shadowBlur);
        qreal alpha = shadowColor.alpha() * factor;
        
        QColor layerColor = shadowColor;
        layerColor.setAlpha((int)alpha);
        
        QRect layerRect = shadowRect.adjusted(-i, -i, i, i);
        painter.setBrush(QBrush(layerColor));
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(layerRect, cornerRadius + i, cornerRadius + i);
    }
    
    // === 绘制主背景 ===
    QPainterPath mainPath;
    mainPath.addRoundedRect(contentRect, cornerRadius, cornerRadius);
    
    // 设置背景色（白色半透明）
    painter.fillPath(mainPath, QBrush(QColor(255, 255, 255, 240)));
    
    // 绘制边框
    painter.setPen(QPen(QColor(180, 180, 180), 1));
    painter.drawPath(mainPath);
    
    QWidget::paintEvent(event);
}

VolumeTool::~VolumeTool()
{
    delete ui;
}

void VolumeTool::setVolumeRatio(int value)
{
    ui->volumeRatio->setText(QString::number(value)+"%");
}

void VolumeTool::setOutSlider(int value)
{
    int fullHeight = ui->inSlider->height();
    int h = value * 0.01 * fullHeight;
    int y = ui->inSlider->y() + (fullHeight - h); // 让底部对齐
    ui->outSlider->setGeometry(ui->inSlider->x(),
                               y,
                               ui->inSlider->width(),
                               h);
    ui->outSlider->raise(); // 确保在最上层
    ui->sliderBtn->raise();//不然会被滑动条覆盖
    // qDebug() << "inSlider h=" << ui->inSlider->height()
    //      << " outSlider h=" << h
    //      << " y=" << y;

}

void VolumeTool::initSliderBtnShadow()
{
    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(this);
    effect->setOffset(0, 0);          // 不偏移，居中
    effect->setBlurRadius(8);         // 模糊程度
    effect->setColor(QColor(0, 0, 0, 120)); // 半透明黑
    ui->sliderBtn->setGraphicsEffect(effect);

}