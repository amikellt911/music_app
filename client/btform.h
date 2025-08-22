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

private:
    Ui::BtForm *ui;
};

#endif // BTFORM_H
