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
private slots:

    void on_btLeft_clicked();

    void on_btRight_clicked();

private:
    //Carousel是轮播的意思
    void updateCarousel();
private:
    Ui::RecBox *ui;
    int row;   // 记录当前RecBox实际总⾏数 
    int col;   // 记录当前RecBox实际每⾏有⼏个元素 
    QJsonArray imageList;  // 保存界⾯上的图⽚, ⾥⾯实际为key、value键值对 
    int currentPage;;//记录当前第几页
    int totalPage;//记录总共页数
};

#endif // RECBOX_H
