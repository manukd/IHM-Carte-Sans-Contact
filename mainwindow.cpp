#pragma comment(lib, "ODALID.lib")
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include "ODALID.h"
#include <QtGui>

#define MI_OK                           0

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_4_clicked()
{
    QMessageBox msg;
    msg.setText("Un trajet a été ajouter");
    msg.exec();
}

void MainWindow::on_pushButton_3_clicked()
{
    QMessageBox msg;
    msg.setText("Un trajet a été retirer");
    msg.exec();
}

ReaderName MonLecteur;
char pszHost[] = "192.168.1.4";

void MainWindow::on_connecter_clicked()
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showStatus()));
    timer->start(1000);
    ui->connecter->setEnabled(false);
    ui->deconnecter->setEnabled(true);
    uint16_t status = 0;
    char version[30];
    uint8_t serial[4];
    char stackReader[20];
    char s_buffer[64];
    unsigned char key_A[6] = { 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5 };
    unsigned char key_B[6] = { 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5 };
    unsigned char key_C[6] = { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5 };
    unsigned char key_D[6] = { 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5 };
    unsigned char key_ff[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };


    MonLecteur.Type = ReaderCDC;
    MonLecteur.device = 0;
    status = OpenCOM1(&MonLecteur);

    int key_index = 0;
    // Charge les clés
    status = Mf_Classic_LoadKey(&MonLecteur, true, key_ff, 0);
    if (status != MI_OK){
        printf("Load Key [FAILED]\n");
    }
    status = Mf_Classic_LoadKey(&MonLecteur, false, key_ff, 0);
    if (status != MI_OK){
        printf("Load Key [FAILED]\n");
    }
    status = Mf_Classic_LoadKey(&MonLecteur, true, key_A, 2);
    if (status != MI_OK){
        printf("Load Key [FAILED]\n");
    }
    status = Mf_Classic_LoadKey(&MonLecteur, false, key_B, 2);
    if (status != MI_OK){
        printf("Load Key [FAILED]\n");
    }
    status = Mf_Classic_LoadKey(&MonLecteur, true, key_C, 3);
    if (status != MI_OK){
        printf("Load Key [FAILED]\n");
    }
    status = Mf_Classic_LoadKey(&MonLecteur, false, key_D, 3);
    if (status != MI_OK){
        printf("Load Key [FAILED]\n");
    }
    BYTE atq[2];
    BYTE sak[1];
    BYTE uid[12];
    uint16_t uid_len = 12;
    RF_Power_Control(&MonLecteur, TRUE, 0); // Recherche de communication
    int status2 = ISO14443_3_A_PollCard(&MonLecteur, atq, sak, uid, &uid_len); // Premier contact
    qDebug() << status2 ;
    printf("Tag found: UID=");
    for (int i = 0; i < uid_len; i++)
        printf("%02X", uid[i]);
    printf(" ATQ=%02X%02X SAK=%02X\n", atq[1], atq[0], sak[0]);


    if ((atq[1] != 0x00) || ((atq[0] != 0x02) && (atq[0] != 0x04) && (atq[0] != 0x18))){
        printf("This is not a Mifare classic tag\n");
    }

    uint8_t data[16];
    int status1 = Mf_Classic_Read_Block(&MonLecteur, TRUE, 9, data, Auth_KeyA, 2);
    QString nom = QString::fromStdString(std::string((char*)data)); // Lecture du nom
    int status3 = Mf_Classic_Read_Block(&MonLecteur, TRUE, 10, data, Auth_KeyA, 2);
    QString prenom = QString::fromStdString(std::string((char*)data)); // Lecture du prénom
    ui->lineName->setText(nom); // Affiche le nom
    ui->linePrenom->setText(prenom); // Affiche le prenom
    qDebug() << status1;
    //Lire Compteur
    uint32_t data1[64];
    int statusC = Mf_Classic_Read_Value(&MonLecteur, TRUE, 14, data1, Auth_KeyA, 3); // Lecture compteur
    QString compteur = QString::number(*data1);
    ui->lineCompteur->setText(compteur); // Affiche compteur
    qDebug() << statusC;

    // CloseCOM1(&MonLecteur);
}

