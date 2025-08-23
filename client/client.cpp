#include "client.h"
#include "./ui_client.h"
#include <QMouseEvent>
#include <QTimer>
#include <QApplication>
#include <QDebug>
#include <QGraphicsDropShadowEffect>

client::client(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::client)
    , m_dragging(false)
    , m_longPress(false)
    , m_longPressTimer(nullptr)
    , lastBtFormId(0)
{
    ui->setupUi(this);
    initUi();


}

client::~client()
{
    delete ui;
}

void client::initUi(){
    this->setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    initLongPressTimer();
    initShadow();
    initQss();
    initImagesHash();
    initBtForm();
    initConnect();
    //图片按照时间随机
    srand(time(NULL));
    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->recMusicBox->initRecBoxUi(initRandomPicture(),1);
    ui->supplyMusicBox->initRecBoxUi(initRandomPicture(),2);

}

void client::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        //鼠标相对整个屏幕的坐标
        m_pressPosition = event->globalPos();

        // 启动长按定时器
        //start只会实例化一次，只有start只会重置500ms
        m_longPressTimer->start();

        // 如果已经在拖拽状态，则更新位置
        if (m_dragging) {
            // 获取鼠标相对窗口的坐标
            m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        }
    }
    //执行父类函数
    QWidget::mousePressEvent(event);
}

void client::mouseMoveEvent(QMouseEvent *event)
{
    // 如果是长按状态，则开始拖拽
    if (m_longPress && !m_dragging) {
        m_dragging = true;
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        //告诉用户正在拖拽窗口（鼠标变化）
        setCursor(Qt::ClosedHandCursor);
    }

    // 如果正在拖拽，则更新窗口位置
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        //通过鼠标目前更新后的位置减去之前的相对距离，就是目前窗口左上角的位置
        move(event->globalPos() - m_dragPosition);
    }

    QWidget::mouseMoveEvent(event);
}

void client::mouseReleaseEvent(QMouseEvent *event)
{
    // 停止长按定时器
    m_longPressTimer->stop();

    // 重置状态
    m_longPress = false;
    m_dragging = false;
    //正常鼠标状态
    setCursor(Qt::ArrowCursor);

    QWidget::mouseReleaseEvent(event);
}

void client::onLongPressTimeout()
{
    // 长按时间到达，设置长按标志
    m_longPress = true;
    // 改变鼠标样式为拖拽样式（开始）
    setCursor(Qt::OpenHandCursor);
}

void client::on_quit_clicked()
{
    close();
}

void client::initLongPressTimer(){
    // 初始化长按定时器
    m_longPressTimer = new QTimer(this);
    m_longPressTimer->setSingleShot(true);
    m_longPressTimer->setInterval(200); // 200ms为长按阈值
    connect(m_longPressTimer, &QTimer::timeout, this, &client::onLongPressTimeout);
}

void client::initShadow(){

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setColor("#000000");
    shadow->setOffset(0, 0);
    this->setGraphicsEffect(shadow);
}

void client::getRgbColor(QString imag){
    QImage img(imag);


       QMap<QRgb, int> counter;

       // 遍历像素，统计颜色
       for (int y = 0; y < img.height(); ++y) {
           for (int x = 0; x < img.width(); ++x) {
               QRgb color = img.pixel(x, y);

               // 忽略完全透明的像素（避免透明背景影响结果）
               if (qAlpha(color) == 0)
                   continue;

               counter[color]++;
           }
       }

       // 找出现次数最多的颜色
       QRgb dominantColor = 0;
       int maxCount = 0;
       for (auto it = counter.constBegin(); it != counter.constEnd(); ++it) {
           if (it.value() > maxCount) {
               maxCount = it.value();
               dominantColor = it.key();
           }
       }

       QColor c(dominantColor);
       qDebug() << "主色 RGB:" << c.red() << c.green() << c.blue()
                << " HEX:" << c.name();
}

void client::initQss()
{
    QFile localStyleFile(":/style/style.qss");
    if (localStyleFile.open(QFile::ReadOnly | QFile::Text)) {
        QString localStyleSheet = localStyleFile.readAll();
        this->setStyleSheet(localStyleSheet); // 将样式应用于 client 窗口及其子控件
        localStyleFile.close();
    }
}

void client::initBtForm()
{
    ui->rec->setIconAndText(":/images/rec.png","推荐");
    ui->rec->setId(1);
    ui->like->setIconAndText(":/images/like.png","我喜欢的音乐");
    ui->like->setId(2);
    ui->local->setIconAndText(":/images/local.png","本地音乐");
    ui->local->setId(3);
    ui->recent->setIconAndText(":/images/recent.png","最近播放");
    ui->recent->setId(4);
    btForms.insert(1,ui->rec);
    btForms.insert(2,ui->like);
    btForms.insert(3,ui->local);
    btForms.insert(4,ui->recent);
}

void client::initConnect()
{
    // 统一连接信号
    for (auto it = btForms.constBegin(); it != btForms.constEnd(); ++it) {
        connect(it.value(), &BtForm::btClicked, this, &client::onBtFormClicked);
    }
}

void client::onBtFormClicked(int id)
{
    // 如果点击的是当前激活的按钮，直接返回
    if (id == lastBtFormId) {
        return;
    }
    
    // 恢复上一个按钮的状态
    if (lastBtFormId != 0) {
        if (auto lastForm = btForms.value(lastBtFormId)) {
            lastForm->revertBtForm(imagesHash.value(lastBtFormId));
        }
    }
    
    // 设置新按钮的激活状态
    if (auto currentForm = btForms.value(id)) {
        currentForm->setIcon(imagesHash.value(id + 4));
    }
    
    // 切换界面和更新状态
    ui->stackedWidget->setCurrentIndex(id - 1);
    lastBtFormId = id;
}

void client::initImagesHash()
{
    // 使用初始化列表，更简洁
    imagesHash = {
        {1, ":/images/rec.png"},
        {2, ":/images/like.png"}, 
        {3, ":/images/local.png"},
        {4, ":/images/recent.png"},
        {5, ":/images/p_rec.png"},
        {6, ":/images/p_like.png"},
        {7, ":/images/p_local.png"},
        {8, ":/images/p_recent.png"}
    };
}

QJsonArray client::initRandomPicture()
{
    QVector<QString> vecImageName;
    vecImageName<<"01.png"<<"02.png"<<"03.png"<<"04.png"<<"05.png"<<"06.png"<<"07.png"<<"08.png"<<"09.png"<<"10.png"<<"11.png"<<"12.png"<<"13.png"<<"14.png"<<"15.png"<<"16.png"<<"17.png"<<"18.png"<<"19.png"<<"20.png";
    std::random_shuffle(vecImageName.begin(), vecImageName.end());
    QJsonArray jsonArray;
    for(int i=0;i<vecImageName.size();i++)
    {
        QJsonObject jsonObject;
        jsonObject.insert("path",":/images/rec/" + vecImageName[i]);
        QString strText = QString("推荐-%1").arg(i, 2, 10, QChar('0')); 
        jsonObject.insert("text", strText);
        jsonArray.append(jsonObject);
    }
    return jsonArray;
}