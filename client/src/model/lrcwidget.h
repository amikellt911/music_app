#ifndef LRCWIDGET_H
#define LRCWIDGET_H

#include <QWidget>
#include <QPropertyAnimation>
struct lrcWordLine
{
    lrcWordLine(qint64 times,QString texts)
    :times(times),texts(texts)
    {}
    qint64 times;
    QString texts;
};
namespace Ui {
class LrcWidget;
}

class LrcWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LrcWidget(QWidget *parent = nullptr);
    ~LrcWidget();
    void setLrcUrl(const QString &url,bool available);
    bool parseLrcFile();
    void positionChanged(qint64 position);
    void setAuthor(const QString &author);
    void setTitle(const QString &title);
private slots:
    void on_hideBtn_clicked();

private:
    Ui::LrcWidget *ui;
    QPropertyAnimation *animation;
    QString lrcUrl;
    bool lrcAvailable;
    QVector<lrcWordLine> lrcWordLines;
};

#endif // LRCWIDGET_H
