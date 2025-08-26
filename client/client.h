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
//class VolumeTool;
QT_BEGIN_NAMESPACE
namespace Ui { class client; }
QT_END_NAMESPACE

class client : public QWidget
{
    Q_OBJECT

public:
    client(QWidget *parent = nullptr);
    ~client();

    void initUi();
    
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
private slots:
    void on_quit_clicked();
    // 长按定时器槽函数
    void onLongPressTimeout();
    //处理点击btform槽函数
    void onBtFormClicked(int id);
    

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
};
#endif // CLIENT_H