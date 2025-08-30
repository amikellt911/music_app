#include "commonpage.h"
#include "ui_commonpage.h"
#include "listitembox.h"
#include <QFileDialog>
#include <QListWidgetItem>
#include <QList>
#include <QDebug>

CommonPage::CommonPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommonPage),
    m_musicList(nullptr)  // 显式初始化 musicList 指针
{
    ui->setupUi(this);

    // 初始化音乐列表QListWidget
    ui->musicList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->musicList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->musicList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->musicList->setFrameShape(QFrame::NoFrame);  // 移除边框
    ui->musicList->setHorizontalScrollMode(QAbstractItemView::ScrollPerItem);
    ui->musicList->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);

    // 初始化成员容器（确保它们被正确初始化）
    m_musicListItems.clear();
    m_listItemBoxes.clear();
    
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
    path+="/client/resources/music/";
    dir.cd(path);
    fileDialog->setDirectory(dir);
    fileDialog->setFileMode(QFileDialog::ExistingFiles);
    fileDialog->setNameFilter(tr("Music Files(*.mp3 *.wav *.flac)"));
    //fileDialog->setMimeTypeFilters();
    if(fileDialog->exec()==QDialog::Accepted){
        m_musicList->addMusicByUrl(fileDialog->selectedUrls());
    }
}

void CommonPage::onMusicListUpdated()
{
    updateMusicListDisplay();
}

void CommonPage::updateMusicListDisplay()
{
    qDebug() << "[DEBUG] updateMusicListDisplay() called - this:" << this;

    if (!m_musicList) {
        qDebug() << "[DEBUG] m_musicList is null, returning";
        return;
    }

    qDebug() << "[DEBUG] Before clearing display - m_musicListItems size:" << m_musicListItems.size();

    // 先清空当前显示
    clearMusicListDisplay();

    qDebug() << "[DEBUG] After clearing display - ui->musicList count:" << ui->musicList->count();

    // 获取当前的音乐列表
    const QVector<Music>& musicList = m_musicList->getMusicList();

    qDebug() << "[DEBUG] Music list size:" << musicList.size();

    // 为每个音乐创建ListItemBox控件并添加到QListWidget
    for (const Music& music : musicList) {
        if(!music.getMusicLike()&&m_pageType==PageType::like)
            continue;
        if(!music.getMusicHistory()&&m_pageType==PageType::recent)
            continue;
        // 创建ListItemBox控件
        ListItemBox *listItemBox = new ListItemBox(ui->musicList);

        // 设置音乐信息
        QString author = music.getMusicAuthor();
        if (author.isEmpty()) {
            author = "未知艺术家";
        }

        QString album = music.getMusicAlbum();
        if (album.isEmpty()) {
            album = "未知专辑";
        }

        listItemBox->setMusicInfo(
            music.getMusicName(),
            author,
            album,
            music.getMusicDuration(),
            music.getMusicLike()
        );
        listItemBox->setId(music.getMusicId());
        // 创建QListWidgetItem并设置为适当的大小
        QListWidgetItem *item = new QListWidgetItem(ui->musicList);
        // 设置ListItemBox的固定高度（与ListItemBox的高度匹配）
        item->setSizeHint(QSize(0, 45));  // 0意味着自适应宽度，45是固定的高度

        // 将ListItemBox设置为QListWidgetItem的widget
        ui->musicList->setItemWidget(item, listItemBox);
        connect(listItemBox, &ListItemBox::likeBtnClicked, this,[this, listItemBox](){
            QString id=listItemBox->getId();
            //m_musicList->musicMap[id]=
            if(m_musicList)
            m_musicList->updateMusicListLikes(id,listItemBox->getIsLike());
        },Qt::UniqueConnection); 
 
        // 存储引用以便清理
        m_musicListItems.append(item);
        m_listItemBoxes.append(listItemBox);
    }

    qDebug() << "[DEBUG] Successfully created" << m_musicListItems.size() << "items";

    // 刷新QListWidget显示
    ui->musicList->update();
    ui->musicList->repaint();

    qDebug() << "[DEBUG] UI updated, final count:" << ui->musicList->count();
}

