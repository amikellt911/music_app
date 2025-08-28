#include "client.h"
#include "./ui_client.h"
#include <QMouseEvent>
#include <QTimer>
#include <QApplication>
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>

client::client(QWidget *parent)
    : QWidget(parent), ui(new Ui::client), m_dragging(false), m_longPress(false), m_longPressTimer(nullptr), lastBtFormId(0), volumeHideTimer(nullptr), volumeToolVisible(false)
{
    ui->setupUi(this);
    musicList = std::make_shared<MusicList>();
    // qDebug()<<"测试1";
    volumeTool = new VolumeTool(this);
    volumeTool->hide();
    // qDebug()<<"测试2";
    initUi();
    // qDebug()<<"测试3";
}

client::~client()
{
    delete ui;
}

void client::initUi()
{
    this->setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    initLongPressTimer();
    initShadow();
    initQss();
    initImagesHash();
    initBtForm();
    initConnect();
    // 图片按照时间随机
    initRecUi();
    initCommonPage();
    initVolumeHideTimer();
    initInstallEventFilter();
    ui->volume->setCheckable(true);
    // connect(ui->volume,&QPushButton::toggled,this,&client::on_volume_toggled);
    initVolumeMonitor();
    ui->localPage->setAddLocalIcon(":/images/add_local.png");
    ui->localPage->setAddLocalIconHover();
    ui->likePage->setAddLocalIconUnused();
    ui->recentPage->setAddLocalIconUnused();
    ui->localPage->setMusicList(musicList);
}

void client::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        // 鼠标相对整个屏幕的坐标
        m_pressPosition = event->globalPos();

        // 启动长按定时器
        // start只会实例化一次，只有start只会重置500ms
        m_longPressTimer->start();

        // 如果已经在拖拽状态，则更新位置
        if (m_dragging)
        {
            // 获取鼠标相对窗口的坐标
            m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        }
    }
    // 执行父类函数
    QWidget::mousePressEvent(event);
}
// 重写 paintEvent 自定义绘制阴影

