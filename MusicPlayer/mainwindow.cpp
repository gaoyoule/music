#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),m_mediaPlayer(new QMediaPlayer),m_isMusicClicked(false),m_musicSet(new MusicSet),
    ui(new Ui::MainWindow),m_currentIndex(0),m_isPause(true),m_fileInfo(nullptr),m_duration(0)
{
    ui->setupUi(this);

    m_systemTrayIcon = new QSystemTrayIcon;//创建一个系统托盘对象
    m_systemTrayIcon->setIcon(QIcon(":/images/ico.ico"));//设置系统托盘显示的图标
    m_systemTrayIcon->setToolTip("天天静听");//设置提示
    m_systemTrayIcon->show();//显示系统托盘图标
    connect(m_systemTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(slotActivatedSystemTrayIcon(QSystemTrayIcon::ActivationReason)));

    //初始化随机种子
    srand(time(NULL));

    //设置播放器默认的音量
    m_mediaPlayer->setVolume(ui->verticalSlider->value());

    //绑定信号与槽
    connect(m_mediaPlayer, SIGNAL(durationChanged(qint64)), this, SLOT(slotDurationChanged(qint64)));
    connect(m_mediaPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(slotPositionChanged(qint64)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//显示歌曲文件的状态
void MainWindow::showMusicNameStatus()
{
    if(m_mediaPlayer->state() == QMediaPlayer::PlayingState)
        ui->labelPlayStatus->setText("正在播放:");
    else if(m_mediaPlayer->state() == QMediaPlayer::StoppedState)
        ui->labelPlayStatus->setText("正在停止:");
    else if(m_mediaPlayer->state() == QMediaPlayer::PausedState)
        ui->labelPlayStatus->setText("正在暂停:");
    QStringList list = m_fileInfo->at(m_currentIndex).fileName().split(".");
    ui->labelMusicName->setText(list[0]);
}

//重写重置事件
void MainWindow::resizeEvent(QResizeEvent *event)
{
//    qDebug()<<"窗口大小发生变化了!!!";
    ui->tableWidget->setColumnWidth(0, ui->tableWidget->width());//重新设置歌曲列表的宽度

}

//点击关闭窗口的X,会自动执行该事件
void MainWindow::closeEvent(QCloseEvent *event)
{
   if(m_musicSet->getCloseType())//== 1 说明应该直接结束程序
   {
       qApp->exit();
   }
   else //最小化 == 0
   {
       event->ignore();//忽略该关闭事件
       this->hide();//将当前的窗口隐藏
   }
}

//当加载音乐播放歌曲后，MainWindow会收到durationChanged信号，MainWindow执行的槽函数
void MainWindow::slotDurationChanged(qint64 duration)
{
    //音乐文件被加载，歌曲持续时间: 266712 ,持续的时间是以ms为单位  1s = 1000ms
    //分钟: 266712 / 60000   --->  4分
    //秒  : 266712 % 60000 / 1000  ---> 26秒
    qDebug()<<"音乐文件被加载，歌曲持续时间:" << duration;
    m_duration = duration; //将歌曲持续时间存储成员变量中
    //设置下进度条范围 0 ---- duration
    ui->horizontalSlider->setRange(0, duration);
    //设置水平滑动条可以通过键盘上左右方向键控制
    //参数 duration / 100  每次移动步长
    ui->horizontalSlider->setSingleStep(duration / 100);
}

//歌曲播放之后，进度改变，MianWindow会收到positionChange信号，MainWindow执行的槽函数
void MainWindow::slotPositionChanged(qint64 position)
{
//    qDebug()<<"播放歌曲的时间进度:" << position;
    ui->horizontalSlider->setValue(position);//将歌曲进度位置的值，设置为进度条的值
    //创建一个QTime对象
    QTime t(0, position/60000, position%60000/1000);
    QString temp = t.toString("mm:ss");//格式化出要显示的时间字符串"03:15"
//    qDebug()<<temp;
    ui->labelTime->setText(temp);//显示在控件上
    //判断歌曲是自动播放完毕,滑动条的位置值与播放的进度位置值相同
    if(position == m_duration)
    {
        //自动播放下一曲（播放下一曲，需要判断当前的播放模式）
        //获取当前Combobox选中的播放模式下标
        int index = ui->comboBox->currentIndex();
        if(index == 0)//说明是顺序播放
        {
            m_currentIndex = m_currentIndex ==  m_fileInfo->size()-1 ?  0 : m_currentIndex+1;
        }
        else if(index == 2)//说明是随机播放
        {
            int post;
            do
            {
                post = rand() % m_fileInfo->size();
            }while(post == m_currentIndex);
            //循环结束后，生成的随机位置下标与当前的位置下标不等
            m_currentIndex = post;
        }
        ui->tableWidget->setCurrentCell(m_currentIndex, 0);//设置处于选中状态
        m_mediaPlayer->setMedia(QUrl::fromLocalFile(m_fileInfo->at(m_currentIndex).filePath()));//加载播放文件
        m_isPause ? m_mediaPlayer->stop() : m_mediaPlayer->play();
    }
}

void MainWindow::slotActivatedSystemTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::DoubleClick)//如果激活的原因是鼠标双击托盘
        this->show();//将主界面显示
}

//点击添加音乐按钮执行的槽函数
void MainWindow::on_toolButtonAddMusic_clicked()
{
    //获取音乐文件的所在文件夹的绝对路径 QFileDialog::ShowDirsOnly 对话框中只显示文件夹
    QString musicDirPath = QFileDialog::getExistingDirectory(this, "请选择音乐文件所在的目录","D:\Project\MusicPlayer\music",QFileDialog::ShowDirsOnly);
    qDebug()<<"音乐文件的存放目录"<<musicDirPath;
    if(musicDirPath.isEmpty())
    {
        QMessageBox::warning(this,"选择提示","您忘记选择音乐文件的存放目录，请重新选择!!");
        return;
    }
    //过滤出musicDirPath目录下的所有的.mp3结尾音乐文件的名字
    QDir dir(musicDirPath);//创建一个目录对象
    QStringList filter;//创建一个过滤器
    filter<<"*.mp3";// "*.mp3"添加到filter中，做为过滤条件
    //只要.mp3结尾的文件 dir.entryInfoList(filter)获取目录下的文件列表
    m_fileInfo = new QList<QFileInfo>(dir.entryInfoList(filter));
    //设置tableWidget的行数和列数
    ui->tableWidget->setRowCount(m_fileInfo->size());//设置行数 mp3文件的个数
    ui->tableWidget->setColumnCount(1);//设置列数 1列
    //循环将每个mp3文件的名字，添加到表格中
    for(int i = 0; i < m_fileInfo->size(); i++)
    {
        QStringList list = m_fileInfo->at(i).fileName().split(".");
        // i,0 行列的位置下标
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(list[0]));
//        qDebug()<<m_fileInfo->at(i).fileName();//将每个文件信息中的文件名字打印输出
//        qDebug()<<m_fileInfo->at(i).filePath();//将每个文件信息中的文件的绝对路径打印输出
    }
    //设置tableWidget的效果
    ui->tableWidget->setColumnWidth(0, ui->tableWidget->width());//设置列宽度与tableWidget一样宽
    ui->tableWidget->resizeRowsToContents();   //重置行的大小和文字内容大小一样
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//设置进制编辑
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);//设置选择方式为整行选择
    ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem("歌曲播放列表"));//设置表头
    ui->tableWidget->setCurrentCell(m_currentIndex,0);//默认第0行处于选中状态
    m_mediaPlayer->setMedia(QUrl::fromLocalFile(m_fileInfo->at(m_currentIndex).filePath()));//默认加载第1首歌
    showMusicNameStatus();
}

