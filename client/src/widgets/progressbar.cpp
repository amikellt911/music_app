#include "progressbar.h"
#include "ui_progressbar.h"
#include <QPainter>
#include <QPainterPath>
#include <QDebug>

ProgressBar::ProgressBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProgressBar)
{
    ui->setupUi(this);

    // 启用鼠标跟踪
    setMouseTracking(true);

    // 设置进度条区域为under widget
    if (ui->under) {
        m_barRect = ui->under->geometry();
    }
}

ProgressBar::~ProgressBar()
{
    delete ui;
}

void ProgressBar::onPositionChanged(qint64 position)
{
    m_currentPosition = position;
    updateProgressVisualization();
}

void ProgressBar::onDurationChanged(qint64 duration)
{
    m_totalDuration = duration;
    if (ui->under) {
        m_barRect = ui->under->geometry();
    }
    updateProgressVisualization();
}

void ProgressBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_totalDuration > 0 &&
        m_barRect.contains(event->pos())) {
        m_isDragging = true;
        grabMouse();  // 抓取鼠标事件
        qint64 position = calculatePositionFromMouse(event->pos());
        m_previewPosition = position;  // 设置预览位置
        updateProgressVisualization();  // 更新显示预览
    }
    QWidget::mousePressEvent(event);
}

void ProgressBar::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDragging && m_totalDuration > 0) {
        // 拖拽中只更新预览，不发射信号
        qint64 position = calculatePositionFromMouse(mapFromGlobal(event->globalPos()));
        m_previewPosition = position;
        updateProgressVisualization();  // 显示预览位置
        event->accept();
        return;
    }
    QWidget::mouseMoveEvent(event);
}

void ProgressBar::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_isDragging) {
        m_isDragging = false;
        releaseMouse();  // 释放鼠标抓取

        // 只有在释放时才发射最终位置信号
        if (m_previewPosition >= 0) {
            emit positionChanged(m_previewPosition);
            m_currentPosition = m_previewPosition;  // 更新当前位置
            m_previewPosition = -1;  // 重置预览位置
        }

        event->accept();
        return;
    }
    QWidget::mouseReleaseEvent(event);
}

void ProgressBar::updateProgressVisualization()
{
    if (!ui->above || m_totalDuration == 0) return;

    // 如果正在拖拽，显示预览位置，否则显示当前播放位置
    qint64 displayPosition = (m_isDragging && m_previewPosition >= 0) ?
                             m_previewPosition : m_currentPosition;

    // 计算进度百分比
    double progressRatio = static_cast<double>(displayPosition) / m_totalDuration;
    progressRatio = qBound(0.0, progressRatio, 1.0);

    // 更新above widget的宽度
    QRect aboveRect = ui->above->geometry();
    aboveRect.setWidth(static_cast<int>(m_barRect.width() * progressRatio));
    ui->above->setGeometry(aboveRect);
}

qint64 ProgressBar::calculatePositionFromMouse(const QPoint &mousePos)
{
    // 根据鼠标位置计算播放位置
    double ratio = static_cast<double>(mousePos.x() - m_barRect.left()) / m_barRect.width();
    ratio = qBound(0.0, ratio, 1.0);
    return static_cast<qint64>(ratio * m_totalDuration);
}
