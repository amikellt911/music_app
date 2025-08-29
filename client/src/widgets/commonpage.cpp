#include "commonpage.h"
#include "ui_commonpage.h"
#include "listitembox.h"
#include <QFileDialog>
CommonPage::CommonPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommonPage)
{
    ui->setupUi(this);
}

CommonPage::~CommonPage()
{
    delete ui;
}

void CommonPage::setCommonPageUi(const QString &text, const QString &imagePath)
{
    ui->commonText->setText(text);
    ui->commonText->setIndent(5);  // 向右缩进 5px,让文字与图片对齐
    ui->commonText->setFont(QFont("Microsoft YaHei", 13));
    ui->cover->setPixmap(QPixmap(imagePath));
    ui->cover->setScaledContents(true);

    //测试添加
    ListItemBox *listItemBox = new ListItemBox(this);
    QListWidgetItem *item = new QListWidgetItem(ui->musicList);
    item->setSizeHint(QSize(listItemBox->width(), listItemBox->height()));
    ui->musicList->setItemWidget(item, listItemBox);
}

void CommonPage::setAddLocalIcon(const QString &imagePath)
{
    ui->addLocalBtn->setIcon(QIcon(imagePath));
    ui->addLocalBtn->setIconSize(QSize(ui->addLocalBtn->width(), ui->addLocalBtn->height()));
}
void CommonPage::on_addLocalBtn_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle("添加本地音乐");
    QDir dir=QDir::currentPath();
    dir.cdUp();
    QString path = dir.absolutePath();
    fileDialog->setDirectory(dir);
    fileDialog->setFileMode(QFileDialog::ExistingFiles);
    fileDialog->setNameFilter(tr("Music Files(*.mp3 *.wav *.flac)"));
    //fileDialog->setMimeTypeFilters();
    if(fileDialog->exec()==QDialog::Accepted){
        m_musicList->addMusicByUrl(fileDialog->selectedUrls());
    }
}

void CommonPage::setAddLocalIconUnused()
{
    ui->addLocalBtn->setEnabled(false);
}

void CommonPage::setAddLocalIconHover()
{
    ui->addLocalBtn->setStyleSheet("#addLocalBtn:hover{background-color: #1ecd97;border:none;border-radius: 5px;}"
    "#addLocalBtn{ border:none;border-radius: 5px;}");
}

void CommonPage::setMusicList(const std::shared_ptr<MusicList>& musicList)
{
    m_musicList=musicList;
}