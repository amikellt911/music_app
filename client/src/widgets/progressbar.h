#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QWidget>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>

namespace Ui {
class ProgressBar;
}

class ProgressBar : public QWidget
{
    Q_OBJECT

public:
    explicit ProgressBar(QWidget *parent = nullptr);
    ~ProgressBar();

public slots:
    void onPositionChanged(qint64 position);
    void onDurationChanged(qint64 duration);

signals:
    void positionChanged(qint64 position);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    //void paintEvent(QPaintEvent *event) override;

private:
    Ui::ProgressBar *ui;

    // 成员变量
    qint64 m_currentPosition = 0;
    qint64 m_totalDuration = 0;
    bool m_isDragging = false;

    // 进度条区域
    QRect m_barRect;  // under widget区域

    // 辅助方法
    qint64 calculatePositionFromMouse(const QPoint &mousePos);
    void updateProgressVisualization();
};

#endif // PROGRESSBAR_H
