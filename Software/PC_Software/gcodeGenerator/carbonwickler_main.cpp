#include "carbonwickler_main.h"
#include "ui_carbonwickler_main.h"
#include "stdio.h"
#include "qfiledialog.h"
#include "qstring.h"

carbonwickler_main::carbonwickler_main(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::carbonwickler_main) {
    ui->setupUi(this);

    // texteintragungen:
    ui->filePath->setText("/tmp/rohr.nc");
}


carbonwickler_main::~carbonwickler_main() {
    delete ui;
}


void carbonwickler_main::on_ButtonQuit_clicked()
{
    printf("knopf beenden gedrückt!\n");
    qApp->quit ();

}

void carbonwickler_main::on_ButtonGenCode_clicked()
{

}

QString carbonwickler_main::fileOpen() {
    QString fn = "/tmp/carbonrohr.nc";
    fn = QFileDialog::getOpenFileName (this, tr("Datei öffnen"), QString(), tr("gcode (*.nc, *.txt, *.ngc)"));
    if (!fn.isEmpty ()) {
        // Datei zum schreiben öffnen
    }
    return fn;
}

void carbonwickler_main::on_ButtonOpenFile_clicked() {
    ui->filePath->setText (this->fileOpen ());
}
