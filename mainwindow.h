#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtSql>
#include <QMainWindow>
#include <QProgressBar>
#include <QLabel>
#include <mainwindowgrafik.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
//    QProgressBar* bar;



    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void filtr_test1();
    void filtr_vyp_vuz();

//функции фильтрации
    void analiz1_otr();

    void analiz3_otr();


//Функции добавленияэлементов в фильтры
    void add_elements_to_otrfiltr();

    void add_elements_to_regfiltr();

    void add_elements_to_oblfiltr();

    void add_elements_to_citfiltr();

    void Otchet();

    int ExecuteSqlScriptFile(QSqlDatabase & db, const QString & fileName);


private slots:
   void slotShowAction(const QString& str);

   void on_comboBoxotr_currentIndexChanged(const QString &arg1);

   void on_comboBoxreg_currentIndexChanged(const QString &arg1);

   void on_comboBoxobl_currentIndexChanged(const QString &arg1);

   void on_comboBoxcity_currentIndexChanged(const QString &arg1);

   void on_checkBoxotr_clicked(bool checked);





   void on_use_filtrs_button_clicked();

   void on_actionVypVuz_triggered();

   void on_comboBox_year_currentIndexChanged(int index);

   void on_comboBox_vidobuch_currentIndexChanged(int index);

   void on_actionVypOtr_triggered();

   void on_report_Button_clicked();

   void on_actionEfotr_triggered();

   void on_actionEfvuz_triggered();

   void on_actionTable9_triggered();

   void on_actionTable10_2_triggered();

   void on_actionOrgtable_triggered();

   void on_actionSpectable_triggered();

   void on_action_triggered();

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
