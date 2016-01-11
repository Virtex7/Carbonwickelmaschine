#include "carbonwickler_main.h"
#include "ui_carbonwickler_main.h"
#include "stdio.h"
#include <QFileDialog>
#include <QString>
#include <QFile>
#include <QMessageBox>
#include <cmath>
#include <QRegExp>
#include <QLineEdit>

QString header = "";

carbonwickler_main::carbonwickler_main(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::carbonwickler_main) {
    ui->setupUi(this);

    // texteintragungen:
    ui->filePath->setText("-Datei auswählen-");
    initGui();
}


carbonwickler_main::~carbonwickler_main() {
    delete ui;
}
void carbonwickler_main::initGui(){

    connect(ui->l_gesamt,SIGNAL(textEdited(QString)),this,SLOT(inputValidatorLineEdit(QString)));
    connect(ui->d_end,SIGNAL(textEdited(QString)),this,SLOT(inputValidatorLineEdit(QString)));
    connect(ui->l_start,SIGNAL(textEdited(QString)),this,SLOT(inputValidatorLineEdit(QString)));
    connect(ui->l_end,SIGNAL(textEdited(QString)),this,SLOT(inputValidatorLineEdit(QString)));
    connect(ui->rovingbreite,SIGNAL(textEdited(QString)),this,SLOT(inputValidatorLineEdit(QString)));
    connect(ui->winkel,SIGNAL(sliderMoved(int)),this,SLOT(winkelDataSync(int)));
}

void carbonwickler_main::on_ButtonQuit_clicked() {
    // Datei hier schließen. Programm is fertig.
    outputfile->close ();
    qApp->quit ();
}

int carbonwickler_main::on_ButtonGenCode_clicked() {
    int ret = EXIT_SUCCESS;
    ret += writeHeader ();
    ret += generateGCode ();

    if (ret == EXIT_SUCCESS) {
        QMessageBox::information (this, "Fertig!", "G-Code erstellt und gespeichert!\nProgramm kann geschlossen werden.");
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

int carbonwickler_main::fileOpen() {
    QString fn = QFileDialog::getSaveFileName (this, tr("Datei öffnen"), QString(), tr("gcode (*.nc *.txt *.ngc)"));
    outputfile = new QFile(fn);
    outputfile->open (QIODevice::WriteOnly);

    if (!(outputfile->isWritable ())) {
        QMessageBox::warning (this, "OBACHT", "Datei nicht schreibbar!");
        return EXIT_FAILURE;
    }

    outputstream = new QTextStream(outputfile);
    *outputstream << QString("; Erste Zeile der Ausgabedatei\n");
    ui->ButtonGenCode->setEnabled (TRUE);

    return EXIT_SUCCESS;
}

void carbonwickler_main::on_ButtonOpenFile_clicked() {
    if(!fileOpen ()) {
        ui->filePath->setText (outputfile->fileName ());
    }
}

void carbonwickler_main::inputValidatorLineEdit(QString new_input)
{
    QLineEdit *lineEditObj = qobject_cast<QLineEdit*>(QObject::sender ());
    QRegExp numbers1to9("[0-9]{1,9}",Qt::CaseInsensitive);
    QPalette pal_norm;
    QPalette pal_err;
    pal_norm.setColor(QPalette::Base,Qt::white);
    pal_err.setColor(QPalette::Base,Qt::red);
        if(!numbers1to9.exactMatch (new_input)){
            lineEditObj->setPalette (pal_err);
            lineEditObj->backspace ();
        }else{
            lineEditObj->setPalette (pal_norm);
        }
}

void carbonwickler_main::winkelDataSync(int value)
{
    ui->winkel_grad->setText (QString("%1 Grad").arg(value));
}


int carbonwickler_main::writeHeader() {
    if (!(outputfile->isWritable ())) {
        QMessageBox::warning (this, "OBACHT", "Datei nicht schreibbar!");
        return EXIT_FAILURE;
    }
    QString header;
    header.append ("; Guten Tag!\n");
    header.append ("; Datei erstellt vom GCode-Generator für Rohre\n");
    header.append ("; \n");

    *outputstream << QString(header);
    return EXIT_SUCCESS;
}

int carbonwickler_main::generateGCode() {

    // KONVENTION:
    // Linearachsen in mm erst mal ohne Komma
    // Drehachsen in 1/10°

    double d_end = (ui->d_end->text ()).toDouble ();
    double l_gesamt = (ui->l_gesamt->text ()).toDouble ();
    double l_end = (ui->l_end->text ()).toDouble ();
    double l_start = (ui->l_start->text ()).toDouble ();
    double l_mitte = l_gesamt - l_start - l_end;
    double ganghoehe = tan ((ui->winkel->value ()))*2*M_PI;
    double lagen = (ui->lagen->value ());
    double rovingbreite = (ui->rovingbreite->text ().toDouble ());
    double feed_normal = 1000;
    double feed_slow = 400;
    double gaenge = l_gesamt / rovingbreite;

    // Maschinenachsen
    double x,z,a,c;
    // MODUS: relative koordinaten

    QString temp;
    // Algo für Rohre

    for (int i = 0; i<lagen; i++) { // Lagenzähler
        for (int j = 0; j<gaenge; j++) { // nebeneinander liegende, einzelne Schnüre
            // anfangsstück
            temp.append (QString("G1 F%1 Z%2 C%3\n").arg (feed_slow).arg (l_start).arg (360*3));
            z+=l_start;
            c+=360*3;

            // wickelgang bis kurz vors ende (winkelkontrolliert)
            temp.append (QString("G1 F%1 Z%2 C%3\n").arg (feed_normal).arg (l_mitte).arg (360*(l_mitte/ganghoehe)));
            z+=l_mitte;
            c+=360*(l_mitte/ganghoehe);

            // Endstück
            temp.append (QString("G1 F%1 Z%2 C%3\n").arg (feed_slow).arg (l_end).arg (360*3));

            // Versatz 180°
            temp.append (QString("G1 F%1 C%2\n").arg (feed_slow).arg (180));

            // Endstück zurückwickeln
            temp.append (QString("G1 F%1 Z%2 C%3\n").arg (feed_slow).arg (-l_end).arg (360*3));

            // Zurückwickeln bis kurz vor start
            temp.append (QString("G1 F%1 Z%2 C%3\n").arg (feed_normal).arg (-l_mitte).arg (360*(l_mitte/ganghoehe)));
            z+=l_mitte;
            c+=360*(l_mitte/ganghoehe);

            // Startstück zurückwickeln
            temp.append (QString("G1 F%1 Z%2 C%3\n").arg (feed_slow).arg (-l_start).arg (360*3));
            z+=-l_start;
            c+=360*3;

            // Verschiebung des Startpunktes um eine Lagenbreite
            temp.append (QString("G1 F%1 C%2\n").arg (feed_slow).arg (5)); // eine rovingbreite weiter! TODO noch ausrechnen!!
            c+=5;
        }
    }
    // Endstück, einfach eine Windung machen
    temp.append (QString("G1 F%1 A3600\n").arg (feed_slow));
    temp.append (QString("M30\n"));

    temp.append ("\n");
    *outputstream << temp;
    outputstream->flush ();
    return EXIT_SUCCESS;
}

