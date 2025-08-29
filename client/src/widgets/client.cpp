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

/**
 * @brief 初始化主界面UI
 * 该函数完成整个主界面所有组件的初始化，按照逻辑分组进行
 * 注释：有些初始化可能有依赖关系，所以顺序很重要
 */
void client::initUi()
{
    // ===== 一、基础窗口属性设置 =====
    this->setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    initLongPressTimer();
    initShadow();

    // ===== 二、样式和资源初始化 =====
    initQss();
    initImagesHash();

    // ===== 三、界面组件初始化 =====
    initBtForm();        // 按钮表单
    initConnect();       // 信号连接
    initRecUi();         // 推荐界面（随机图片）
    initCommonPage();    // 通用页面

    // ===== 四、音量控件初始化 =====
    initVolumeHideTimer();
    initInstallEventFilter();
    ui->volume->setCheckable(true);
    initVolumeMonitor();
    // 注释：音量toggle连接已被注释，可能存在某些情况下不需要的连接

    // ===== 五、页面数据绑定 =====
    ui->localPage->setAddLocalIcon(":/images/add_local.png");
    ui->localPage->setAddLocalIconHover();
    ui->likePage->setAddLocalIconUnused();
    ui->recentPage->setAddLocalIconUnused();
    ui->localPage->setMusicList(musicList);
    ui->likePage->setMusicList(musicList);
    ui->recentPage->setMusicList(musicList);
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
// =============================================================================
//  窗口阴影绘制相关方法 - 重构后的分割实现
// =============================================================================

/**
 * @brief 初始化阴影绘制所需的参数
 * 设置阴影效果的基础参数，包括模糊程度、偏移距离、圆角半径和颜色
 */
void client::setupShadowParameters(int &shadowBlur, int &shadowOffset, int &cornerRadius, QColor &shadowColor)
{
    shadowBlur = 15;           // 阴影模糊半径（越大阴影越模糊）
    shadowOffset = 3;          // 阴影偏移距离（模拟光源位置）
    cornerRadius = 8;          // 圆角半径
    shadowColor = QColor(0, 0, 0, 25);  // 阴影颜色（黑色，透明度25）
}

/**
 * @brief 计算绘制区域
 * 基于阴影参数计算主窗口内容区域和阴影绘制区域
 */
void client::calculateDrawAreas(int shadowBlur, int shadowOffset, const QRect &widgetRect, QRect &contentRect, QRect &shadowRect)
{
    // 主窗口内容区域（为阴影让出空间）
    contentRect = widgetRect.adjusted(shadowBlur, shadowBlur, -shadowBlur, -shadowBlur);

    // 阴影区域（向右下偏移，模拟光源从左上照射）
    shadowRect = contentRect.adjusted(shadowOffset, shadowOffset, shadowOffset, shadowOffset);
}

/**
 * @brief 绘制阴影效果
 * 使用多层绘制技术模拟阴影的模糊效果，越外层透明度越低
 */
void client::drawShadowEffect(QPainter &painter, const QRect &shadowRect, int cornerRadius, const QColor &shadowColor, int shadowBlur)
{
    for (int layer = 0; layer < shadowBlur; ++layer)
    {
        // 计算当前层的透明度（越外层越透明）
        qreal alpha = shadowColor.alpha() * (1.0 - (qreal)layer / shadowBlur);
        QColor layerColor = shadowColor;
        layerColor.setAlpha((int)alpha);

        // 计算当前层的矩形（向外扩展，模拟向外模糊）
        QRect layerRect = shadowRect.adjusted(-layer, -layer, layer, layer);

        painter.setBrush(QBrush(layerColor));
        painter.setPen(Qt::NoPen); // 无边框，只有填充
        painter.drawRoundedRect(layerRect, cornerRadius + layer, cornerRadius + layer);
    }
}

/**
 * @brief 绘制主窗口背景
 * 绘制窗口的主要内容区域背景，使用圆角矩形
 */
void client::drawWindowBackground(QPainter &painter, const QRect &contentRect, int cornerRadius)
{
    // 创建圆角路径
    QPainterPath mainPath;
    mainPath.addRoundedRect(contentRect, cornerRadius, cornerRadius);

    // 设置半透明白色背景
    QColor backgroundColor(255, 255, 255, 250); // 白色，透明度250
    painter.fillPath(mainPath, QBrush(backgroundColor));
}

/**
 * @brief 绘制窗口边框
 * 为窗口主体绘制边框线条，增强视觉效果
 */
void client::drawWindowBorder(QPainter &painter, const QRect &contentRect, int cornerRadius)
{
    // 创建圆角路径（与背景使用相同的路径确保对齐）
    QPainterPath borderPath;
    borderPath.addRoundedRect(contentRect, cornerRadius, cornerRadius);

    // 设置边框样式
    QColor borderColor(200, 200, 200);  // 淡灰色边框
    painter.setPen(QPen(borderColor, 1));
    painter.setBrush(Qt::NoBrush); // 无填充，只有边框
    painter.drawPath(borderPath);
}

// =============================================================================
//  事件过滤器相关方法 - 重构后的分割实现
// =============================================================================

/**
 * @brief 调试打印鼠标位置信息
 * 打印当前鼠标位置和相关控件的调试信息，保留原有调试逻辑
 */
void client::printDebugCursorInfo()
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
}

