#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <Phonon>
#include <phonon/audiooutput.h>
#include <phonon/seekslider.h>
#include <phonon/mediaobject.h>
#include <phonon/volumeslider.h>
#include <phonon/backendcapabilities.h>
#include <QList>

class QAction;
class QTableWidget;
class QLCDNumber;

class QGraphicsView;
class QGameController;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    //MainWindow(QWidget *parent = 0);
    //~MainWindow();
    MainWindow();
    QSize sizeHint() const
    {
        return QSize(500,300);
    }

private slots:
    void addFiles();
    //void about();
    void stateChanged(Phonon::State newState,Phonon::State oldState);
    void tick(qint64 time);
    void sourceChanged(const Phonon::MediaSource &source);
    void metaStateChanged(Phonon::State newState,Phonon::State oldState);
    void aboutToFinish();
    void tableClicked(int row,int column);

private:
    void setupActions();
    void setupMenus();
    void setupUi();
    //QSlider提供垂直或水平的滑动条
    Phonon::SeekSlider *seekSlider;
    Phonon::MediaObject *mediaObject;
    Phonon::MediaObject *metaInformationResolver;
    Phonon::AudioOutput *audioOutput;
    Phonon::VolumeSlider *volumeSlider;
    QList<Phonon::MediaSource> sources;

    QAction *playAction;
    QAction *pauseAction;
    QAction *stopAction;
    QAction *nextAction;
    QAction *previousAction;
    QAction *addFilesAction;
    QAction *exitAction;
    //QAction *aboutAction;
    //QAction *aboutQtAction;
    QLCDNumber *timeLcd;
    //表格视图
    //需要对应的成员函数
    QTableWidget *musicTable;
    //QTableView *musicTable;
    //成员函数
    //rowCountChanged

    void createStatusBar();

};

#endif // MAINWINDOW_H
