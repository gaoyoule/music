#include "musicset.h"
#include "ui_musicset.h"

MusicSet::MusicSet(QWidget *parent) :
    QWidget(parent),m_timer(new QTimer),
    ui(new Ui::MusicSet),m_closeType(1)
{
    ui->setupUi(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(slotExitApplicaton()));
}

MusicSet::~MusicSet()
{
    delete ui;
}

//点击设置按钮
void MusicSet::on_pushButtonSet_clicked()
{
    if(ui->pushButtonSet->text() == "设置")
    {
        //获取设置的小时、分钟、秒
        int hour = ui->spinBoxHour->value();
        int min  = ui->spinBoxMin->value();
        int sec = ui->spinBoxSec->value();
        m_timer->start((hour*3600+min*60+sec)*1000);
        ui->pushButtonSet->setText("取消设置");
        QMessageBox::warning(this,"设置提示","定时关闭设置成功!!");
    }
    else
    {
        m_timer->stop();//停止定时
        ui->pushButtonSet->setText("设置");
    }
}
//退出应用程序
void MusicSet::slotExitApplicaton()
{
    qApp->exit();
}

//点击最小化
void MusicSet::on_radioButtonMin_clicked()
{
    m_closeType = 0;//最小化
}

//点击退出
void MusicSet::on_radioButtonExit_clicked()
{
    m_closeType = 1;//直接结束程序
}
