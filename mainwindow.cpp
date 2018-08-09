#include "mainwindow.h"
#include <QtGui>
#include <QDebug>

//MainWindow::MainWindow(QWidget *parent)
//    : QMainWindow(parent)
//{
//}

//MainWindow::~MainWindow()
//{
    
//}
//
MainWindow::MainWindow()
{
    //VedioWidget视频播放
    //声音播放
    audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory,this);
    mediaObject = new Phonon::MediaObject(this);
    metaInformationResolver = new Phonon::MediaObject(this);
    //设置Interval为1000
    mediaObject->setTickInterval(1000);
    //信号槽
    connect(mediaObject,SIGNAL(tick(qint64)),this,SLOT(tick(qint64)));
    //播放状态
    connect(mediaObject,SIGNAL(stateChanged(Phonon::State,Phonon::State)),this,
            SLOT(stateChanged(Phonon::State,Phonon::State)));
    connect(metaInformationResolver,SIGNAL(stateChanged(Phonon::State,Phonon::State)),
            this,SLOT(metaStateChanged(Phonon::State,Phonon::State)));
    connect(mediaObject,SIGNAL(currentSourceChanged(Phonon::MediaSource)),this,
            SLOT(sourceChanged(Phonon::MediaSource)));
    connect(mediaObject,SIGNAL(aboutToFinish()),this,SLOT(aboutToFinish()));

    Phonon::createPath(mediaObject,audioOutput);
    //创建动作
    setupActions();
    //创建菜单
    setupMenus();
    setupUi();

    createStatusBar();

    timeLcd->display("00:00");
}

void MainWindow::addFiles()
{
    //对话框
    //QFileDialog选择文件或目录
    QStringList files = QFileDialog::getOpenFileNames(this,tr("Select Music Files"),
                                                      QDesktopServices::storageLocation(QDesktopServices::MusicLocation));
    //files.isEmpty!=0;
    if(files.isEmpty())
        return;
    int index = sources.size();
    foreach(QString string,files)
    {
        Phonon::MediaSource source(string);
        sources.append(source);
    }
    //sources.isEmpty==0;
    if(!sources.isEmpty())
        metaInformationResolver->setCurrentSource(sources.at(index));
}
//设置播放按钮状态
void MainWindow::stateChanged(Phonon::State newState, Phonon::State /*oldState*/)
{
    switch(newState)
    {
        //detail
        case Phonon::ErrorState:
            if(mediaObject->errorType() == Phonon::FatalError)
                QMessageBox::warning(this,tr("Fata Error"),mediaObject->errorString());
            else
                QMessageBox::warning(this,tr("Error"),mediaObject->errorString());
            break;
        //newState==PlayingState,
        case Phonon::PlayingState:
            playAction->setEnabled(false);
            pauseAction->setEnabled(true);
            stopAction->setEnabled(true);
            break;
        //newState==StoppedState,
        case Phonon::StoppedState:
            stopAction->setEnabled(false);
            playAction->setEnabled(true);
            pauseAction->setEnabled(false);
            //timeshow
            timeLcd->display("00:00");
            break;
        case Phonon::PausedState:
            pauseAction->setEnabled(false);
            playAction->setEnabled(true);
            stopAction->setEnabled(true);
            break;
        case Phonon::BufferingState:
            break;
        default:
            ;
    }
}
//impro
void MainWindow::tick(qint64 time)
{
    //需要优化播放显示的时间
    //bug
    QTime displayTime(0,(time/6000)%60,(time/6000)%60);
    timeLcd->display(displayTime.toString("mm:ss"));
    qDebug("show false time");
}

//cellPress
void MainWindow::tableClicked(int row, int /*column*/)
{
    bool wasPlaying = mediaObject->state() == Phonon::PlayingState;
    mediaObject->stop();
    //清除媒体队列
    mediaObject->clearQueue();
    if(row>=sources.size())
        return;
    mediaObject->setCurrentSource(sources[row]);
    if(wasPlaying)
        mediaObject->play();
    else
        mediaObject->stop();
}

