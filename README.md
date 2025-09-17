# 本地音乐播放器

## 项目简介

本项目是一款基于 **C++11** 和 **Qt5** 实现的、功能完备的本地音乐播放器。项目 UI 界面借鉴了主流商业软件的设计，旨在提供**流畅的用户体验**。

## 项目架构

项目结构采用如下**类 MVC 结构**：

```
├── build
└── client
    ├── resources
    ├── src
    │   ├── controller
    │   ├── model
    │   ├── ui
    │   └── widgets
    └── CMakeLists.txt
    └── main.cpp
```

### 目录结构说明

- **build** 文件夹为 cmake 编译文件
- **resources** 文件夹放置的是图片、qss 等 qrc 资源
- **src** 文件夹放置的是源码，分为四个部分：
  - **controller** 模块负责控制管理，比如数据库 music.db 的管理
  - **model** 模块用于管理数据模块，比如 music、musicList 等模块
  - **ui** 模块用于管理 qt 的 ui 文件
  - **widgets** 是页面以及逻辑交互

## 技术栈

### 开发环境
- **版本**：C++11 (mingw1310_64), Qt5.14.2

### 使用的 QT5 模块
- **Qt5::Widgets**: 构建响应式、可定制的用户界面和自定义控件
- **Qt5::Multimedia**: 实现核心的音频播放、控制与状态管理
- **Qt5::Sql**: 通过 `SQLite` 驱动，实现用户播放列表、收藏等数据的本地持久化
- **Qt5::Network**: 通过本地套接字 (`QLocalServer`) 实现应用的单例启动模式

## 详细文件结构

```
└── client
    ├── resources
    ├── src
    │   ├── controller
    │   │   ├── databaseworker.cpp
    │   │   └── databaseworker.h
    │   ├── model
    │   │   ├── lrcwidget.cpp
    │   │   ├── lrcwidget.h
    │   │   ├── mediaplayermanager.cpp
    │   │   ├── mediaplayermanager.h
    │   │   ├── music.cpp
    │   │   ├── music.h
    │   │   ├── musiclist.cpp
    │   │   └── musiclist.h
    │   ├── ui
    │   │   ├── btform.ui
    │   │   ├── client.ui
    │   │   ├── commonpage.ui
    │   │   ├── listitembox.ui
    │   │   ├── lrcwidget.ui
    │   │   ├── progressbar.ui
    │   │   ├── recbox.ui
    │   │   ├── recboxitem.ui
    │   │   └── volumetool.ui
    │   └── widgets
    │       ├── btform.cpp
    │       ├── btform.h
    │       ├── client.cpp
    │       ├── client.h
    │       ├── commonpage.cpp
    │       ├── commonpage.h
    │       ├── listitembox.cpp
    │       ├── listitembox.h
    │       ├── progressbar.cpp
    │       ├── progressbar.h
    │       ├── recbox.cpp
    │       ├── recbox.h
    │       ├── recboxitem.cpp
    │       ├── recboxitem.h
    │       ├── volumetool.cpp
    │       └── volumetool.h
```

## 模块详解

### Controller 模块
- **databaseworker**: 负责数据库的 SQL 语句处理，专门封装在多线程使用，防止 UI 阻塞

### Model 模块
- **lrcwidget**: 负责歌词解析以及歌词页面显示
- **mediaplayermanager**: 音乐播放功能
- **music**: 音乐数据类
- **musiclist**: 音乐列表

### UI 和 Widgets 模块
- **client**: 主页面，最终展示
- **btform**: 负责左侧菜单栏按钮的图片和文字显示
- **commonpage**: 负责我的音乐栏的页面，因为他们有相似的逻辑，所以会把相似的封装在一起
- **listitembox**: 每行音乐的名称、歌手、专辑等信息封装在一栏，放在 commonpage 中
- **progressbar**: 音乐播放进度条
- **recbox**: 推荐界面
- **recboxitem**: 推荐界面的单个选项
- **volumetool**: 音量控件

## 未来展望

### 1. 构建 C/S 架构，实现云同步功能
将项目拓展为 **Client/Server** 模式。利用自研的 **MiniMuduo 高性能网络库**作为服务端底层，封装 **HTTP/RESTful API**，实现用户的"云歌单"、播放记录同步等功能，完成一个**全栈项目**的闭环。

### 2. 集成 ffmpeg，增强解码能力
引入 `ffmpeg` 库代替平台原生解码器，以支持更广泛的音频格式（如 **FLAC、APE**），并实现跨平台解码能力的一致性，摆脱对 K-Lite 等第三方解码包的依赖。

### 3. 完善用户体验
进一步优化 UI 交互，增加**皮肤更换**、**歌词自动搜索与下载**、**歌曲搜索**等高级功能。

## 主要功能展示

### 主界面
目前主界面如下（推荐页面只是为了学习轮播效果而设计的，也是一个占位效果，为后面网络服务端做准备）：

![](readme_img/Pasted%20image%2020250917102259.png)

### 功能使用指南

#### 1. 本地音乐上传
点击本地音乐，并上传音乐

![](readme_img/Pasted%20image%2020250917102502.png)

#### 2. 文件选择
根据弹出的文件夹选择本地音乐（可以选择多首），git 提交的文件夹有这些测试音乐，lrc 歌词文件需要额外下载，这里推荐使用 [ZonyLrcToolsX](https://docs.myzony.com/#/?id=zonylrctoolsx) 下载歌词（还可以下载封面，但是代码使用的是音乐文件的元数据获取的封面）

![](readme_img/Pasted%20image%2020250917102552.png)

#### 3. 音乐加载完成
加载完效果如下：

![](readme_img/Pasted%20image%2020250917103025.png)

#### 4. 音乐播放
双击歌曲播放音乐：

![](readme_img/Pasted%20image%2020250917103108.png)

#### 5. 音量控制
鼠标悬浮在音量按钮可以调节音量，默认静音，支持拖拽，以及点击音量按钮，静音或最大音量切换。

![](readme_img/Pasted%20image%2020250917103151.png)
![](readme_img/Pasted%20image%2020250917103202.png)

#### 6. 播放记录
播放后最近播放会保留记录

![](readme_img/Pasted%20image%2020250917103814.png)

#### 7. 歌词显示
点击歌词按钮会弹出歌词

![](readme_img/Pasted%20image%2020250917103833.png)
![](readme_img/Pasted%20image%2020250917103845.png)

#### 8. 歌词收回
点击下拉按钮收回歌词

![](readme_img/Pasted%20image%2020250917103913.png)

#### 9. 收藏功能
可以点击喜欢，自动同步到我喜欢页面

![](readme_img/Pasted%20image%2020250917104040.png)
![](readme_img/Pasted%20image%2020250917104050.png)

#### 10. 播放模式切换
可以切换播放模式

![](readme_img/Pasted%20image%2020250917104114.png)

#### 11. 切歌功能
可以点击切换上一首下一首

![](readme_img/Pasted%20image%2020250917104207.png)

