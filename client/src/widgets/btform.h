#ifndef BTFORM_H
#define BTFORM_H

#include <QWidget>

namespace Ui {
class BtForm;
}

class BtForm : public QWidget
{
    Q_OBJECT

public:
    explicit BtForm(QWidget *parent = nullptr);
    ~BtForm();
    void setIconAndText(const QString &icon,const QString &text);
    void setIcon(const QString &icon);
    void setId(int id);
    void revertBtForm(const QString &icon);
private:
    Ui::BtForm *ui;
    int id;
protected:
    void mousePressEvent(QMouseEvent *event);
signals:
    void btClicked(int id);
};

#endif // BTFORM_H