//双击某个歌曲执行的槽函数
void MainWindow::on_tableWidget_doubleClicked(const QModelIndex &index)
{
    //    qDebug()<<index.row() << index.column();//打印输出双击选中的行和列的下标
    int i = index.row();//获取点击的函数
    //加载要播放的音乐文件
    qDebug()<<m_fileInfo->at(i).filePath();//打印输出双击选中的歌曲的绝对路径
    //fromLocalFile参数需要的是要播放的音乐文件的绝对路径
    m_mediaPlayer->setMedia(QUrl::fromLocalFile(m_fileInfo->at(i).filePath()));
    m_mediaPlayer->play();//开始播放
    ui->toolButtonPlayOrPause->setStyleSheet("background:url(:/images/gtk-media-pause.png);");
    m_isPause = false;//处于播放状态
    showMusicNameStatus();
}

//单击某个歌曲执行的槽函数
void MainWindow::on_tableWidget_clicked(const QModelIndex &index)
{
    //获取鼠标单击的歌曲的行数下标
    m_currentIndex = index.row();
    m_isMusicClicked = true;//说明有歌曲被选中了
}
//点击播放或暂停按钮执行的槽函数
void MainWindow::on_toolButtonPlayOrPause_clicked()
{
    //容错判断
    if(m_fileInfo == nullptr)
    {
        QMessageBox::warning(this,"使用提示","请先导入播放的歌曲列表");
        return;
    }

    if(m_isMusicClicked)
    {
        m_mediaPlayer->setMedia(QUrl::fromLocalFile(m_fileInfo->at(m_currentIndex).filePath()));//加载播放文件
        m_isMusicClicked = false;
    }
    if(m_isPause)//处于暂停状态
    {
        m_mediaPlayer->play();//播放音乐
        m_isPause = false;//处于播放状态
        ui->toolButtonPlayOrPause->setStyleSheet("background:url(:/images/gtk-media-pause.png);"); //切换图片
    }
    else //处于播放状态
    {
        m_mediaPlayer->pause();//暂停音乐
        m_isPause = true;//处于暂停状态
        ui->toolButtonPlayOrPause->setStyleSheet("background:url(:/images/gtk-media-play-ltr.png);");//切换图片
    }
    showMusicNameStatus();
}

