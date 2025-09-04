#ifndef CLIENT_H
#define CLIENT_H

#include <QWidget>
#include <QPoint>
#include <QTimer>
#include <QHash>
#include "btform.h"
#include <QJsonArray>
#include <QJsonObject>
#include "volumetool.h"
#include "musiclist.h"
#include <memory>
#include "mediaplayermanager.h"
#include "progressbar.h"
#include "lrcwidget.h"
#include <QPropertyAnimation>
#include "databaseworker.h"
#include <QSqlDatabase>
#include <QThread>
//class VolumeTool;
QT_BEGIN_NAMESPACE
namespace Ui { class client; }
QT_END_NAMESPACE

class client : public QWidget
{
    Q_OBJECT

public slots:
    void onMusicItemPlayed(const QModelIndex& index, PageType pageType);
    void onMusicPlayAll(PageType pageType);
    void onRecentPlaySignal(const QUrl& url);
    void onVolumeChanged(int value);
    //任务是否完成
    void onDbTaskFinished(bool success, const QString &result);
    void onErrorOccurred(const QString &error);

    void oncheckMusics(const QString &mid,const QUrl &url);

    void onmusicListLikeUpdated(const QString& id, bool like);

public:
    client(QWidget *parent = nullptr);
    ~client();

    void initUi();

    //最近播放的列表的智能指针
    using RecentPlayDataPtr = std::shared_ptr<QList<QUrl>>;
    RecentPlayDataPtr getRecentPlayData() const;

    void recordRecentPlay(const QUrl& url);


signals:
    void recentPlayHistoryUpdated(const QUrl& url);
    //执行sql的信号
    void startSqlTask(const QString& sql, const QVariant& param);

    void checkMusic(const QString& mid,const QUrl& url);
    void ifCheckMusic(bool hasNewMusic,const QUrl& url);
protected:
    // 添加鼠标事件处理函数
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void initLongPressTimer();
    void initShadow();
    //获取图片的主色
    void getRgbColor(QString imag);
    //之后可能会再搞一个set图片的背景色
    //设置qss
    void initQss();
    //设置btform
    void initBtForm();
    //设置信号和槽连接
    void initConnect();
    //设置图片链接的哈希表
    void initImagesHash();
    //设置随机图片的轮播的键值对
    QJsonArray initRandomPicture();
    //初始化推荐界面
    void initRecUi();
    //初始化共同页面
    void initCommonPage();
    //事件过滤器
    bool eventFilter(QObject *obj, QEvent *event) override;
    //安装事件过滤器
    void initInstallEventFilter();
    //事件：音量控件展示,因为音量btn位置锁定，所以不需要event形参
    void onVolumeControlShow();
    //事件：音量控件隐藏
    void onVolumeControlHide();
    //初始化音量隐藏定时器
    void initVolumeHideTimer();
    void paintEvent(QPaintEvent *event) override;
    void initVolumeMonitor();

    // ===== 窗口绘制相关私有方法 =====
    /**
     * @brief 初始化阴影绘制所需的参数
     * @param shadowBlur [out] 阴影模糊半径
     * @param shadowOffset [out] 阴影偏移距离
     * @param cornerRadius [out] 圆角半径
     * @param shadowColor [out] 阴影颜色
     */
    void setupShadowParameters(int &shadowBlur, int &shadowOffset, int &cornerRadius, QColor &shadowColor);

    /**
     * @brief 计算绘制区域
     * @param shadowBlur 阴影模糊半径
     * @param shadowOffset 阴影偏移距离
     * @param widgetRect 整个窗口的矩形区域
     * @param contentRect [out] 主窗口内容区域
     * @param shadowRect [out] 阴影区域
     */
    void calculateDrawAreas(int shadowBlur, int shadowOffset, const QRect &widgetRect, QRect &contentRect, QRect &shadowRect);

    /**
     * @brief 绘制阴影效果
     * @param painter 绘制器对象
     * @param shadowRect 阴影区域
     * @param cornerRadius 圆角半径
     * @param shadowColor 阴影颜色
     * @param shadowBlur 阴影模糊半径
     */
    void drawShadowEffect(QPainter &painter, const QRect &shadowRect, int cornerRadius, const QColor &shadowColor, int shadowBlur);

    /**
     * @brief 绘制主窗口背景
     * @param painter 绘制器对象
     * @param contentRect 主窗口内容区域
     * @param cornerRadius 圆角半径
     */
    void drawWindowBackground(QPainter &painter, const QRect &contentRect, int cornerRadius);

    /**
     * @brief 绘制窗口边框
     * @param painter 绘制器对象
     * @param contentRect 主窗口内容区域
     * @param cornerRadius 圆角半径
     */
    void drawWindowBorder(QPainter &painter, const QRect &contentRect, int cornerRadius);

    // ===== 事件过滤器相关私有方法 =====
    /**
     * @brief 处理音量按钮的事件
     * @param obj 事件对象
     * @param event 事件类型
     * @return 是否处理了事件
     */
    bool handleVolumeButtonEvents(QObject *obj, QEvent *event);

    /**
     * @brief 处理音量工具的事件
     * @param obj 事件对象
     * @param event 事件类型
     * @return 是否处理了事件
     */
    bool handleVolumeToolEvents(QObject *obj, QEvent *event);

    /**
     * @brief 调试打印鼠标位置信息
     * 打印当前鼠标位置和相关控件的调试信息
     */
    void printDebugCursorInfo();
    
 private slots:
    void on_quit_clicked();
    // 长按定时器槽函数
    void onLongPressTimeout();
    //处理点击btform槽函数
    void onBtFormClicked(int id);
    

    void on_volume_clicked();

    void checkCursorForHide();      // 轮询回调

    void on_play_clicked();

    //void mediaPlayerError(QMediaPlayer::Error error);

    void on_playPrev_clicked();

    void on_playNext_clicked();

    void on_playMode_clicked();

    void onDurationChanged(qint64 duration);

    void onPositionChanged(qint64 position);

    void onMetaDataChanged(bool available);

    void on_lrcWord_clicked();

private:
    Ui::client *ui;
    
    // 拖拽相关变量
    bool m_dragging;           // 是否正在拖拽
    QPoint m_dragPosition;     // 拖拽起始位置
    bool m_longPress;          // 是否长按
    QTimer *m_longPressTimer;  // 长按定时器
    QPoint m_pressPosition;    // 鼠标按下位置
    int lastBtFormId;
    //记录不同id的图片链接
    QHash<int, QString> imagesHash;
    //切换不同界面
    QHash<int, BtForm*> btForms;
    //音量设置
    VolumeTool *volumeTool;
    //音量控件隐藏定时器
    QTimer *volumeHideTimer;
    //bool是否显示 ,直接用isVisible()替代
    //bool volumeShow;
    bool volumeToolVisible;
    // bool isVolumeControlActive;  // 音量控制是否激活状态
    // QTimer* volumeShowTimer;     // 延迟显示定时器
    QTimer *volumeMonitorTimer;     // 新增：轮询鼠标位置的 timer
    //音乐列表
    std::shared_ptr<MusicList> musicList;
    //媒体播放
    std::shared_ptr<MediaPlayerManager> playerManager;
    //播放模式
    int playMode;
    //最近播放数据
    RecentPlayDataPtr m_recentPlayData;  
    //歌词界面
    LrcWidget *lrcPage;
    //歌词显示动画
    QPropertyAnimation *lrcAnimation;
    //数据库操作线程
    QThread* m_dbThread;
    DatabaseWorker* m_dbWorker;
    
};
#endif // CLIENT_H
