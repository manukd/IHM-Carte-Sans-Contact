#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_4_clicked();

    void on_pushButton_3_clicked();

    void on_connecter_clicked();

    void on_pushModif_clicked();

    void on_pushButton_increment_clicked();

    void on_pushButton_decrement_clicked();

    void lire();

    void on_deconnecter_clicked();

    void on_pushButton_formater_clicked();

    void showStatus();

    void deco();
private:
    Ui::MainWindow *ui;
    QTimer *timer;
};

#endif // MAINWINDOW_H