void CommonPage::clearMusicListDisplay()
{
    // qDebug() << "[DEBUG] Clearing music list display - before clear:"
    //          << "items:" << m_musicListItems.size()
    //          << "boxes:" << m_listItemBoxes.size()
    //          << "widget count:" << ui->musicList->count();

    // 重要：先清空QListWidget中的所有项
    // 这个操作会自动清理所有QListWidgetItem和它们对应的widget
    ui->musicList->clear();

    //qDebug() << "[DEBUG] After QListWidget::clear() - widget count:" << ui->musicList->count();

    // 清理我们的引用列表（QListWidgetItem由QListWidget管理）
    m_musicListItems.clear();

    // ListItemBox控件会由Qt自动管理，因为它们是QListWidgetItem的widget
    // 我们只需要清理引用即可（避免悬空指针）
    m_listItemBoxes.clear();

    // qDebug() << "[DEBUG] Clear operation completed"
    //          << "- final state: items:" << m_musicListItems.size()
    //          << "boxes:" << m_listItemBoxes.size()
    //          << "widget count:" << ui->musicList->count();
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
    //防御性编程
    // // 如果已经有音乐列表连接，先断开之前的连接
    // if (m_musicList) {
    //     disconnect(m_musicList.get(), &MusicList::musicListUpdated,
    //                this, &CommonPage::onMusicListUpdated);
    // }

    m_musicList = musicList;

    // 连接新的音乐列表更新信号
    if (m_musicList) {
        connect(m_musicList.get(), &MusicList::musicListUpdated,
                this, &CommonPage::onMusicListUpdated,
                Qt::UniqueConnection);  // 防止重复连接

        // // 初始化显示当前音乐列表
        // updateMusicListDisplay();
    }
    connectMusicListSignals();
}

void CommonPage::connectMusicListSignals()
{
    if (m_musicList) {
        // 连接全局音乐列表的信号到当前页面的槽函数
        connect(m_musicList.get(), &MusicList::musicListLikeUpdated,
                this, &CommonPage::onMusicLikeUpdated, Qt::UniqueConnection);
    }
}

void CommonPage::onMusicLikeUpdated(const QString& musicId, bool isLiked)
{
    qDebug() << "[DEBUG] onMusicLikeUpdated called - musicId:" << musicId 
             << "isLiked:" << isLiked << "this:" << this;
    
    if(m_pageType!=PageType::like)
    {
            // 遍历当前页面的所有ListItemBox，找到对应的音乐项并更新
    for (int i = 0; i < ui->musicList->count(); ++i) {
        QListWidgetItem *item = ui->musicList->item(i);
        if (!item) continue;
        
        ListItemBox *listItemBox = qobject_cast<ListItemBox*>(
            ui->musicList->itemWidget(item));
        
        if (listItemBox && listItemBox->getId() == musicId) {
            // 找到对应的音乐项，更新其点赞状态
            // 注意：这里需要临时阻断信号，避免循环触发
            bool oldState = listItemBox->blockSignals(true);
            listItemBox->setLikeState(isLiked);  // 您需要在ListItemBox中实现这个方法
            listItemBox->blockSignals(oldState);
            
            qDebug() << "[DEBUG] Updated ListItemBox like state for ID:" << musicId;
            break;  // 找到了就退出循环
        }
    }
    }else 
    {
        // 喜欢页面：需要动态添加/移除项目
        if (isLiked) {
            // 点赞了，需要添加到喜欢列表
            addMusicItemToLikeList(musicId);
        } else {
            // 取消点赞，需要从喜欢列表移除
            removeMusicItemFromLikeList(musicId);
        }
    }
}

