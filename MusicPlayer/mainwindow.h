#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <QMediaPlayer>
#include <QTime>
#include <ctime>
#include "musicset.h"
#include <QCloseEvent>
#include <QSystemTrayIcon> //系统托盘图标头文件


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void showMusicNameStatus();//显示当前歌曲的状态
    //重写重置事件
    void resizeEvent(QResizeEvent* event);//重置事件，当窗口大小发生变化时，会自动执行该函数
    //重写窗口关闭事件
    void closeEvent(QCloseEvent *event);//点击关闭窗口的X,会自动执行该事件
private slots:

    //当加载音乐播放歌曲后，MainWindow会收到durationChanged信号，MainWindow执行的槽函数
    void slotDurationChanged(qint64 duration);
    //歌曲播放之后，进度改变，MianWindow会收到positionChange信号，MainWindow执行的槽函数
    void slotPositionChanged(qint64 position);
    //当激活系统托盘图标后，执行对应的槽函数, 参数是托盘被激活的具体的原因
    void slotActivatedSystemTrayIcon(QSystemTrayIcon::ActivationReason reason);

    void on_toolButtonAddMusic_clicked();

    void on_tableWidget_doubleClicked(const QModelIndex &index);

    void on_tableWidget_clicked(const QModelIndex &index);

    void on_toolButtonPlayOrPause_clicked();

    void on_toolButtonPre_clicked();

    void on_toolButtonNext_clicked();

    void on_toolButtonStop_clicked();

    void on_toolButtonExit_clicked();

    void on_verticalSlider_sliderMoved(int position);

    void on_horizontalSlider_sliderMoved(int position);

    void on_horizontalSlider_valueChanged(int value);

    void on_toolButtonSet_clicked();

private:
    Ui::MainWindow *ui;
    QMediaPlayer *m_mediaPlayer;//音乐媒体播放器对象指针
    QList<QFileInfo> *m_fileInfo; //用来保存所有mp3音乐文件的信息
    int m_currentIndex;//用来记录当前单击选中的歌曲下标
    bool m_isPause;//是否处于暂停状态标志位 true 处于暂停状态  false 处于播放状态
    bool m_isMusicClicked;//判断歌曲是否被单击选中 true 代表点击选中， false代表未点击选中
    qint64 m_duration;//用来记录歌曲的持续时长
    MusicSet *m_musicSet;//定义一个播放器设置界面对象的指针
    QSystemTrayIcon *m_systemTrayIcon;//系统托盘图标对象的指针
};

#endif // MAINWINDOW_H
