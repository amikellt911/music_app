// databaseworker.cpp
#include "databaseworker.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>


DatabaseWorker::DatabaseWorker(QObject *parent) : QObject(parent)
{
    // 不要在构造函数中创建数据库连接，因为它此时还在主线程
}

DatabaseWorker::~DatabaseWorker()
{
    // 清理资源
}

void DatabaseWorker::setupDatabase()
{
    // 这个函数在后台线程中被调用，所以这里的操作都是线程安全的。

    // 步骤 1: "添加"一个数据库连接到 Qt 的连接池中。
    // 我们给这个连接起一个名字 "db_worker_connection"，这样以后就可以通过名字找到它。
    // 注意：addDatabase 返回的是一个 QSqlDatabase 的值对象，而不是指针。
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "db_worker_connection");
    
    // 步骤 2: 配置这个连接。
    db.setDatabaseName("music.db");

    // 步骤 3: 打开数据库。如果文件不存在，SQLite会自动创建。
    if (!db.open()) {
        // 关键：不要在这里弹窗，而是发出信号通知主线程。
        qWarning() << "数据库打开失败:" << db.lastError().text();
        emit errorOccurred("数据库打开失败: " + db.lastError().text());
        return;
    }
    
    qDebug() << "数据库连接在工作线程中成功建立...";

    // 步骤 4: 创建一个查询对象，并将其与我们的数据库连接 'db' 关联起来。
    // 这是非常重要的一步！
    QSqlQuery query(db);

    QString sql = "CREATE TABLE IF NOT EXISTS music_info ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT, " // 使用 INTEGER 更标准
                  "mid VARCHAR(200) UNIQUE, "
                  "mname VARCHAR(100), "
                  "mauthor VARCHAR(50), "
                  "malbum VARCHAR(50), "
                  "mduration INT, "
                  "murl VARCHAR(200), "
                  "mlike INT, "
                  "mhistory INT"
                  ");";

    // 步骤 5: 执行建表语句。
    if (!query.exec(sql)) {
        qWarning() << "创建数据库表失败:" << query.lastError().text();
        emit errorOccurred("创建数据库表失败: " + query.lastError().text());
        return;
    }

    qDebug() << "数据库表检查/创建成功";
    getMusicInfo();
}


void DatabaseWorker::executeSqlTask(const QString &sql, const QVariant &param)
{
    qDebug() << "Worker thread received task:" << sql;

    QSqlDatabase db = QSqlDatabase::database("db_worker_connection");
    QSqlQuery query(db);
    query.prepare(sql);

    if (param.isValid()) {
        // 检查传入的 QVariant 是否能被转换成一个列表
        if (param.canConvert<QVariantList>()) {
            QVariantList paramList = param.toList();
            // 遍历列表，将每个元素依次绑定到占位符上
            for (int i = 0; i < paramList.size(); ++i) {
                query.bindValue(i, paramList.at(i));
            }
        } else {
            // 如果不是列表，就当作单个参数处理（为了兼容性）
            query.bindValue(0, param);
        }
    }

    if (query.exec()) {
        qDebug() << "Task executed successfully.";
        emit taskFinished(true, "操作成功");
    } else {
        qWarning() << "Task execution failed:" << query.lastError().text();
        emit taskFinished(false, query.lastError().text());
    }
}