// 添加音乐项到喜欢列表
void CommonPage::addMusicItemToLikeList(const QString& musicId)
{
    // 首先检查是否已经存在，避免重复添加
    for (int i = 0; i < ui->musicList->count(); ++i) {
        QListWidgetItem *item = ui->musicList->item(i);
        if (!item) continue;
        
        ListItemBox *listItemBox = qobject_cast<ListItemBox*>(
            ui->musicList->itemWidget(item));
        
        if (listItemBox && listItemBox->getId() == musicId) {
            qDebug() << "[DEBUG] Music item already exists in like list:" << musicId;
            return;  // 已经存在，不需要添加
        }
    }
    
    // 从全局音乐列表中找到对应的音乐数据
    if (!m_musicList) {
        qDebug() << "[ERROR] m_musicList is null";
        return;
    }
    
    const QVector<Music>& musicList = m_musicList->getMusicList();
    const Music* targetMusic = nullptr;
    
    for (const Music& music : musicList) {
        if (music.getMusicId() == musicId) {
            targetMusic = &music;
            break;
        }
    }
    
    if (!targetMusic) {
        qDebug() << "[ERROR] Music not found in global list:" << musicId;
        return;
    }
    
    qDebug() << "[DEBUG] Adding music to like list:" << targetMusic->getMusicName();
    
    // 创建新的ListItemBox
    ListItemBox *listItemBox = new ListItemBox(ui->musicList);
    
    // 设置音乐信息
    QString author = targetMusic->getMusicAuthor();
    if (author.isEmpty()) {
        author = "未知艺术家";
    }
    
    QString album = targetMusic->getMusicAlbum();
    if (album.isEmpty()) {
        album = "未知专辑";
    }
    
    listItemBox->setMusicInfo(
        targetMusic->getMusicName(),
        author,
        album,
        targetMusic->getMusicDuration(),
        true  // 在喜欢页面，肯定是liked状态
    );
    listItemBox->setId(targetMusic->getMusicId());
    
    // 创建QListWidgetItem
    QListWidgetItem *item = new QListWidgetItem(ui->musicList);
    item->setSizeHint(QSize(0, 45));
    
    // 设置ItemWidget
    ui->musicList->setItemWidget(item, listItemBox);
    
    // 连接信号
    connect(listItemBox, &ListItemBox::likeBtnClicked, this,
            [this, listItemBox](){
                QString id = listItemBox->getId();
                if(m_musicList) {
                    m_musicList->updateMusicListLikes(id, listItemBox->getIsLike());
                }
            }, Qt::UniqueConnection);
    
    // 存储引用
    m_musicListItems.append(item);
    m_listItemBoxes.append(listItemBox);
    
    qDebug() << "[DEBUG] Successfully added music item to like list, total count:" << ui->musicList->count();
}

// 从喜欢列表移除音乐项
void CommonPage::removeMusicItemFromLikeList(const QString& musicId)
{
    qDebug() << "[DEBUG] Removing music from like list:" << musicId;
    
    // 遍历列表，找到对应的项目并移除
    for (int i = 0; i < ui->musicList->count(); ++i) {
        QListWidgetItem *item = ui->musicList->item(i);
        if (!item) continue;
        
        ListItemBox *listItemBox = qobject_cast<ListItemBox*>(
            ui->musicList->itemWidget(item));
        
        if (listItemBox && listItemBox->getId() == musicId) {
            qDebug() << "[DEBUG] Found music item to remove at index:" << i;
            
            // 从我们的引用列表中移除
            m_musicListItems.removeAt(i);
            m_listItemBoxes.removeAt(i);
            
            // 从QListWidget中移除（这会自动删除item和widget）
            delete ui->musicList->takeItem(i);
            
            qDebug() << "[DEBUG] Successfully removed music item, remaining count:" << ui->musicList->count();
            break;
        }
    }
}

void CommonPage::setPageType(PageType type)
{
    m_pageType = type;
}

PageType CommonPage::getPageType() const
{
    return m_pageType;
}