#include "lrcwidget.h"
#include "ui_lrcwidget.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
LrcWidget::LrcWidget(QWidget *parent) : QWidget(parent),
                                        ui(new Ui::LrcWidget)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);
    ui->hideBtn->setIcon(QIcon(":/images/hideBtn.png"));
    animation = new QPropertyAnimation(this, "geometry", this);
    animation->setDuration(500);
    animation->setStartValue(QRect(10, 10, width(), height()));
    animation->setEndValue(QRect(10, 10 + height(), width(), height()));
    connect(animation, &QPropertyAnimation::finished, [this]()
            { this->hide(); });
}

LrcWidget::~LrcWidget()
{
    delete ui;
}

void LrcWidget::on_hideBtn_clicked()
{
    animation->start();
}

void LrcWidget::setLrcUrl(const QString &url, bool available)
{
    lrcUrl = url;
    lrcAvailable = available;
}

bool LrcWidget::parseLrcFile()
{
    lrcWordLines.clear();
    qDebug() << "lrcAvailable:" << lrcAvailable;
    if (!lrcAvailable)
        return false;
    QFile file(lrcUrl);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "歌词文件打开失败";
        return false;
    }
    QTextStream in(&file);
    in.setCodec("utf-8");
    qDebug() << "歌词开始解析";
    int i = 0;
    while (!in.atEnd())
    {
        // 读取，并将两边空格搞掉
        QString line = in.readLine().trimmed();
        if (line.isEmpty())
            continue;
        int pos = line.indexOf("]");
        QString timeText = line.mid(1, pos - 1);
        QString lryicText = line.mid(pos + 1);
        QStringList timeList = timeText.split(":");
        if (timeList.size() != 2)
            continue;
        int minutes = timeList[0].toInt();
        double seconds = timeList[1].toDouble();
        i++;
        qint64 ms = qRound64((minutes * 60 + seconds) * 1000);
        lrcWordLines.push_back(lrcWordLine(ms, lryicText));
    }
    qDebug() << "歌词解析完毕";
    // for (auto e : lrcWordLines)
    // {
    //     qDebug() << e.times << ":" << e.texts;
    // }
    return true;
}

void LrcWidget::setTitle(const QString &title)
{
    ui->title->setText(title);
}
void LrcWidget::setAuthor(const QString &author)
{
    ui->author->setText(author);
}

void LrcWidget::positionChanged(qint64 position)
{
    if(!lrcAvailable)
    {
        //处理歌词文件不存在的情况
        ui->centerLrc->setText("暂无歌词");
        ui->lrc1->setText("");
        ui->lrc2->setText("");
        ui->lrc3->setText("");
        ui->lrc4->setText("");
        ui->lrc5->setText("");
        ui->lrc6->setText("");
        return;
    }
    auto it = std::lower_bound(lrcWordLines.begin(), lrcWordLines.end(), position,
                               [](const lrcWordLine &l, qint64 r)
                               { 
                                return l.times < r; 
                                });
    if (it == lrcWordLines.begin()) {
    // 播放时间在第一行之前
    qDebug() << "当前歌词:" << it->texts;
    ui->centerLrc->setText(it->texts);
    ui->lrc1->setText("");
    ui->lrc2->setText("");
    ui->lrc3->setText("");
    ui->lrc4->setText("");
    ui->lrc5->setText("");
    ui->lrc6->setText("");
    //要不要else直接break，不然it超了end怎么办，else直接下面的也设置为""(我提前设置一遍就不怕这个情况了)
    if(it+1 != lrcWordLines.end())
    ui->lrc4->setText((it+1)->texts);
    else {
        return;
    }
    if(it+2 != lrcWordLines.end())
    ui->lrc5->setText((it+2)->texts);
    else {
        return;
    }
    if(it+3 != lrcWordLines.end())
    ui->lrc6->setText((it+3)->texts);
    else {
        return;
    }
} else {
    // lower_bound 返回的是 >= curTime 的第一个元素
    // 所以真正对应的歌词是前一个
    --it;
    qDebug() << "当前歌词:" << it->texts;
    ui->centerLrc->setText(it->texts);
    ui->lrc1->setText("");
    ui->lrc2->setText("");
    ui->lrc3->setText("");
    ui->lrc4->setText("");
    ui->lrc5->setText("");
    ui->lrc6->setText("");
    //不应该是-1,因为begin()应该-1，然后抵消了
    //因为有可能it=begin()+1，然后开头--it，所以必须要判断
    if(it != lrcWordLines.begin())
    ui->lrc3->setText((it-1)->texts);
    else {
        return;
    }
    if(it-1 != lrcWordLines.begin())
    ui->lrc2->setText((it-2)->texts);
    else {
        return;
    }
    if(it-2 != lrcWordLines.begin())
    ui->lrc1->setText((it-3)->texts);
    else {
        return;
    }
        if(it+1 != lrcWordLines.end())
    ui->lrc4->setText((it+1)->texts);
    else {
        return;
    }
    if(it+2 != lrcWordLines.end())
    ui->lrc5->setText((it+2)->texts);
    else {
        return;
    }
    if(it+3 != lrcWordLines.end())
    ui->lrc6->setText((it+3)->texts);
    else {
        return;
    }
}
}