void DatabaseWorker::onCheckMusic(const QString &mid, const QUrl &url)
{
    QSqlDatabase db = QSqlDatabase::database("db_worker_connection");
    // 检查数据库连接是否仍然有效
    if (!db.isOpen()) {
        qWarning() << "数据库连接无效，无法检查音乐。";
        emit errorOccurred("数据库连接无效");
        return;
    }

    QSqlQuery query(db);
    // 修正：你的表名是 music_info
    query.prepare("SELECT murl FROM music_info WHERE mid = ?");
    query.bindValue(0, mid);

    if (!query.exec()) {
        // 如果查询本身就失败了，直接报错
        qWarning() << "检查音乐是否存在时查询失败:" << query.lastError().text();
        emit errorOccurred("检查音乐是否存在时查询失败: " + query.lastError().text());
        return;
    }

    if (query.next()) {
        // --- 情况一：音乐已存在于数据库中 ---
        qDebug() << "音乐已存在 (mid):" << mid;
        emit ifCheckMusic(false, url);

        // 逻辑扩展：检查文件路径是否已更改
        QString oldUrl = query.value(0).toString();
        QString newUrl = url.toString();
        if (oldUrl != newUrl) {
            qDebug() << "文件路径已更改，正在更新...";
            query.prepare("UPDATE music_info SET murl = ? WHERE mid = ?");
            query.bindValue(0, newUrl);
            query.bindValue(1, mid);
            if (!query.exec()) {
                qWarning() << "更新音乐路径失败:" << query.lastError().text();
                emit errorOccurred("更新音乐路径失败: " + query.lastError().text());
            } else {
                qDebug() << "音乐路径更新成功。";
            }
        }

    } else {
        // --- 情况二：音乐不存在，需要插入新记录 ---
        qDebug() << "新音乐，准备插入数据库 (mid):" << mid;
        emit ifCheckMusic(true, url);

        // 1. 创建Music对象以解析元数据
        Music music(url);

        // 2. 准备INSERT语句
        query.prepare("INSERT INTO music_info (mid, mname, mauthor, malbum, mduration, murl, mlike, mhistory) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");

        // 3. 绑定所有数据
        query.bindValue(0, music.getMusicId());       // mid (来自文件哈希)
        query.bindValue(1, music.getMusicName());     // mname
        query.bindValue(2, music.getMusicAuthor());   // mauthor
        query.bindValue(3, music.getMusicAlbum());    // malbum
        query.bindValue(4, music.getMusicDuration()); // mduration
        query.bindValue(5, music.getMusicUrl().toString()); // murl
        query.bindValue(6, music.getMusicLike());     // mlike (bool会自动转为0或1)
        query.bindValue(7, music.getMusicHistory());  // mhistory (bool会自动转为0或1)

        // 4. 执行插入
        if (query.exec()) {
            qDebug() << "新音乐插入成功:" << music.getMusicName();
            // 可以选择在这里也发一个taskFinished信号
            emit taskFinished(true, "新音乐添加成功");
        } else {
            qWarning() << "新音乐插入失败:" << query.lastError().text();
            emit errorOccurred("新音乐插入失败: " + query.lastError().text());
        }
    }
}

void DatabaseWorker::getMusicInfo()
{
    QSqlDatabase db = QSqlDatabase::database("db_worker_connection");
    // 检查数据库连接是否仍然有效
    if (!db.isOpen()) {
        qWarning() << "数据库连接无效，无法检查音乐。";
        emit errorOccurred("数据库连接无效");
        return;
    }

    QSqlQuery query(db);
    QString sql = "SELECT mid, mname, mauthor, malbum, mduration, murl, mlike, mhistory FROM music_info";
    
    if (!query.exec(sql)) {
        qWarning() << "getMusicInfo 查询失败:" << query.lastError().text();
        emit errorOccurred("查询音乐列表失败: " + query.lastError().text());
        return;
    }

    qDebug() << "开始从数据库加载音乐列表...";

    // 遍历查询结果的每一行
    while (query.next()) {
        // 创建一个空的 Music 对象，我们将手动填充它的数据
        Music music;

        // 从查询结果中，按列名或索引获取数据，并设置到 music 对象中
        // 使用列名 (如 query.value("mid")) 更健壮，不易因表结构顺序改变而出错
        music.setMusicId(query.value("mid").toString()); // 假设你有 setMusicId
        music.setMusicName(query.value("mname").toString());
        music.setMusicAuthor(query.value("mauthor").toString());
        music.setMusicAlbum(query.value("malbum").toString());
        music.setMusicDuration(query.value("mduration").toLongLong());
        music.setMusicUrl(QUrl(query.value("murl").toString()));
        music.setMusicLike(query.value("mlike").toBool());
        music.setMusicHistory(query.value("mhistory").toBool());
        
        // 数据填充完毕后，通过信号将这个完整的 Music 对象发送出去
        emit initMusicInfo(music);
    }
    
    qDebug() << "音乐列表加载完成。";
}