//点击上一曲
void MainWindow::on_toolButtonPre_clicked()
{
    //容错判断
    if(m_fileInfo == nullptr)
    {
        QMessageBox::warning(this,"使用提示","请先导入播放的歌曲列表");
        return;
    }
    //先对当前播放的模式进行判断
    int index = ui->comboBox->currentIndex();
    if(index == 0 || index == 1)//顺序播放模式
        m_currentIndex = m_currentIndex == 0 ? m_fileInfo->size()-1 : m_currentIndex-1;
    else if(index == 2)//随机播放模式
    {
        int post;
        do
        {
            post = rand() % m_fileInfo->size();
        }while(post == m_currentIndex);
        //循环结束后，生成的随机位置下标与当前的位置下标不等
        m_currentIndex = post;
    }
    ui->tableWidget->setCurrentCell(m_currentIndex, 0);
    m_mediaPlayer->setMedia(QUrl::fromLocalFile(m_fileInfo->at(m_currentIndex).filePath()));//加载播放文件
    m_isPause ? m_mediaPlayer->stop() : m_mediaPlayer->play();
    showMusicNameStatus();
}
//点击下一曲
void MainWindow::on_toolButtonNext_clicked()
{
    //容错判断
    if(m_fileInfo == nullptr)
    {
        QMessageBox::warning(this,"使用提示","请先导入播放的歌曲列表");
        return;
    }
    //先对当前播放的模式进行判断
    int index = ui->comboBox->currentIndex();
    if(index == 0 || index == 1)//顺序播放模式
        m_currentIndex = m_currentIndex ==  m_fileInfo->size()-1 ?  0 : m_currentIndex+1;
    else if(index == 2)//随机播放模式
    {
        int post;
        do
        {
            post = rand() % m_fileInfo->size();
        }while(post == m_currentIndex);
        //循环结束后，生成的随机位置下标与当前的位置下标不等
        m_currentIndex = post;
    }
    ui->tableWidget->setCurrentCell(m_currentIndex, 0);
    m_mediaPlayer->setMedia(QUrl::fromLocalFile(m_fileInfo->at(m_currentIndex).filePath()));//加载播放文件
    m_isPause ? m_mediaPlayer->stop() : m_mediaPlayer->play();
    showMusicNameStatus();
}

//点击停止
void MainWindow::on_toolButtonStop_clicked()
{
    //容错判断
    if(m_fileInfo == nullptr)
    {
        QMessageBox::warning(this,"使用提示","请先导入播放的歌曲列表");
        return;
    }
    m_mediaPlayer->stop();
    m_isPause = true;
    ui->toolButtonPlayOrPause->setStyleSheet("background:url(:/images/gtk-media-play-ltr.png);");//切换图片
    showMusicNameStatus();
}

//点击退出按钮
void MainWindow::on_toolButtonExit_clicked()
{
    qApp->exit();
}

//当垂直滑动移动的时候，执行的槽函数
void MainWindow::on_verticalSlider_sliderMoved(int position)
{
    m_mediaPlayer->setVolume(position);
    ui->labelVolume->setText(QString("音量:%1").arg(position));
}

//当水平进度条移动执行的槽函数
void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
    //将进度条移动时的位置值，设置为音乐播放的进度
    m_mediaPlayer->setPosition(position);
}

//当进度条的值发生变化时，执行的槽函数
void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    //将进度条变化的位置值，设置为音乐播放的进度
    m_mediaPlayer->setPosition(value);
}

//点击更多设置
void MainWindow::on_toolButtonSet_clicked()
{
    m_musicSet->show();//显示设置界面
}
