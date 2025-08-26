#ifndef LISTITEMBOX_H
#define LISTITEMBOX_H

#include <QWidget>

namespace Ui {
class ListItemBox;
}

class ListItemBox : public QWidget
{
    Q_OBJECT

public:
    explicit ListItemBox(QWidget *parent = nullptr);
    ~ListItemBox();
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::ListItemBox *ui;
};

#endif // LISTITEMBOX_H