/**
 * @brief 处理音量按钮的事件
 * 处理音量按钮的鼠标进入和离开事件，控制音量工具的显示和隐藏
 * 注意：保留原有逻辑，不进行任何优化，只是代码重组
 */
bool client::handleVolumeButtonEvents(QObject *obj, QEvent *event) {
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
            printDebugCursorInfo(); // 使用重构后的调试打印方法

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

            // 真实离开：使用原有逻辑启动延迟隐藏
            if (volumeHideTimer) volumeHideTimer->start(500);
            return true;
        }
    }
    return false; // 如果对象不是音量按钮或事件不匹配，返回false
}

/**
 * @brief 处理音量工具的事件
 * 处理音量工具的鼠标离开事件，控制工具的隐藏逻辑
 * 注意：保留原有逻辑，不进行任何优化，只是代码重组
 */
bool client::handleVolumeToolEvents(QObject *obj, QEvent *event) {
    if (obj == volumeTool) {
        if (event->type() == QEvent::Leave) {
            qDebug() << "volumeTool leave";
            QPoint globalPos = QCursor::pos();

            // 检查是否回到 volume 按钮
            QRect volumeGlobalRect = ui->volume->geometry();
            volumeGlobalRect.moveTopLeft(ui->volume->mapToGlobal(QPoint(0, 0)));

            if (!volumeGlobalRect.contains(globalPos)) {
                volumeHideTimer->start(500); // 200ms 延迟隐藏
            }
            return true;
        }
    }
    return false; // 如果对象不是音量工具或事件不匹配，返回false
}

// 重写 paintEvent 自定义绘制阴影 - 重构后简洁版本
void client::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true); // 开启抗锯齿，边缘更平滑

    // 初始化阴影参数
    int shadowBlur, shadowOffset, cornerRadius;
    QColor shadowColor;
    setupShadowParameters(shadowBlur, shadowOffset, cornerRadius, shadowColor);

    // 计算绘制区域
    QRect widgetRect = rect();  // 整个窗口的矩形区域
    QRect contentRect, shadowRect;
    calculateDrawAreas(shadowBlur, shadowOffset, widgetRect, contentRect, shadowRect);

    // 绘制阴影效果
    drawShadowEffect(painter, shadowRect, cornerRadius, shadowColor, shadowBlur);

    // 绘制主窗口背景
    drawWindowBackground(painter, contentRect, cornerRadius);

    // 绘制窗口边框
    drawWindowBorder(painter, contentRect, cornerRadius);

    // 调用父类的 paintEvent，确保子控件正常绘制
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
/**
 * @brief 重构后的事件过滤器函数
 * 使用封装的私有方法处理不同对象的不同事件，代码结构更加清晰
 * 注意：保留原有逻辑，不进行任何优化，只是代码重组
 */
bool client::eventFilter(QObject *obj, QEvent *event)
{
    // 处理音量按钮的事件
    if (handleVolumeButtonEvents(obj, event)) {
        return true;
    }

    // 处理音量工具的事件
    if (handleVolumeToolEvents(obj, event)) {
        return true;
    }

    // 如果以上都没有处理，则调用父类的默认处理
    return QWidget::eventFilter(obj, event);
}
/* ============ 已废弃的eventFilter实现 - 保留作为历史参考 ============
 * 以下是重构前的eventFilter函数实现，现在已使用新的封装方法代替
 * 如果需要回滚或对比，可以参考这些注释代码
 *
 * 原版本1：简单的定时器控制版本
 * 原版本2：复杂的区域检测版本（包含更多边界情况处理）
 * 原版本3：最基础版本（仅基本的隐藏显示控制）
 *
 * 注：重构后的事件逻辑更加清晰，调试信息更加集中，
 *     同时保留了所有原有功能（包括所有已知的问题）
 */

void client::initInstallEventFilter()
{
    ui->volume->installEventFilter(this);
    volumeTool->installEventFilter(this);
}

/* ============ 已废弃的onVolumeControlShow实现 ============
 * 原版本包含详细的调试信息，现在已重构为简洁版本
 * 如果需要更多调试信息，可以恢复此版本
 */
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