void MainWindow::on_pushModif_clicked()
{
    // Modification des infos
    char dataIn[16];
    sprintf(dataIn, ui->lineName->text().toUtf8().data(), 16);
    uint8_t newName[16];
    for (int i = 0 ; i < 15 ; i++) {
        newName[i] = (uint8_t) dataIn[i];
    }
    Mf_Classic_Write_Block(&MonLecteur, TRUE, 9, newName, Auth_KeyB, 2); // Modification nom

    sprintf(dataIn, ui->linePrenom->text().toUtf8().data(), 16);
    uint8_t newPrenom[16];
    for (int i = 0 ; i < 15 ; i++) {
        newPrenom[i] = (uint8_t) dataIn[i];
    }
    Mf_Classic_Write_Block(&MonLecteur, TRUE, 10, newPrenom, Auth_KeyB, 2); // Modification prenom
    this->lire();
}

void MainWindow::on_pushButton_increment_clicked()
{
    // incrementer compteur
    int status = Mf_Classic_Increment_Value(&MonLecteur, TRUE, 13, 1, 14, Auth_KeyB, 3);
    if(status == 0) {
        Mf_Classic_Restore_Value(&MonLecteur, TRUE, 14, 13, Auth_KeyB, 3);
    }
    qDebug() << status;
    this->lire();
}

void MainWindow::on_pushButton_decrement_clicked()
{
    // decrementer compteur
    int status = Mf_Classic_Decrement_Value(&MonLecteur, TRUE, 13, 1, 14, Auth_KeyB, 3);
    if(status == 0) {
        Mf_Classic_Restore_Value(&MonLecteur, TRUE, 14, 13, Auth_KeyB, 3);
    }
    qDebug() << status;
    this->lire();
}

void MainWindow::lire()
{
    // Debut de la lecture manuelle
    uint8_t data[16];
    int status1 = Mf_Classic_Read_Block(&MonLecteur, TRUE, 9, data, Auth_KeyA, 2);
    QString nom = QString::fromStdString(std::string((char*)data));
    int status3 = Mf_Classic_Read_Block(&MonLecteur, TRUE, 10, data, Auth_KeyA, 2);
    QString prenom = QString::fromStdString(std::string((char*)data));
    ui->lineName->setText(nom);
    ui->linePrenom->setText(prenom);
    qDebug() << status1;
    //Lire Compteur
    uint32_t data1[64];
    int statusC = Mf_Classic_Read_Value(&MonLecteur, TRUE, 14, data1, Auth_KeyA, 3);
    QString compteur = QString::number(*data1);
    ui->lineCompteur->setText(compteur);
    qDebug() << statusC;
}


void MainWindow::on_deconnecter_clicked()
{
    // Deconnexion boutton
    CloseCOM1(&MonLecteur);
    ui->connecter->setEnabled(true);
    ui->deconnecter->setEnabled(false);
    ui->lineName->setText("");
    ui->linePrenom->setText("");
    ui->lineCompteur->setText("0");
    timer->stop();
}

void MainWindow::deco()
{
    // Deconnexion non prevue
    CloseCOM1(&MonLecteur);
    ui->lineName->setText("");
    ui->linePrenom->setText("");
    ui->lineCompteur->setText("0");
}

void MainWindow::on_pushButton_formater_clicked()
{
    // Remise a zero de la carte
    char dataIn[16];
    const QString a = "";
    sprintf(dataIn, a.toUtf8().data(), 16);
    uint8_t newName[16];
    for (int i = 0 ; i < 15 ; i++) {
        newName[i] = (uint8_t) dataIn[i];
    }
    Mf_Classic_Write_Block(&MonLecteur, TRUE, 9, newName, Auth_KeyB, 2); // RAZ nom
    const QString b = "";
    sprintf(dataIn, a.toUtf8().data(), 16);
    uint8_t newPrenom[16];
    for (int i = 0 ; i < 15 ; i++) {
        newPrenom[i] = (uint8_t) dataIn[i];
    }
    Mf_Classic_Write_Block(&MonLecteur, TRUE, 10, newPrenom, Auth_KeyB, 2); // RAZ prenom

    uint32_t data1[64];
    int status = Mf_Classic_Write_Value(&MonLecteur, TRUE, 14, 0, Auth_KeyB, 3); // RAZ compteur
    status = Mf_Classic_Write_Value(&MonLecteur, TRUE, 13, 0, Auth_KeyB, 3); // RAZ backup compteur
    qDebug() << status;
    this->lire();
}