void MainWindow::sourceChanged(const Phonon::MediaSource &source)
{
    musicTable->selectRow(sources.indexOf(source));
    timeLcd->display("00:00");
}

void MainWindow::metaStateChanged(Phonon::State newState, Phonon::State /*oldState*/)
{
    if(newState == Phonon::ErrorState)
    {
        QMessageBox::warning(this,tr("Error opening Files"),
                             metaInformationResolver->errorString());
        while(!sources.isEmpty()&&!(sources.takeLast() == metaInformationResolver->currentSource())){};
        return;
    }
    if(newState != Phonon::StoppedState && newState != Phonon::PausedState)
        return;
    if(metaInformationResolver->currentSource().type() == Phonon::MediaSource::Invalid)
        return;
    QMap<QString,QString>metaData = metaInformationResolver->metaData();
    QString title = metaData.value("TITLE");
    if(title == "")
        title = metaInformationResolver->currentSource().fileName();
    QTableWidgetItem *titleItem = new QTableWidgetItem(title);
    titleItem->setFlags(titleItem->flags()^Qt::ItemIsEditable);
    QTableWidgetItem *artisItem = new QTableWidgetItem(metaData.value("ARTIST"));
    artisItem->setFlags(artisItem->flags()^Qt::ItemIsEditable);
    QTableWidgetItem *albumItem = new QTableWidgetItem(metaData.value("ALBUM"));
    albumItem->setFlags(albumItem->flags()^Qt::ItemIsEditable);
    QTableWidgetItem *yearItem = new QTableWidgetItem(metaData.value("DATE"));
    yearItem->setFlags(yearItem->flags()^Qt::ItemIsEditable);

    //QTableWidget number,insertRow,
    int currentRow = musicTable->rowCount();
    //插入空行
    musicTable->insertRow(currentRow);
    //显示title,artis,album,year
    musicTable->setItem(currentRow,0,titleItem);
    musicTable->setItem(currentRow,1,artisItem);
    musicTable->setItem(currentRow,2,albumItem);
    musicTable->setItem(currentRow,3,yearItem);

    if(musicTable->selectedItems().isEmpty())
    {
        musicTable->selectRow(0);
        mediaObject->setCurrentSource(metaInformationResolver->currentSource());
    }
    //Phonon框架
    Phonon::MediaSource source = metaInformationResolver->currentSource();
    int index = sources.indexOf(metaInformationResolver->currentSource())+1;
    if(sources.size()>index)
        metaInformationResolver->setCurrentSource(sources.at(index));
    else
    {
        musicTable->resizeColumnsToContents();
        if(musicTable->columnWidth(0)>300)
            musicTable->setColumnWidth(0,300);
    }
}

void MainWindow::aboutToFinish()
{
    int index = sources.indexOf(mediaObject->currentSource())+1;
    if(sources.size()>index)
        mediaObject->enqueue(sources.at(index));
}

void MainWindow::setupActions()
{
    playAction = new QAction(style()->standardIcon(QStyle::SP_MediaPlay),tr("play"),this);
    playAction->setShortcut(tr("Ctrl+P"));
    playAction->setDisabled(true);
    pauseAction = new QAction(style()->standardIcon(QStyle::SP_MediaPause),tr("pause"),this);
    pauseAction->setShortcut(tr("Ctrl+A"));
    pauseAction->setDisabled(true);
    stopAction = new QAction(style()->standardIcon(QStyle::SP_MediaStop),tr("stop"),this);
    stopAction->setShortcut(tr("Ctrl+S"));
    stopAction->setDisabled(true);
    nextAction = new QAction(style()->standardIcon(QStyle::SP_MediaSkipForward),tr("Next"),this);
    nextAction->setShortcut(tr("Ctrl+N"));
    nextAction->setDisabled(true);
    previousAction = new QAction(style()->standardIcon(QStyle::SP_MediaSeekBackward),tr("Previous"),this);
    previousAction->setShortcut(tr("Ctrl+R"));
    addFilesAction = new QAction(tr("Add Files"),this);
    addFilesAction->setShortcut(tr("Ctrl+F"));
    exitAction = new QAction(tr("&Exit"),this);
    exitAction->setShortcut(QKeySequence::Quit);
    //mediaObject
    connect(playAction,SIGNAL(triggered()),mediaObject,SLOT(play()));
    connect(pauseAction,SIGNAL(triggered()),mediaObject,SLOT(pause()));
    connect(stopAction,SIGNAL(triggered()),mediaObject,SLOT(stop()));
    connect(addFilesAction,SIGNAL(triggered()),this,SLOT(addFiles()));
    connect(exitAction,SIGNAL(triggered()),this,SLOT(close()));
}

