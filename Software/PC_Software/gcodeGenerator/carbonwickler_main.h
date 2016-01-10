#ifndef CARBONWICKLER_MAIN_H
#define CARBONWICKLER_MAIN_H

#include <QMainWindow>

namespace Ui {
class carbonwickler_main;
}

class carbonwickler_main : public QMainWindow
{
    Q_OBJECT

public:
    explicit carbonwickler_main(QWidget *parent = 0);
    ~carbonwickler_main();

private slots:
    void on_ButtonQuit_clicked();

    void on_ButtonGenCode_clicked();

    void on_ButtonOpenFile_clicked();

private:
    Ui::carbonwickler_main *ui;
    QString fileOpen();
};

#endif // CARBONWICKLER_MAIN_H