void MainWindow::showStatus()
{
    // Lecture en continue avec QTimer
    uint8_t data[16];
    uint32_t data1[64];
    int status1 = Mf_Classic_Read_Block(&MonLecteur, TRUE, 9, data, Auth_KeyA, 2);
    int status2 = Mf_Classic_Read_Block(&MonLecteur, TRUE, 10, data, Auth_KeyA, 2);
    int status3 = Mf_Classic_Read_Value(&MonLecteur, TRUE, 14, data1, Auth_KeyA, 3);
    // Si on ne peux pas lire les trois donnees precedentes alors on remet a zero et on essai de charger les cles en continues
    if(status1 != 0 || status2 != 0 || status3 != 0)
    {
        ui->lineName->setText("");
        ui->linePrenom->setText("");
        ui->lineCompteur->setText("0");
        this->deco();

        uint16_t status = 0;
        char version[30];
        uint8_t serial[4];
        char stackReader[20];
        char s_buffer[64];
        unsigned char key_A[6] = { 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5 };
        unsigned char key_B[6] = { 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5 };
        unsigned char key_C[6] = { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5 };
        unsigned char key_D[6] = { 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5 };
        unsigned char key_ff[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };


        MonLecteur.Type = ReaderCDC;
        MonLecteur.device = 0;
        status = OpenCOM1(&MonLecteur);

        int key_index = 0;
        status = Mf_Classic_LoadKey(&MonLecteur, true, key_ff, 0);
        if (status != MI_OK){
            printf("Load Key [FAILED]\n");
        }
        status = Mf_Classic_LoadKey(&MonLecteur, false, key_ff, 0);
        if (status != MI_OK){
            printf("Load Key [FAILED]\n");
        }
        status = Mf_Classic_LoadKey(&MonLecteur, true, key_A, 2);
        if (status != MI_OK){
            printf("Load Key [FAILED]\n");
        }
        status = Mf_Classic_LoadKey(&MonLecteur, false, key_B, 2);
        if (status != MI_OK){
            printf("Load Key [FAILED]\n");
        }
        status = Mf_Classic_LoadKey(&MonLecteur, true, key_C, 3);
        if (status != MI_OK){
            printf("Load Key [FAILED]\n");
        }
        status = Mf_Classic_LoadKey(&MonLecteur, false, key_D, 3);
        if (status != MI_OK){
            printf("Load Key [FAILED]\n");
        }
        // Si aucun probleme alors on lit les donnees
        if(status == 0) {
                BYTE atq[2];
                BYTE sak[1];
                BYTE uid[12];
                uint16_t uid_len = 12;
                RF_Power_Control(&MonLecteur, TRUE, 0);
                int status2 = ISO14443_3_A_PollCard(&MonLecteur, atq, sak, uid, &uid_len);
                qDebug() << status2 ;
                printf("Tag found: UID=");
                for (int i = 0; i < uid_len; i++)
                    printf("%02X", uid[i]);
                printf(" ATQ=%02X%02X SAK=%02X\n", atq[1], atq[0], sak[0]);


                if ((atq[1] != 0x00) || ((atq[0] != 0x02) && (atq[0] != 0x04) && (atq[0] != 0x18))){
                    printf("This is not a Mifare classic tag\n");
                }

                uint8_t data[16];
                int status1 = Mf_Classic_Read_Block(&MonLecteur, TRUE, 9, data, Auth_KeyA, 2);
                QString nom = QString::fromStdString(std::string((char*)data));
                int status3 = Mf_Classic_Read_Block(&MonLecteur, TRUE, 10, data, Auth_KeyA, 2);
                QString prenom = QString::fromStdString(std::string((char*)data));
                ui->lineName->setText(nom);
                ui->linePrenom->setText(prenom);
                qDebug() << status1;
                //Lire Compteur
                uint32_t data1[64];
                int statusC = Mf_Classic_Read_Value(&MonLecteur, TRUE, 14, data1, Auth_KeyA, 3);
                QString compteur = QString::number(*data1);
                ui->lineCompteur->setText(compteur);
                qDebug() << statusC;
        }
    }
}