void MainWindow::setupMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(addFilesAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);
}

void MainWindow::setupUi()
{
    QToolBar *bar = new QToolBar;
    bar->addAction(playAction);
    bar->addAction(pauseAction);
    bar->addAction(stopAction);
    //滚动条
    seekSlider = new Phonon::SeekSlider(this);
    seekSlider->setMediaObject(mediaObject);
    //音量控制
    volumeSlider = new Phonon::VolumeSlider(this);
    volumeSlider->setAudioOutput(audioOutput);
    volumeSlider->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Minimum);

    QLabel *volumeLabel = new QLabel(tr("Volume"));
    //volumeLabel->setPixmap(QPixmap("images/volume.png"));

    //active,disabled,inactive
    QPalette palette;
    palette.setBrush(QPalette::Light,Qt::darkGray);

    timeLcd = new QLCDNumber;
    timeLcd->setPalette(palette);
    //设置播放详细
    QStringList headers;
    headers<<tr("Title")<<tr("Artist")<<tr("Album")<<tr("Year");
    //播放详细设置成4列
    musicTable = new QTableWidget(0,4);
    //musicTable =new QTableWidget(this);
    //musicTable->setRowCount(5);
    //musicTable->setColumnCount(5);
    musicTable->setHorizontalHeaderLabels(headers);
    musicTable->setSelectionMode(QAbstractItemView::SingleSelection);
    musicTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    //修改为双击播放，单击选定歌曲
    //connect(musicTable,SIGNAL(cellPressed(int,int)),this,SLOT(tableClicked(int,int)));
    connect(musicTable,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(tableClicked(int,int)));

    QHBoxLayout *seekerLayout = new QHBoxLayout;
    //播放进度滑条
    seekerLayout->addWidget(seekSlider);
    //播放时间
    seekerLayout->addWidget(timeLcd);

    QHBoxLayout *playbackLayout = new QHBoxLayout;
    playbackLayout->addWidget(bar);
    playbackLayout->addStretch();
    //volumeLabel
    playbackLayout->addWidget(volumeLabel);
    playbackLayout->addWidget(volumeSlider);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(musicTable);
    mainLayout->addLayout(seekerLayout);
    mainLayout->addLayout(playbackLayout);

    QWidget *widget = new QWidget;
    widget->setLayout(mainLayout);

    setCentralWidget(widget);
    setWindowTitle("ddmusic");
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("lwyhekies@gmail.com"));
}

#if 0
void MainWindow::openFiles()
{
    QString path = QFileDialog::getOpenFileName(this,tr("open file"),".",tr("text files(*.txt"));
    if(!path.isEmpty()){
        QFile file(path);
        if(!file.open(QIODevice::Readonly|QIODevice::Text)){
            QMessageBox::warning(this,tr("read file"),tr("can not open file:\n%1").arg(path));
            return;
        }
        QTextStream in(&file);
        textEdit->setText(in.readAll());
        file.close();
    }
    else{
        QMessageBox::warning(this,tr("path"),tr("you do not select any file"));
    }
}
#endif
