#ifndef CARBONWICKLER_MAIN_H
#define CARBONWICKLER_MAIN_H

#include <QMainWindow>
#include <QFile>
#include <QTextStream>

namespace Ui {
class carbonwickler_main;
}

class carbonwickler_main : public QMainWindow
{
    Q_OBJECT

public:
    explicit carbonwickler_main(QWidget *parent = 0);
    ~carbonwickler_main();
static int testmethstatic();
int testmet();
private slots:
    void on_ButtonQuit_clicked();

    int on_ButtonGenCode_clicked();

    void on_ButtonOpenFile_clicked();

    void inputValidatorLineEdit(QString);

    void winkelDataSync(int);

private:
    Ui::carbonwickler_main *ui;
    int fileOpen();
    QFile *outputfile;
    QTextStream *outputstream;
    int writeHeader();
    int generateGCode();
    double d_end;
    double l_gesamt;
    double l_end;
    double l_start;
    double ganghoehe;
    double lagen;
    double feed_normal;
    double feed_slow;
    double rovingbreite;
    double gaenge;
    double x,z,a,c;
    void initGui();
};

#endif // CARBONWICKLER_MAIN_H
