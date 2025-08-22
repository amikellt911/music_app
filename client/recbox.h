#ifndef RECBOX_H
#define RECBOX_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>
namespace Ui {
class RecBox;
}

class RecBox : public QWidget
{
    Q_OBJECT

public:
    explicit RecBox(QWidget *parent = nullptr);
    ~RecBox();
    void initRecBoxUi(QJsonArray data, int row);
private:
    void createRecBoxItem();
private:
    Ui::RecBox *ui;
    int row;   // 记录当前RecBox实际总⾏数 
    int col;   // 记录当前RecBox实际每⾏有⼏个元素 
    QJsonArray imageList;  // 保存界⾯上的图⽚, ⾥⾯实际为key、value键值对 
};

#endif // RECBOX_H