void client::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true); // 开启抗锯齿，让边缘更平滑

    // === 阴影参数配置 ===
    const int shadowBlur = 15;             // 阴影模糊半径（越大阴影越模糊）
    const int shadowOffset = 3;            // 阴影偏移距离（模拟光源位置）
    const int cornerRadius = 8;            // 圆角半径
    const QColor shadowColor(0, 0, 0, 25); // 阴影颜色（黑色，透明度60）

    // === 计算绘制区域 ===
    QRect widgetRect = rect(); // 获取整个窗口的矩形区域

    // 主窗口内容区域（为阴影让出空间）
    QRect contentRect = widgetRect.adjusted(shadowBlur, shadowBlur,
                                            -shadowBlur, -shadowBlur);

    // 阴影区域（向右下偏移）
    QRect shadowRect = contentRect.adjusted(shadowOffset, shadowOffset,
                                            shadowOffset, shadowOffset);

    // === 绘制阴影效果 ===
    // 方法1：使用多层绘制模拟模糊效果
    for (int i = 0; i < shadowBlur; ++i)
    {
        // 计算当前层的透明度（越外层越透明）
        qreal alpha = shadowColor.alpha() * (1.0 - (qreal)i / shadowBlur);
        QColor layerColor = shadowColor;
        layerColor.setAlpha((int)alpha);

        // 计算当前层的矩形（向外扩展）
        QRect layerRect = shadowRect.adjusted(-i, -i, i, i);

        painter.setBrush(QBrush(layerColor));
        painter.setPen(Qt::NoPen); // 不绘制边框
        painter.drawRoundedRect(layerRect, cornerRadius + i, cornerRadius + i);
    }

    // === 绘制主窗口背景 ===
    // 创建圆角路径
    QPainterPath mainPath;
    mainPath.addRoundedRect(contentRect, cornerRadius, cornerRadius);

    // 设置背景色（这里使用白色，你可以根据需要调整）
    painter.fillPath(mainPath, QBrush(QColor(255, 255, 255, 250)));

    // === 绘制边框（可选） ===
    painter.setPen(QPen(QColor(200, 200, 200), 1)); // 淡灰色边框
    painter.drawPath(mainPath);

    // 调用父类的 paintEvent，让子控件正常绘制
    QWidget::paintEvent(event);
}
void client::mouseMoveEvent(QMouseEvent *event)
{
    // 如果是长按状态，则开始拖拽
    if (m_longPress && !m_dragging)
    {
        m_dragging = true;
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        // 告诉用户正在拖拽窗口（鼠标变化）
        setCursor(Qt::ClosedHandCursor);
    }

    // 如果正在拖拽，则更新窗口位置
    if (m_dragging && (event->buttons() & Qt::LeftButton))
    {
        // 通过鼠标目前更新后的位置减去之前的相对距离，就是目前窗口左上角的位置
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
    // 正常鼠标状态
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

void client::initLongPressTimer()
{
    // 初始化长按定时器
    m_longPressTimer = new QTimer(this);
    m_longPressTimer->setSingleShot(true);
    m_longPressTimer->setInterval(200); // 200ms为长按阈值
    connect(m_longPressTimer, &QTimer::timeout, this, &client::onLongPressTimeout);
}

void client::initShadow()
{

    // QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    // shadow->setBlurRadius(20);
    // shadow->setColor("#000000");
    // shadow->setOffset(0, 0);
    // this->setGraphicsEffect(shadow);
    // QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    // shadow->setBlurRadius(10);  // 减小模糊半径
    // shadow->setColor(QColor(0, 0, 0, 127));  // 使用半透明
    // shadow->setOffset(2, 2);    // 设置小的偏移
    // this->setGraphicsEffect(shadow);
    // 不再使用 QGraphicsDropShadowEffect

    // 设置窗口内容边距，为阴影留出空间
    int shadowMargin = 15; // 与 shadowBlur 保持一致
    this->setContentsMargins(shadowMargin, shadowMargin, shadowMargin, shadowMargin);

    // 强制重绘窗口
    this->update();
}

void client::getRgbColor(QString imag)
{
    QImage img(imag);

    QMap<QRgb, int> counter;

    // 遍历像素，统计颜色
    for (int y = 0; y < img.height(); ++y)
    {
        for (int x = 0; x < img.width(); ++x)
        {
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
    for (auto it = counter.constBegin(); it != counter.constEnd(); ++it)
    {
        if (it.value() > maxCount)
        {
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
    if (localStyleFile.open(QFile::ReadOnly | QFile::Text))
    {
        QString localStyleSheet = localStyleFile.readAll();
        this->setStyleSheet(localStyleSheet); // 将样式应用于 client 窗口及其子控件
        localStyleFile.close();
    }
}

void client::initBtForm()
{
    ui->rec->setIconAndText(":/images/rec.png", "推荐");
    ui->rec->setId(1);
    ui->like->setIconAndText(":/images/like.png", "我喜欢的音乐");
    ui->like->setId(2);
    ui->local->setIconAndText(":/images/local.png", "本地音乐");
    ui->local->setId(3);
    ui->recent->setIconAndText(":/images/recent.png", "最近播放");
    ui->recent->setId(4);
    btForms.insert(1, ui->rec);
    btForms.insert(2, ui->like);
    btForms.insert(3, ui->local);
    btForms.insert(4, ui->recent);
}

void client::initConnect()
{
    // 统一连接信号
    for (auto it = btForms.constBegin(); it != btForms.constEnd(); ++it)
    {
        connect(it.value(), &BtForm::btClicked, this, &client::onBtFormClicked);
    }
}

void client::onBtFormClicked(int id)
{
    // 如果点击的是当前激活的按钮，直接返回
    if (id == lastBtFormId)
    {
        return;
    }

    // 恢复上一个按钮的状态
    if (lastBtFormId != 0)
    {
        if (auto lastForm = btForms.value(lastBtFormId))
        {
            lastForm->revertBtForm(imagesHash.value(lastBtFormId));
        }
    }

    // 设置新按钮的激活状态
    if (auto currentForm = btForms.value(id))
    {
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
        {8, ":/images/p_recent.png"}};
}

QJsonArray client::initRandomPicture()
{
    QVector<QString> vecImageName;
    vecImageName << "01.png" << "02.png" << "03.png" << "04.png" << "05.png" << "06.png" << "07.png" << "08.png" << "09.png" << "10.png" << "11.png" << "12.png" << "13.png" << "14.png" << "15.png" << "16.png" << "17.png" << "18.png" << "19.png" << "20.png";
    std::random_shuffle(vecImageName.begin(), vecImageName.end());
    QJsonArray jsonArray;
    for (int i = 0; i < vecImageName.size(); i++)
    {
        QJsonObject jsonObject;
        jsonObject.insert("path", ":/images/rec/" + vecImageName[i]);
        QString strText = QString("推荐-%1").arg(i, 2, 10, QChar('0'));
        jsonObject.insert("text", strText);
        jsonArray.append(jsonObject);
    }
    return jsonArray;
}

void client::initRecUi()
{
    srand(time(NULL));
    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->recMusicBox->initRecBoxUi(initRandomPicture(), 1);
    ui->supplyMusicBox->initRecBoxUi(initRandomPicture(), 2);
}

void client::initCommonPage()
{
    ui->likePage->setCommonPageUi("我喜欢的音乐", ":/images/cover/02.png");
    ui->localPage->setCommonPageUi("本地音乐", ":/images/cover/03.png");
    ui->recentPage->setCommonPageUi("最近播放", ":/images/cover/04.png");
}
bool client::eventFilter(QObject *obj, QEvent *event)
{
    auto dbgPrintUnderCursor = [&]()
    {
        QPoint g = QCursor::pos();
        QWidget *w = QApplication::widgetAt(g);
        qDebug() << "CURSOR_GLOBAL:" << g
                 << " widgetAt:" << (w ? w->objectName() : QString("null"))
                 << " class:" << (w ? w->metaObject()->className() : "null");
        qDebug() << "volume global rect:"
                 << QRect(ui->volume->mapToGlobal(QPoint(0, 0)), ui->volume->size());
        qDebug() << "tool global rect:"
                 << QRect(volumeTool->mapToGlobal(QPoint(0, 0)), volumeTool->size());
    };
    if (obj == ui->volume) {
    if (event->type() == QEvent::Enter) {
        qDebug() << "enter volume";
        // 停止任何等待隐藏的定时器
        if (volumeHideTimer) volumeHideTimer->stop();
        // 延迟显示，避免和 show/move 的竞态（若你已经用轮询，可把延迟调小或直接调用）
        QTimer::singleShot(10, this, [this](){ onVolumeControlShow(); });
        return true;
    }
    if (event->type() == QEvent::Leave) {
        qDebug() << "volume leave";
        dbgPrintUnderCursor(); // 如果你继续保留调试输出

        QPoint g = QCursor::pos();
        QRect volumeGlobalRect(ui->volume->mapToGlobal(QPoint(0,0)), ui->volume->size());

        // 如果鼠标仍然在按钮区域 -> 假 Leave，恢复按钮状态
        if (volumeGlobalRect.contains(g)) {
            qDebug() << "   ignore fake leave: cursor still inside volume, re-post Enter";
            QCoreApplication::postEvent(ui->volume, new QEvent(QEvent::Enter));
            return true;
        }

        // 如果在工具上，也忽略（不隐藏）
        if (volumeToolVisible) {
            QRect toolGlobalRect(volumeTool->mapToGlobal(QPoint(0,0)), volumeTool->size());
            if (toolGlobalRect.contains(g)) {
                qDebug() << "   cursor is on tool, ignore leave";
                return true;
            }
        }

        // 真实离开：你可以选择直接隐藏或启动延迟隐藏
        // 使用已有延迟隐藏
        if (volumeHideTimer) volumeHideTimer->start(500);
        return true;
    }
}


    if (obj == volumeTool)
    {
        if (event->type() == QEvent::Leave)
        {
            qDebug() << "volumeTool leave";
            QPoint globalPos = QCursor::pos();

            // 检查是否回到 volume 按钮
            QRect volumeGlobalRect = ui->volume->geometry();
            volumeGlobalRect.moveTopLeft(ui->volume->mapToGlobal(QPoint(0, 0)));

            if (!volumeGlobalRect.contains(globalPos))
            {
                volumeHideTimer->start(500); // 200ms 延迟隐藏
            }
            return true;
        }
    }

    return QWidget::eventFilter(obj, event);
}
// bool client::eventFilter(QObject *obj, QEvent *event)
// {
//         if(obj == ui->volume)
//     {
//         if(event->type() == QEvent::Enter)
//         {
//             qDebug() << "enter1";
//             volumeHideTimer->stop();
//             onVolumeControlShow();
//             return true;
//         }
//         if(event->type() == QEvent::Leave)
//         {
//             qDebug() << "leave1";
//             // 增加延迟，避免快速进入/离开导致的闪烁
//             volumeHideTimer->start(300);  // 300ms 延迟
//             return true;
//         }
//     }

//     if(obj == volumeTool)
//     {
//         if(event->type() == QEvent::Enter)
//         {
//             qDebug() << "enter2";
//             // 鼠标进入 volumeTool 时，停止隐藏定时器
//             volumeHideTimer->stop();
//             return true;
//         }
//         if(event->type() == QEvent::Leave)
//         {
//             qDebug() << "leave2";
//             // 检查鼠标是否真的离开了控制区域
//             QPoint globalPos = QCursor::pos();
//             QRect volumeRect = ui->volume->geometry();
//             QRect toolRect = volumeTool->geometry();

//             // 将按钮坐标转换为全局坐标
//             volumeRect.moveTopLeft(ui->volume->mapToGlobal(volumeRect.topLeft()));

//             // 如果鼠标不在任何相关控件上，才开始隐藏定时器
//             if (!volumeRect.contains(globalPos) && !toolRect.contains(globalPos)) {
//                 volumeHideTimer->start(200);  // 200ms 延迟
//             }
//             return true;
//         }
//     }

//     return QWidget::eventFilter(obj, event);
//     // if(obj == ui->volume)
//     // {
//     //     if(event->type()==QEvent::Enter)
//     //     {
//     //         qDebug()<<"enter1";
//     //         volumeHideTimer->stop();
//     //         onVolumeControlShow();
//     //         return true;
//     //     }
//     //     if(event->type()==QEvent::Leave)
//     //     {
//     //         qDebug()<<"leave1";
//     //         volumeHideTimer->start();
//     //         //onVolumeControlHide();
//     //         return true;
//     //     }
//     // }
//     // if(obj == volumeTool)
//     // {
//     //     if(event->type()==QEvent::Leave)
//     //     {
//     //         qDebug()<<"leave2";
//     //         volumeHideTimer->start();
//     //         //onVolumeControlHide();
//     //         return true;
//     //     }
//     //     if(event->type()==QEvent::Enter)
//     //     {
//     //         qDebug()<<"enter2";
//     //         volumeHideTimer->stop();
//     //         onVolumeControlShow();
//     //         return true;
//     //     }
//     // }
//     // return QWidget::eventFilter(obj, event);
// }

void client::initInstallEventFilter()
{
    ui->volume->installEventFilter(this);
    volumeTool->installEventFilter(this);
}

// void client::onVolumeControlShow(){
//     //qDebug()<<"测试进入";
//     qDebug() << "=== 显示前状态 ===";
//     qDebug() << "volume button geometry:" << ui->volume->geometry();
//     qDebug() << "volume button global pos:" << ui->volume->mapToGlobal(QPoint(0, 0));
//     qDebug() << "volumeTool size:" << volumeTool->size();
//     QPoint pos = ui->volume->mapToGlobal(QPoint(15,0));
//     QPoint leftTopVolume=pos-QPoint(volumeTool->width()/2,volumeTool->height());
//     volumeTool->move(leftTopVolume);
//     volumeTool->show();
//     qDebug() << "=== 显示后状态 ===";
//     qDebug() << "volumeTool geometry:" << volumeTool->geometry();
//     qDebug() << "volumeTool visible:" << volumeTool->isVisible();
//     // volumeTool->raise();
//     // volumeTool->activateWindow();
// }
void client::onVolumeControlShow()
{
    if (volumeToolVisible) return;

    QPoint buttonPos = ui->volume->mapToGlobal(QPoint(0,0));
    QSize buttonSize = ui->volume->size();
    QSize toolSize = volumeTool->size();
    //如果变大的话，因为leave，会直接隐藏
    int spacing = 0;
    QPoint toolPos(buttonPos.x() + (buttonSize.width() - toolSize.width())/2,
                   buttonPos.y() - toolSize.height() - spacing);

    volumeTool->move(toolPos);
    volumeTool->show();
    //volumeTool->raise();
    volumeToolVisible = true;

    // 启动轮询监控（作为最后的保障）
    if (volumeMonitorTimer) volumeMonitorTimer->start();
}

void client::onVolumeControlHide()
{
    if (!volumeToolVisible) return;

    volumeTool->hide();
    volumeToolVisible = false;

    // 停止轮询
    if (volumeMonitorTimer) volumeMonitorTimer->stop();
    //QCoreApplication::postEvent(ui->volume, new QEvent(QEvent::Leave));
}

void client::initVolumeHideTimer()
{
    volumeHideTimer = new QTimer(this);
    // 设置单次触发
    volumeHideTimer->setSingleShot(true);
    volumeHideTimer->setInterval(500);
    connect(volumeHideTimer, &QTimer::timeout, this, &client::onVolumeControlHide);
}
void client::on_volume_toggled(bool checked)
{
    if (checked)
    {
        qDebug() << "测试点击1";
        ui->volume->setStyleSheet("background-image: url(:/images/volume.png);");
        volumeTool->setVolumeRatio(100);
        volumeTool->setOutSlider(100);
        volumeTool->setSliderBtn(100);
    }
    else
    {
        qDebug() << "测试点击2";
        ui->volume->setStyleSheet("background-image: url(:/images/mute.png);");
        volumeTool->setVolumeRatio(0);
        volumeTool->setOutSlider(0);
        volumeTool->setSliderBtn(0);
    }
}

void client::initVolumeMonitor()
{
    // 100ms 轮询（可更改为 80~150ms）
    volumeMonitorTimer = new QTimer(this);
    volumeMonitorTimer->setInterval(100);
    volumeMonitorTimer->setSingleShot(false);
    connect(volumeMonitorTimer, &QTimer::timeout, this, &client::checkCursorForHide);
    // 不自动启动，只有在 volumeTool 显示时启动
}

void client::checkCursorForHide()
{
    // 如果面板不可见或 timer 不该跑，直接返回
    if (!volumeToolVisible) {
        if (volumeMonitorTimer) volumeMonitorTimer->stop();
        return;
    }

    QPoint g = QCursor::pos();

    QRect volumeGlobalRect(ui->volume->mapToGlobal(QPoint(0,0)), ui->volume->size());
    QRect toolGlobalRect(volumeTool->mapToGlobal(QPoint(0,0)), volumeTool->size());

    // 如果鼠标既不在按钮也不在工具面板上，则隐藏（立即）
    if (!volumeGlobalRect.contains(g) && !toolGlobalRect.contains(g)) {
        // 你可以选择先启动 volumeHideTimer（延迟）或直接隐藏
        // 我这里直接调用隐藏，使体验更明确；如需延迟改为 start timer
        onVolumeControlHide();
    }
}