// void LrcWidget::positionChanged(qint64 position)
// {
//     if(!lrcAvailable)
//     {
//         //处理歌词文件不存在的情况
//         ui->centerLrc->setText("暂无歌词");
//         ui->lrc1->setText("");
//         ui->lrc2->setText("");
//         ui->lrc3->setText("");
//         ui->lrc4->setText("");
//         ui->lrc5->setText("");
//         ui->lrc6->setText("");
//         return;
//     }

//     // 如果歌词列表为空，直接返回
//     if (lrcWordLines.isEmpty()) {
//         ui->centerLrc->setText("暂无歌词");
//         ui->lrc1->setText("");
//         ui->lrc2->setText("");
//         ui->lrc3->setText("");
//         ui->lrc4->setText("");
//         ui->lrc5->setText("");
//         ui->lrc6->setText("");
//         return;
//     }

//     // 先清空所有歌词显示
//     ui->lrc1->setText("");
//     ui->lrc2->setText("");
//     ui->lrc3->setText("");
//     ui->centerLrc->setText("");
//     ui->lrc4->setText("");
//     ui->lrc5->setText("");
//     ui->lrc6->setText("");

//     // 使用lower_bound查找当前时间对应的歌词位置
//     auto it = std::lower_bound(lrcWordLines.begin(), lrcWordLines.end(), position,
//                                [](const lrcWordLine &l, qint64 r)
//                                { 
//                                 return l.times < r; 
//                                 });

//     int currentIndex = -1;
    
//     if (it == lrcWordLines.begin()) {
//         // 播放时间在第一行之前，显示第一行
//         currentIndex = 0;
//     } else if (it == lrcWordLines.end()) {
//         // 播放时间在最后一行之后，显示最后一行
//         currentIndex = lrcWordLines.size() - 1;
//     } else {
//         // lower_bound返回的是>=curTime的第一个元素
//         // 所以真正对应的歌词是前一个
//         currentIndex = std::distance(lrcWordLines.begin(), it) - 1;
//     }

//     // 安全检查当前索引
//     if (currentIndex < 0 || currentIndex >= lrcWordLines.size()) {
//         return;
//     }

//     // 设置当前歌词
//     ui->centerLrc->setText(lrcWordLines[currentIndex].texts);
//     qDebug() << "当前歌词:" << lrcWordLines[currentIndex].texts;

//     // 安全设置前面的歌词（lrc1, lrc2, lrc3）
//     if (currentIndex >= 3) {
//         ui->lrc1->setText(lrcWordLines[currentIndex - 3].texts);
//     }
//     if (currentIndex >= 2) {
//         ui->lrc2->setText(lrcWordLines[currentIndex - 2].texts);
//     }
//     if (currentIndex >= 1) {
//         ui->lrc3->setText(lrcWordLines[currentIndex - 1].texts);
//     }

//     // 安全设置后面的歌词（lrc4, lrc5, lrc6）
//     if (currentIndex + 1 < lrcWordLines.size()) {
//         ui->lrc4->setText(lrcWordLines[currentIndex + 1].texts);
//     }
//     if (currentIndex + 2 < lrcWordLines.size()) {
//         ui->lrc5->setText(lrcWordLines[currentIndex + 2].texts);
//     }
//     if (currentIndex + 3 < lrcWordLines.size()) {
//         ui->lrc6->setText(lrcWordLines[currentIndex + 3].texts);
//     }
// }