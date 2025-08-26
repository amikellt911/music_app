#ifndef VOLUMETOOL_H
#define VOLUMETOOL_H

#include <QWidget>

namespace Ui {
class VolumeTool;
}

class VolumeTool : public QWidget
{
    Q_OBJECT

public:
    explicit VolumeTool(QWidget *parent = nullptr);
    ~VolumeTool();
    void setVolumeRatio(int value);
    void setOutSlider(int value);
protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::VolumeTool *ui;
private:
    void initSliderBtnShadow();
};

#endif // VOLUMETOOL_H
