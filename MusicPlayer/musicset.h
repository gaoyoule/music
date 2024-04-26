#ifndef MUSICSET_H
#define MUSICSET_H

#include <QWidget>
#include <QTimer>
#include <QMessageBox>
namespace Ui {
class MusicSet;
}

class MusicSet : public QWidget
{
    Q_OBJECT

public:
    explicit MusicSet(QWidget *parent = 0);
    ~MusicSet();

    inline int getCloseType(){return m_closeType;}

private slots:
    void on_pushButtonSet_clicked();
    void slotExitApplicaton();//退出应用程序

    void on_radioButtonMin_clicked();

    void on_radioButtonExit_clicked();

private:
    Ui::MusicSet *ui;
    QTimer *m_timer; //定时关闭的定时器
    int m_closeType; // 用1代表 直接退出程序， 用0代表最小化
};

#endif // MUSICSET_H
