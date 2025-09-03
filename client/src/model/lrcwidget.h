#ifndef LRCWIDGET_H
#define LRCWIDGET_H

#include <QWidget>

namespace Ui {
class LrcWidget;
}

class LrcWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LrcWidget(QWidget *parent = nullptr);
    ~LrcWidget();

private slots:
    void on_hideBtn_clicked();

private:
    Ui::LrcWidget *ui;
};

#endif // LRCWIDGET_H
