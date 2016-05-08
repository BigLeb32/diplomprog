#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mainwindowgrafik.h"
#include <QtSql>
#include <QProgressBar>
#include <lrreportengine.h>
#include <lrcallbackdatasourceintf.h>


#define ANALIZ_1 1
#define ANALIZ_2 2
#define ANALIZ_3 3
#define ANALIZ_4 4


QSqlDatabase db;
QString curtable, curvyp_vuz_otr;
int curanaliz;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)

{


//----------------Соединение с БД----------------------------//
    QSqlDatabase db = QSqlDatabase::addDatabase("QOCI");
        db.setUserName("cap1");
        db.setHostName("localhost");
        db.setPassword("cap1");
        if (!db.open()) {
        qDebug() << "Cannot open database:" << db.lastError();
        QMessageBox::warning(this, "Ошибка!", "Не удалось подключится к БД!");
        exit(0);
    }else{qDebug()<<"ok\n";}
    ui->setupUi(this);
//----------------------------------------------------------//

//----------------Открытие таблиц(TABLES)----------------------------//
    QSignalMapper* psigMapper = new QSignalMapper(this);
    connect(psigMapper, SIGNAL(mapped(const QString&)),
            this, SLOT(slotShowAction(const QString&)));

    connect(ui->actionTable9, SIGNAL(triggered()),
            psigMapper, SLOT(map()));
    psigMapper->setMapping(ui->actionTable9,"table9");

    connect(ui->actionTable10_2, SIGNAL(triggered()),
            psigMapper, SLOT(map()));
    psigMapper->setMapping(ui->actionTable10_2,"table10");

    connect(ui->actionOrgtable, SIGNAL(triggered()),
            psigMapper, SLOT(map()));
    psigMapper->setMapping(ui->actionOrgtable,"orgtable");

    connect(ui->actionSpectable, SIGNAL(triggered()),
            psigMapper, SLOT(map()));
    psigMapper->setMapping(ui->actionSpectable,"spectable");
//----------------------------------------------------------//

//--------------------Открытие таблиц(Анализ)--------------//
    connect(ui->actionEfvuz, SIGNAL(triggered()),
            psigMapper, SLOT(map()));
    psigMapper->setMapping(ui->actionEfvuz,"filtr_test1");

    connect(ui->actionEfotr, SIGNAL(triggered()),
            psigMapper, SLOT(map()));
    psigMapper->setMapping(ui->actionEfotr,"testotr_2011");

    connect(ui->actionVypVuz, SIGNAL(triggered()),
            psigMapper, SLOT(map()));
    psigMapper->setMapping(ui->actionVypVuz,"filtr_vyp_vuz");

//----------------------------------------------------------//


//------------------Заполнение комбобоксов по годам и по виду обучения------------//
    QStringList list1, list2;
    list1 << "Бюджетное обучение" << "Платное обучение";
    list2 << "2011" << "2012" << "2013" << "2014" <<"2015";
    ui->comboBox_vidobuch->addItems(list1);
    ui->comboBox_year->addItems(list2);


//--------------------Добавление элементов в фильтр--------------//
    add_elements_to_regfiltr();
    add_elements_to_otrfiltr();
//-------------------------------------------------------------//


//----скрываем фильтры
    ui->framefiltrs->setHidden(true);
    ui->checkBoxfiltrs->hide();


//----Очистка таблицы---
    QSqlTableModel* model = new QSqlTableModel( this, db );
    ui->tableView->setModel( model );
    model->clear();
    model->select();
    model->sort(0,Qt::AscendingOrder);

    ui->lblnmoftable->clear();

    ui->use_filtrs_button->setEnabled(false);


//для установки в 1 и 3 анализе значений по умолчанию
    filtr_test1();
    filtr_vyp_vuz();

}

//-------Деструктор-------//
MainWindow::~MainWindow()
{
    delete ui;
}

//--------Открывает таблицы--------//
void MainWindow::slotShowAction(const QString& str)
{
    //Установка модели таблицы
    QSqlTableModel* model = new QSqlTableModel( this, db );

    ui->tableView->setModel( model );
    model->setTable( str );
    model->select();

    curtable = str;

    QString curyear = ui->comboBox_year->currentText();
    QString curvidobuch = ui->comboBox_vidobuch->currentText();

//---Таблица эффективности по ВУЗам-----
    if(str=="test1"){
        ui->lblnmoftable->setText("Эффективность выпуска по ВУЗам за 1 год");
        ui->tableView->setColumnHidden(6,true);
        ui->tableView->setColumnHidden(7,true);
        ui->tableView->setColumnHidden(8,true);
        ui->tableView->setColumnHidden(9,true);

        model->setHeaderData( 0, Qt::Horizontal, QObject::tr("Название организации") );
        model->setHeaderData( 1, Qt::Horizontal, QObject::tr("Факт. выпуск \n в отчетном году: \n всего") );
        model->setHeaderData( 2, Qt::Horizontal, QObject::tr("Факт. выпуск \n в отчетном году: \n с защитой диссертации \n в срок") );
        model->setHeaderData( 3, Qt::Horizontal, QObject::tr("Защищено диссертаций: \n в отчетном году \n после окончания") );
        model->setHeaderData( 4, Qt::Horizontal, QObject::tr("Эффективность \n выпуска") );
        model->setHeaderData( 5, Qt::Horizontal, QObject::tr("Эффективность \n выпуска \n с учетом дозащиты") );
        ui->tableView->resizeColumnsToContents();
        ui->tableView->setColumnWidth(0,520);
    }
//-----Отфильтрованнная таблица эффективности по ВУЗам-----
    else if(str=="filtr_test1"){
        ui->lblnmoftable->setText("Эффективность выпуска по ВУЗам за 1 год");
        ui->checkBoxfiltrs->setHidden(false);
        ui->tableView->setColumnHidden(6,true);
        ui->tableView->setColumnHidden(7,true);
        ui->tableView->setColumnHidden(8,true);
        ui->tableView->setColumnHidden(9,true);

        model->setHeaderData( 0, Qt::Horizontal, QObject::tr("Название организации") );
        model->setHeaderData( 1, Qt::Horizontal, QObject::tr("Факт. выпуск \n в отчетном году: \n всего") );
        model->setHeaderData( 2, Qt::Horizontal, QObject::tr("Факт. выпуск \n в отчетном году: \n с защитой диссертации \n в срок") );
        model->setHeaderData( 3, Qt::Horizontal, QObject::tr("Защищено диссертаций: \n в отчетном году \n после окончания") );
        model->setHeaderData( 4, Qt::Horizontal, QObject::tr("Эффективность \n выпуска") );
        model->setHeaderData( 5, Qt::Horizontal, QObject::tr("Эффективность \n выпуска \n с учетом дозащиты") );
        ui->tableView->resizeColumnsToContents();
        ui->tableView->setColumnWidth(0,520);
    }

//-------Исходные данные: Таблица 9 или 10---------
    if(str=="table9" || str=="table10"){
        if(str=="table9")
            ui->lblnmoftable->setText("Исходные данные: Выпуск аспирантов(бюджет)");
        else if(str=="table10")
            ui->lblnmoftable->setText("Исходные данные: Выпуск аспирантов(платное обучение)");
        model->setHeaderData( 0, Qt::Horizontal, QObject::tr("Отчетный \n год") );
        model->setHeaderData( 1, Qt::Horizontal, QObject::tr("Код \n организации") );
        model->setHeaderData( 2, Qt::Horizontal, QObject::tr("Шифр \n специальности") );
        model->setHeaderData( 3, Qt::Horizontal, QObject::tr("Факт. выпуск \n в отчетном году: \n всего") );
        model->setHeaderData( 4, Qt::Horizontal, QObject::tr("Факт. выпуск \n в отчетном году: \n с защитой диссертации: \n всего") );
        model->setHeaderData( 5, Qt::Horizontal, QObject::tr("Факт. выпуск \n в отчетном году: \n с защитой диссертации: \n очно") );
        model->setHeaderData( 6, Qt::Horizontal, QObject::tr("Факт. выпуск \n в отчетном году: \n с защитой диссертации: \n женщины") );
        model->setHeaderData( 7, Qt::Horizontal, QObject::tr("Факт. выпуск \n в отчетном году: \n с представлением диссертации") );
        model->setHeaderData( 8, Qt::Horizontal, QObject::tr("Защищено диссертаций: \n в отчетном году") );
        model->setHeaderData( 9, Qt::Horizontal, QObject::tr("Защищено диссертаций: \n в предшествующий") );
        model->setHeaderData( 10, Qt::Horizontal, QObject::tr("Защищено диссертаций: \n за два года \n до отчетного") );
        model->setHeaderData( 11, Qt::Horizontal, QObject::tr("Защищено диссертаций: \n за три года \n до отчетного") );
        model->setHeaderData( 12, Qt::Horizontal, QObject::tr("Защищено диссертаций: \n всего") );
        model->setHeaderData( 13, Qt::Horizontal, QObject::tr("Защищено диссертаций: \n женщинами") );
        ui->tableView->resizeColumnsToContents();
    }

//-------Исходные данные: Таблица Организаций---------
    else if(str=="orgtable"){
        ui->lblnmoftable->setText("Исходные данные: Таблица организаций");
        model->setHeaderData( 0, Qt::Horizontal, QObject::tr("Код \n организации") );
        model->setHeaderData( 1, Qt::Horizontal, QObject::tr("Название \n организации") );
        model->setHeaderData( 2, Qt::Horizontal, QObject::tr("Полное название \n организации") );
        model->setHeaderData( 3, Qt::Horizontal, QObject::tr("Сокращенное название \n организации") );
        model->setHeaderData( 4, Qt::Horizontal, QObject::tr("Адрес \n организации") );
        model->setHeaderData( 5, Qt::Horizontal, QObject::tr("Телефон \n ректора") );
        model->setHeaderData( 6, Qt::Horizontal, QObject::tr("Факс") );
        model->setHeaderData( 7, Qt::Horizontal, QObject::tr("E-mail") );
        model->setHeaderData( 8, Qt::Horizontal, QObject::tr("Ректор") );
        model->setHeaderData( 9, Qt::Horizontal, QObject::tr("Федеральный \n округ") );
        model->setHeaderData( 10, Qt::Horizontal, QObject::tr("Город") );
        model->setHeaderData( 11, Qt::Horizontal, QObject::tr("Статус") );
        model->setHeaderData( 12, Qt::Horizontal, QObject::tr("Субъект РФ") );
        model->setHeaderData( 13, Qt::Horizontal, QObject::tr("Группа \n ведущих \n вузов") );
        ui->tableView->resizeColumnsToContents();
        ui->tableView->setColumnWidth(1,550);
        ui->tableView->setColumnWidth(2,550);

    }


//-------Исходные данные: Таблица специальностей---------
    else if(str=="spectable"){
        ui->lblnmoftable->setText("Исходные данные: Таблица специальностей");
        model->setHeaderData( 0, Qt::Horizontal, QObject::tr("Шифр \n специальности") );
        model->setHeaderData( 1, Qt::Horizontal, QObject::tr("Наименование \n специальности") );
        ui->tableView->resizeColumnsToContents();
    }
//-------Таблица Эффективность по отраслям---------
    else if(str=="testotr" || str=="testotr_2011" || str=="testotr_2012"
            || str=="testotr_2013" || str=="testotr_2014"
            || str=="testotr_2015" || str=="testotr_2011_platn"
            || str=="testotr_2012_platn" ||  str=="testotr_2013_platn"
            ||  str=="testotr_2014_platn" ||  str=="testotr_2015_platn"){

        ui->lblnmoftable->setText("Эффективность выпуска по отраслям за 1 год");
        model->setHeaderData( 0, Qt::Horizontal, QObject::tr("Шифр \n специальности") );
        model->setHeaderData( 1, Qt::Horizontal, QObject::tr("Наименование \n специальности") );
        model->setHeaderData( 2, Qt::Horizontal, QObject::tr("Факт. выпуск \n в отчетном году: \n всего") );
        model->setHeaderData( 3, Qt::Horizontal, QObject::tr("Факт. выпуск \n в отчетном году: \n с защитой диссертации") );
        model->setHeaderData( 4, Qt::Horizontal, QObject::tr("Защищено диссертаций: \n в отчетном году") );
        model->setHeaderData( 5, Qt::Horizontal, QObject::tr("Эффективность \n выпуска") );
        model->setHeaderData( 6, Qt::Horizontal, QObject::tr("Эффективность \n выпуска \n с учетом дозащиты") );
        ui->tableView->resizeColumnsToContents();
    }
    else if( str=="vyp_otr_2011" || str=="filtr_vyp_vuz" && curvidobuch == "Бюджетное обучение" && curyear == "2011"){
        if(str == "filtr_vyp_vuz"){
            ui->lblnmoftable->setText("Эффективность выпуска с учетом дозащиты по ВУЗам. Бюджетное обучение. 2011 год");
            model->setHeaderData( 0, Qt::Horizontal, QObject::tr("Название \n организации") );
            ui->tableView->setColumnWidth(0,580);
        }else if(str == "vyp_otr_2011"){
            ui->lblnmoftable->setText("Эффективность выпуска с учетом дозащиты по отраслям. Бюджетное обучение. 2011 год");
            model->setHeaderData( 0, Qt::Horizontal, QObject::tr("Название \n отрасли науки") );
            ui->tableView->setColumnWidth(0,300);
        }
        model->setHeaderData( 1, Qt::Horizontal, QObject::tr("Выпущено в 2011\n всего") );
        model->setHeaderData( 2, Qt::Horizontal, QObject::tr("Выпущено в 2011\n с защитой в срок") );
        model->setHeaderData( 3, Qt::Horizontal, QObject::tr("Выпущено в 2011\n с защитой в срок %") );
        model->setHeaderData( 4, Qt::Horizontal, QObject::tr("Защиты в 2011-2015") );
        model->setHeaderData( 5, Qt::Horizontal, QObject::tr("Всего защитилось %") );
        ui->tableView->hideColumn(6);
        ui->tableView->hideColumn(7);
        ui->tableView->hideColumn(8);
        ui->tableView->hideColumn(9);
        ui->tableView->setColumnWidth(1,145);
        ui->tableView->setColumnWidth(2,145);
        ui->tableView->setColumnWidth(3,145);
        ui->tableView->setColumnWidth(4,145);
        ui->tableView->setColumnWidth(5,145);
    }
    else if(str=="vyp_otr_2012" || str=="filtr_vyp_vuz" && curvidobuch == "Бюджетное обучение" && curyear == "2012"){
        if(str == "filtr_vyp_vuz"){
            ui->lblnmoftable->setText("Эффективность выпуска с учетом дозащиты по ВУЗам. Бюджетное обучение. 2012 год");
            model->setHeaderData( 0, Qt::Horizontal, QObject::tr("Название \n организации") );
            ui->tableView->setColumnWidth(0,580);
        }else if(str == "vyp_otr_2012"){
            ui->lblnmoftable->setText("Эффективность выпуска с учетом дозащиты по отраслям. Бюджетное обучение. 2012 год");
            model->setHeaderData( 0, Qt::Horizontal, QObject::tr("Название \n отрасли науки") );
            ui->tableView->setColumnWidth(0,300);

        }
        model->setHeaderData( 1, Qt::Horizontal, QObject::tr("Выпущено в 2012\n всего") );
        model->setHeaderData( 2, Qt::Horizontal, QObject::tr("Выпущено в 2012\n с защитой в срок") );
        model->setHeaderData( 3, Qt::Horizontal, QObject::tr("Выпущено в 2012\n с защитой в срок %") );
        model->setHeaderData( 4, Qt::Horizontal, QObject::tr("Защиты в 2012-2015") );
        model->setHeaderData( 5, Qt::Horizontal, QObject::tr("Всего защитилось %") );
        ui->tableView->hideColumn(6);
        ui->tableView->hideColumn(7);
        ui->tableView->hideColumn(8);
        ui->tableView->hideColumn(9);
        ui->tableView->setColumnWidth(1,145);
        ui->tableView->setColumnWidth(2,145);
        ui->tableView->setColumnWidth(3,145);
        ui->tableView->setColumnWidth(4,145);
        ui->tableView->setColumnWidth(5,145);

    }
    else if(str=="vyp_otr_2013" || str=="filtr_vyp_vuz"  && curvidobuch == "Бюджетное обучение" && curyear == "2013"){
        if(str == "filtr_vyp_vuz"){
            ui->lblnmoftable->setText("Эффективность выпуска с учетом дозащиты по ВУЗам. Бюджетное обучение. 2013 год");
            model->setHeaderData( 0, Qt::Horizontal, QObject::tr("Название \n организации") );
            ui->tableView->setColumnWidth(0,580);

        }else if(str == "vyp_otr_2013"){
            ui->lblnmoftable->setText("Эффективность выпуска с учетом дозащиты по отраслям. Бюджетное обучение. 2013 год");
            model->setHeaderData( 0, Qt::Horizontal, QObject::tr("Название \n отрасли науки") );
            ui->tableView->setColumnWidth(0,300);

        }
        model->setHeaderData( 1, Qt::Horizontal, QObject::tr("Выпущено в 2013\n всего") );
        model->setHeaderData( 2, Qt::Horizontal, QObject::tr("Выпущено в 2013\n с защитой в срок") );
        model->setHeaderData( 3, Qt::Horizontal, QObject::tr("Выпущено в 2013\n с защитой в срок %") );
        model->setHeaderData( 4, Qt::Horizontal, QObject::tr("Защиты в 2013-2015") );
        model->setHeaderData( 5, Qt::Horizontal, QObject::tr("Всего защитилось %") );
        ui->tableView->hideColumn(6);
        ui->tableView->hideColumn(7);
        ui->tableView->hideColumn(8);
        ui->tableView->hideColumn(9);
        ui->tableView->setColumnWidth(1,145);
        ui->tableView->setColumnWidth(2,145);
        ui->tableView->setColumnWidth(3,145);
        ui->tableView->setColumnWidth(4,145);
        ui->tableView->setColumnWidth(5,145);

    }
    else if(str=="vyp_otr_2014" || str=="filtr_vyp_vuz" && curvidobuch == "Бюджетное обучение" && curyear == "2014"){
        if(str == "filtr_vyp_vuz"){
            ui->lblnmoftable->setText("Эффективность выпуска с учетом дозащиты по ВУЗам. Бюджетное обучение. 2014 год");
            model->setHeaderData( 0, Qt::Horizontal, QObject::tr("Название \n организации") );
            ui->tableView->setColumnWidth(0,580);

        }else if(str == "vyp_otr_2014"){
            ui->lblnmoftable->setText("Эффективность выпуска с учетом дозащиты по отраслям. Бюджетное обучение. 2014 год");
            model->setHeaderData( 0, Qt::Horizontal, QObject::tr("Название \n отрасли науки") );
            ui->tableView->setColumnWidth(0,300);

        }
        model->setHeaderData( 1, Qt::Horizontal, QObject::tr("Выпущено в 2014\n всего") );
        model->setHeaderData( 2, Qt::Horizontal, QObject::tr("Выпущено в 2014\n с защитой в срок") );
        model->setHeaderData( 3, Qt::Horizontal, QObject::tr("Выпущено в 2014\n с защитой в срок %") );
        model->setHeaderData( 4, Qt::Horizontal, QObject::tr("Защиты в 2014-2015") );
        model->setHeaderData( 5, Qt::Horizontal, QObject::tr("Всего защитилось %") );
        ui->tableView->hideColumn(6);
        ui->tableView->hideColumn(7);
        ui->tableView->hideColumn(8);
        ui->tableView->hideColumn(9);
        ui->tableView->setColumnWidth(1,145);
        ui->tableView->setColumnWidth(2,145);
        ui->tableView->setColumnWidth(3,145);
        ui->tableView->setColumnWidth(4,145);
        ui->tableView->setColumnWidth(5,145);

    }
    else if(str=="vyp_otr_2015" || str=="filtr_vyp_vuz"  && curvidobuch == "Бюджетное обучение" && curyear == "2015"){
        if(str == "filtr_vyp_vuz"){
            ui->lblnmoftable->setText("Эффективность выпуска с учетом дозащиты по ВУЗам. Бюджетное обучение. 2015 год");
            model->setHeaderData( 0, Qt::Horizontal, QObject::tr("Название \n организации") );
            ui->tableView->setColumnWidth(0,580);

        }else if(str == "vyp_otr_2015"){
            ui->lblnmoftable->setText("Эффективность выпуска с учетом дозащиты по отраслям. Бюджетное обучение. 2015 год");
            model->setHeaderData( 0, Qt::Horizontal, QObject::tr("Название \n отрасли науки") );
            ui->tableView->setColumnWidth(0,300);

        }
        model->setHeaderData( 1, Qt::Horizontal, QObject::tr("Выпущено в 2015\n всего") );
        model->setHeaderData( 2, Qt::Horizontal, QObject::tr("Выпущено в 2015\n с защитой в срок") );
        model->setHeaderData( 3, Qt::Horizontal, QObject::tr("Выпущено в 2015\n с защитой в срок %") );
        model->setHeaderData( 4, Qt::Horizontal, QObject::tr("Защиты в 2015") );
        model->setHeaderData( 5, Qt::Horizontal, QObject::tr("Всего защитилось %") );
        ui->tableView->hideColumn(6);
        ui->tableView->hideColumn(7);
        ui->tableView->hideColumn(8);
        ui->tableView->hideColumn(9);
        ui->tableView->setColumnWidth(1,145);
        ui->tableView->setColumnWidth(2,145);
        ui->tableView->setColumnWidth(3,145);
        ui->tableView->setColumnWidth(4,145);
        ui->tableView->setColumnWidth(5,145);
    }
    else if(str=="vyp_otr_platn_2011" || str=="filtr_vyp_vuz" && curvidobuch == "Платное обучение" && curyear == "2011"){
        if(str == "filtr_vyp_vuz"){
            ui->lblnmoftable->setText("Эффективность выпуска с учетом дозащиты по ВУЗам. Платное обучение. 2011 год");
            model->setHeaderData( 0, Qt::Horizontal, QObject::tr("Название \n организации") );
            ui->tableView->setColumnWidth(0,580);

        }else if(str == "vyp_otr_platn_2011"){
            ui->lblnmoftable->setText("Эффективность выпуска с учетом дозащиты по отраслям. Платное обучение. 2011 год");
            model->setHeaderData( 0, Qt::Horizontal, QObject::tr("Название \n отрасли науки") );
            ui->tableView->setColumnWidth(0,300);

        }
        model->setHeaderData( 1, Qt::Horizontal, QObject::tr("Выпущено в 2011\n всего") );
        model->setHeaderData( 2, Qt::Horizontal, QObject::tr("Выпущено в 2011\n с защитой в срок") );
        model->setHeaderData( 3, Qt::Horizontal, QObject::tr("Выпущено в 2011\n с защитой в срок %") );
        model->setHeaderData( 4, Qt::Horizontal, QObject::tr("Защиты в 2011-2015") );
        model->setHeaderData( 5, Qt::Horizontal, QObject::tr("Всего защитилось %") );
        ui->tableView->hideColumn(6);
        ui->tableView->hideColumn(7);
        ui->tableView->hideColumn(8);
        ui->tableView->hideColumn(9);
        ui->tableView->setColumnWidth(1,145);
        ui->tableView->setColumnWidth(2,145);
        ui->tableView->setColumnWidth(3,145);
        ui->tableView->setColumnWidth(4,145);
        ui->tableView->setColumnWidth(5,145);

    }
    else if(str=="vyp_otr_platn_2012" || str=="filtr_vyp_vuz" && curvidobuch == "Платное обучение" && curyear == "2012"){
        if(str == "filtr_vyp_vuz"){
            ui->lblnmoftable->setText("Эффективность выпуска с учетом дозащиты по ВУЗам. Платное обучение. 2012 год");
            model->setHeaderData( 0, Qt::Horizontal, QObject::tr("Название \n организации") );
            ui->tableView->setColumnWidth(0,580);

        }else if(str == "vyp_otr_platn_2012"){
            ui->lblnmoftable->setText("Эффективность выпуска с учетом дозащиты по отраслям. Платное обучение. 2012 год");
            model->setHeaderData( 0, Qt::Horizontal, QObject::tr("Название \n отрасли науки") );
            ui->tableView->setColumnWidth(0,300);

        }
        model->setHeaderData( 1, Qt::Horizontal, QObject::tr("Выпущено в 2012\n всего") );
        model->setHeaderData( 2, Qt::Horizontal, QObject::tr("Выпущено в 2012\n с защитой в срок") );
        model->setHeaderData( 3, Qt::Horizontal, QObject::tr("Выпущено в 2012\n с защитой в срок %") );
        model->setHeaderData( 4, Qt::Horizontal, QObject::tr("Защиты в 2012-2015") );
        model->setHeaderData( 5, Qt::Horizontal, QObject::tr("Всего защитилось %") );
        ui->tableView->hideColumn(6);
        ui->tableView->hideColumn(7);
        ui->tableView->hideColumn(8);
        ui->tableView->hideColumn(9);
        ui->tableView->setColumnWidth(1,145);
        ui->tableView->setColumnWidth(2,145);
        ui->tableView->setColumnWidth(3,145);
        ui->tableView->setColumnWidth(4,145);
        ui->tableView->setColumnWidth(5,145);

    }
    else if(str=="vyp_otr_platn_2013" || str=="filtr_vyp_vuz" && curvidobuch == "Платное обучение" && curyear == "2013"){
        if(str == "filtr_vyp_vuz"){
            ui->lblnmoftable->setText("Эффективность выпуска с учетом дозащиты по ВУЗам. Платное обучение. 2013 год");
            model->setHeaderData( 0, Qt::Horizontal, QObject::tr("Название \n организации") );
            ui->tableView->setColumnWidth(0,580);

        }else if(str == "vyp_otr_platn_2013"){
            ui->lblnmoftable->setText("Эффективность выпуска с учетом дозащиты по отраслям. Платное обучение. 2013 год");
            model->setHeaderData( 0, Qt::Horizontal, QObject::tr("Название \n отрасли науки") );
            ui->tableView->setColumnWidth(0,300);

        }
        model->setHeaderData( 1, Qt::Horizontal, QObject::tr("Выпущено в 2013\n всего") );
        model->setHeaderData( 2, Qt::Horizontal, QObject::tr("Выпущено в 2013\n с защитой в срок") );
        model->setHeaderData( 3, Qt::Horizontal, QObject::tr("Выпущено в 2013\n с защитой в срок %") );
        model->setHeaderData( 4, Qt::Horizontal, QObject::tr("Защиты в 2013-2015") );
        model->setHeaderData( 5, Qt::Horizontal, QObject::tr("Всего защитилось %") );
        ui->tableView->hideColumn(6);
        ui->tableView->hideColumn(7);
        ui->tableView->hideColumn(8);
        ui->tableView->hideColumn(9);
        ui->tableView->setColumnWidth(1,145);
        ui->tableView->setColumnWidth(2,145);
        ui->tableView->setColumnWidth(3,145);
        ui->tableView->setColumnWidth(4,145);
        ui->tableView->setColumnWidth(5,145);

    }
    else if(str=="vyp_otr_platn_2014" || str=="filtr_vyp_vuz" && curvidobuch == "Платное обучение" && curyear == "2014"){
        if(str == "filtr_vyp_vuz"){
            ui->lblnmoftable->setText("Эффективность выпуска с учетом дозащиты по ВУЗам. Платное обучение. 2014 год");
            model->setHeaderData( 0, Qt::Horizontal, QObject::tr("Название \n организации") );
            ui->tableView->setColumnWidth(0,580);

        }else if(str == "vyp_otr_platn_2014"){
            ui->lblnmoftable->setText("Эффективность выпуска с учетом дозащиты по отраслям. Платное обучение. 2014 год");
            model->setHeaderData( 0, Qt::Horizontal, QObject::tr("Название \n отрасли науки") );
            ui->tableView->setColumnWidth(0,300);

        }
        model->setHeaderData( 1, Qt::Horizontal, QObject::tr("Выпущено в 2014\n всего") );
        model->setHeaderData( 2, Qt::Horizontal, QObject::tr("Выпущено в 2014\n с защитой в срок") );
        model->setHeaderData( 3, Qt::Horizontal, QObject::tr("Выпущено в 2014\n с защитой в срок %") );
        model->setHeaderData( 4, Qt::Horizontal, QObject::tr("Защиты в 2014-2015") );
        model->setHeaderData( 5, Qt::Horizontal, QObject::tr("Всего защитилось %") );
        ui->tableView->hideColumn(6);
        ui->tableView->hideColumn(7);
        ui->tableView->hideColumn(8);
        ui->tableView->hideColumn(9);
        ui->tableView->setColumnWidth(1,145);
        ui->tableView->setColumnWidth(2,145);
        ui->tableView->setColumnWidth(3,145);
        ui->tableView->setColumnWidth(4,145);
        ui->tableView->setColumnWidth(5,145);

    }
    else if(str=="vyp_otr_platn_2015" || str=="filtr_vyp_vuz" && curvidobuch == "Платное обучение" && curyear == "2015"){
        if(str == "filtr_vyp_vuz"){
            ui->lblnmoftable->setText("Эффективность выпуска с учетом дозащиты по ВУЗам. Платное обучение. 2015 год");
            model->setHeaderData( 0, Qt::Horizontal, QObject::tr("Название \n организации") );
            ui->tableView->setColumnWidth(0,580);

        }else if(str == "vyp_otr_platn_2015"){
            ui->lblnmoftable->setText("Эффективность выпуска с учетом дозащиты по отраслям. Платное обучение. 2015 год");
            model->setHeaderData( 0, Qt::Horizontal, QObject::tr("Название \n отрасли науки") );
            ui->tableView->setColumnWidth(0,300);

        }
        model->setHeaderData( 1, Qt::Horizontal, QObject::tr("Выпущено в 2015\n всего") );
        model->setHeaderData( 2, Qt::Horizontal, QObject::tr("Выпущено в 2015\n с защитой в срок") );
        model->setHeaderData( 3, Qt::Horizontal, QObject::tr("Выпущено в 2015\n с защитой в срок %") );
        model->setHeaderData( 4, Qt::Horizontal, QObject::tr("Защиты в 2015") );
        model->setHeaderData( 5, Qt::Horizontal, QObject::tr("Всего защитилось %") );
        ui->tableView->hideColumn(6);
        ui->tableView->hideColumn(7);
        ui->tableView->hideColumn(8);
        ui->tableView->hideColumn(9);
        ui->tableView->setColumnWidth(1,145);
        ui->tableView->setColumnWidth(2,145);
        ui->tableView->setColumnWidth(3,145);
        ui->tableView->setColumnWidth(4,145);
        ui->tableView->setColumnWidth(5,145);

    }

}

//----------Фильтр по отраслям(combobox)-------------//
void MainWindow::on_comboBoxotr_currentIndexChanged(const QString &arg1)
{
    if(curanaliz == 1)
        analiz1_otr();
    else if(curanaliz == 3)
        analiz3_otr();
    ui->use_filtrs_button->setEnabled(true);

}

//------Фильтр по регионам(combobox)--------//
void MainWindow::on_comboBoxreg_currentIndexChanged(const QString &arg1)
{
    Q_UNUSED(arg1);

    add_elements_to_oblfiltr();
}


//Для sql скриптов ПРИМЕР - ExecuteSqlScriptFile(db, "/home/bigleb32/testdb1/ef1_ef2_vuz.sql");
int MainWindow::ExecuteSqlScriptFile(QSqlDatabase & db, const QString & fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return  0;

    QTextStream in(&file);
    QString sql = in.readAll();
    QStringList sqlStatements = sql.split(';', QString::SkipEmptyParts);
    int successCount = 0;

    foreach(const QString& statement, sqlStatements)
    {
        if (statement.trimmed() != "")
        {
            QSqlQuery query(db);
            if (query.exec(statement))
                successCount++;
            else
                qDebug() << "Failed:" << statement << "\nReason:" << query.lastError();
        }
    }
    return successCount;
}


//----------Создание sql запроса на фильтрацию таблицы(по географии) АНАЛИЗ 1-------//
void MainWindow::filtr_test1()
{
    QSqlQuery query;
    query.prepare("delete from filtr_test1");
    query.exec();

    QStringList list;
    int curyear, curvidobuch;
    curyear = ui->comboBox_year->currentIndex();
    curvidobuch = ui->comboBox_vidobuch->currentIndex();

    list<<ui->comboBoxreg->currentText()<<ui->comboBoxobl->currentText()<<ui->comboBoxcity->currentText();

    if(list[0]=="Выкл" && list[1]=="Выкл" && list[2]=="Выкл"){
        if(curvidobuch == 0){
            switch(curyear){
        case 0:
                query.prepare("insert into FILTR_TEST1 select test1_2011.* from test1_2011");
                break;
        case 1:
                query.prepare("insert into FILTR_TEST1 select test1_2012.* from test1_2012");
                break;
        case 2:
                query.prepare("insert into FILTR_TEST1 select test1_2013.* from test1_2013");
                break;
        case 3:
                query.prepare("insert into FILTR_TEST1 select test1_2014.* from test1_2014");
                break;
        case 4:
                query.prepare("insert into FILTR_TEST1 select test1_2015.* from test1_2015");
                break;
        }
        }else if(curvidobuch == 1){
            switch(curyear){
        case 0:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2011.* from test1_platn_2011");
                break;
        case 1:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2012.* from test1_platn_2012");
                break;
        case 2:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2013.* from test1_platn_2013");
                break;
        case 3:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2014.* from test1_platn_2014");
                break;
        case 4:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2015.* from test1_platn_2015");
                break;
        }
        }
      }

    if(list[0]!="Выкл" && list[1]=="Выкл" && list[2]=="Выкл"){
        if(curvidobuch == 0){
            switch(curyear){
        case 0:
                query.prepare("insert into FILTR_TEST1 select test1_2011.* from test1_2011 where region=(:reg)");
                break;
        case 1:
                query.prepare("insert into FILTR_TEST1 select test1_2012.* from test1_2012 where region=(:reg)");
                break;
        case 2:
                query.prepare("insert into FILTR_TEST1 select test1_2013.* from test1_2013 where region=(:reg)");
                break;
        case 3:
                query.prepare("insert into FILTR_TEST1 select test1_2014.* from test1_2014 where region=(:reg)");
                break;
        case 4:
                query.prepare("insert into FILTR_TEST1 select test1_2015.* from test1_2015 where region=(:reg)");
                break;
        }
        }else if(curvidobuch == 1){
            switch(curyear){
        case 0:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2011.* from test1_platn_2011 where region=(:reg)");
                break;
        case 1:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2012.* from test1_platn_2012 where region=(:reg)");
                break;
        case 2:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2013.* from test1_platn_2013 where region=(:reg)");
                break;
        case 3:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2014.* from test1_platn_2014 where region=(:reg)");
                break;
        case 4:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2015.* from test1_platn_2015 where region=(:reg)");
                break;
        }
    }
        query.bindValue(":reg",list[0]);

    }

    if(list[0]!="Выкл" && list[1]!="Выкл" && list[2]=="Выкл"){
        if(curvidobuch == 0){
            switch(curyear){
        case 0:
                query.prepare("insert into FILTR_TEST1 select test1_2011.* from test1_2011 where oblname=(:obl)");
                break;
        case 1:
                query.prepare("insert into FILTR_TEST1 select test1_2012.* from test1_2012 where oblname=(:obl)");
                break;
        case 2:
                query.prepare("insert into FILTR_TEST1 select test1_2013.* from test1_2013 where oblname=(:obl)");
                break;
        case 3:
                query.prepare("insert into FILTR_TEST1 select test1_2014.* from test1_2014 where oblname=(:obl)");
                break;
        case 4:
                query.prepare("insert into FILTR_TEST1 select test1_2015.* from test1_2015 where oblname=(:obl)");
                break;
        }
        }else if(curvidobuch == 1){
            switch(curyear){
        case 0:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2011.* from test1_platn_2011 where oblname=(:obl)");
                break;
        case 1:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2012.* from test1_platn_2012 where oblname=(:obl)");
                break;
        case 2:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2013.* from test1_platn_2013 where oblname=(:obl)");
                break;
        case 3:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2014.* from test1_platn_2014 where oblname=(:obl)");
                break;
        case 4:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2015.* from test1_platn_2015 where oblname=(:obl)");
                break;
        }
        }

        query.bindValue(":obl",list[1]);
    }

    if(list[0]!="Выкл" && list[1]!="Выкл" && list[2]!="Выкл"){
        if(curvidobuch == 0){
            switch(curyear){
        case 0:
                query.prepare("insert into FILTR_TEST1 select test1_2011.* from test1_2011 where city=(:cit)");
                break;
        case 1:
                query.prepare("insert into FILTR_TEST1 select test1_2012.* from test1_2012 where city=(:cit)");
                break;
        case 2:
                query.prepare("insert into FILTR_TEST1 select test1_2013.* from test1_2013 where city=(:cit)");
                break;
        case 3:
                query.prepare("insert into FILTR_TEST1 select test1_2014.* from test1_2014 where city=(:cit)");
                break;
        case 4:
                query.prepare("insert into FILTR_TEST1 select test1_2015.* from test1_2015 where city=(:cit)");
                break;
        }
        }else if(curvidobuch == 1){
            switch(curyear){
        case 0:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2011.* from test1_platn_2011 where city=(:cit)");
                break;
        case 1:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2012.* from test1_platn_2012 where city=(:cit)");
                break;
        case 2:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2013.* from test1_platn_2013 where city=(:cit)");
                break;
        case 3:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2014.* from test1_platn_2014 where city=(:cit)");
                break;
        case 4:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2015.* from test1_platn_2015 where city=(:cit)");
                break;
        }
        }
        query.bindValue(":cit",list[2]);
    }

    if(list[0]=="Выкл" && list[1]=="Выкл" && list[2]!="Выкл"){
        if(curvidobuch == 0){
            switch(curyear){
        case 0:
                query.prepare("insert into FILTR_TEST1 select test1_2011.* from test1_2011 where city=(:cit)");
                break;
        case 1:
                query.prepare("insert into FILTR_TEST1 select test1_2012.* from test1_2012 where city=(:cit)");
                break;
        case 2:
                query.prepare("insert into FILTR_TEST1 select test1_2013.* from test1_2013 where city=(:cit)");
                break;
        case 3:
                query.prepare("insert into FILTR_TEST1 select test1_2014.* from test1_2014 where city=(:cit)");
                break;
        case 4:
                query.prepare("insert into FILTR_TEST1 select test1_2015.* from test1_2015 where city=(:cit)");
                break;
        }
        }else if(curvidobuch == 1){
            switch(curyear){
        case 0:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2011.* from test1_platn_2011 where city=(:cit)");
                break;
        case 1:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2012.* from test1_platn_2012 where city=(:cit)");
                break;
        case 2:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2013.* from test1_platn_2013 where city=(:cit)");
                break;
        case 3:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2014.* from test1_platn_2014 where city=(:cit)");
                break;
        case 4:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2015.* from test1_platn_2015 where city=(:cit)");
                break;
        }
        }

        query.bindValue(":cit",list[2]);
    }

    if(list[0]=="Выкл" && list[1]!="Выкл" && list[2]!="Выкл"){
        if(curvidobuch == 0){
            switch(curyear){
        case 0:
                query.prepare("insert into FILTR_TEST1 select test1_2011.* from test1_2011 where city=(:cit)");
                break;
        case 1:
                query.prepare("insert into FILTR_TEST1 select test1_2012.* from test1_2012 where city=(:cit)");
                break;
        case 2:
                query.prepare("insert into FILTR_TEST1 select test1_2013.* from test1_2013 where city=(:cit)");
                break;
        case 3:
                query.prepare("insert into FILTR_TEST1 select test1_2014.* from test1_2014 where city=(:cit)");
                break;
        case 4:
                query.prepare("insert into FILTR_TEST1 select test1_2015.* from test1_2015 where city=(:cit)");
                break;
        }
        }else if(curvidobuch == 1){
            switch(curyear){
        case 0:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2011.* from test1_platn_2011 where city=(:cit)");
                break;
        case 1:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2012.* from test1_platn_2012 where city=(:cit)");
                break;
        case 2:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2013.* from test1_platn_2013 where city=(:cit)");
                break;
        case 3:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2014.* from test1_platn_2014 where city=(:cit)");
                break;
        case 4:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2015.* from test1_platn_2015 where city=(:cit)");
                break;
        }
        }
        query.bindValue(":cit",list[2]);
    }

    if(list[0]=="Выкл" && list[1]!="Выкл" && list[2]=="Выкл"){
        if(curvidobuch == 0){
            switch(curyear){
        case 0:
                query.prepare("insert into FILTR_TEST1 select test1_2011.* from test1_2011 where oblname=(:obl)");
                break;
        case 1:
                query.prepare("insert into FILTR_TEST1 select test1_2012.* from test1_2012 where oblname=(:obl)");
                break;
        case 2:
                query.prepare("insert into FILTR_TEST1 select test1_2013.* from test1_2013 where oblname=(:obl)");
                break;
        case 3:
                query.prepare("insert into FILTR_TEST1 select test1_2014.* from test1_2014 where oblname=(:obl)");
                break;
        case 4:
                query.prepare("insert into FILTR_TEST1 select test1_2015.* from test1_2015 where oblname=(:obl)");
                break;
        }
        }else if(curvidobuch == 1){
            switch(curyear){
        case 0:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2011.* from test1_platn_2011 where oblname=(:obl)");
                break;
        case 1:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2012.* from test1_platn_2012 where oblname=(:obl)");
                break;
        case 2:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2013.* from test1_platn_2013 where oblname=(:obl)");
                break;
        case 3:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2014.* from test1_platn_2014 where oblname=(:obl)");
                break;
        case 4:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2015.* from test1_platn_2015 where oblname=(:obl)");
                break;
        }
        }
        query.bindValue(":obl",list[1]);
    }

    if(list[0]!="Выкл" && list[1]=="Выкл" && list[2]!="Выкл"){
        if(curvidobuch == 0){
            switch(curyear){
        case 0:
                query.prepare("insert into FILTR_TEST1 select test1_2011.* from test1_2011 where city=(:cit)");
                break;
        case 1:
                query.prepare("insert into FILTR_TEST1 select test1_2012.* from test1_2012 where city=(:cit)");
                break;
        case 2:
                query.prepare("insert into FILTR_TEST1 select test1_2013.* from test1_2013 where city=(:cit)");
                break;
        case 3:
                query.prepare("insert into FILTR_TEST1 select test1_2014.* from test1_2014 where city=(:cit)");
                break;
        case 4:
                query.prepare("insert into FILTR_TEST1 select test1_2015.* from test1_2015 where city=(:cit)");
                break;
        }
        }else if(curvidobuch == 1){
            switch(curyear){
        case 0:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2011.* from test1_platn_2011 where city=(:cit)");
                break;
        case 1:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2012.* from test1_platn_2012 where city=(:cit)");
                break;
        case 2:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2013.* from test1_platn_2013 where city=(:cit)");
                break;
        case 3:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2014.* from test1_platn_2014 where city=(:cit)");
                break;
        case 4:
                query.prepare("insert into FILTR_TEST1 select test1_platn_2015.* from test1_platn_2015 where city=(:cit)");
                break;
        }
        }
        query.bindValue(":cit",list[2]);
    }

    query.exec();

    query.exec("create table filtr_test2 as select z1 as z1, out as out, outthesis as outthesis, awarded0 as awarded0, "
               "round(ef1)||'%' as ef1, round(ef2)||'%' as ef2, region as region, oblname as oblname, city as city, codras as codras "
               "from filtr_test1 order by z1 ASC");
    query.exec("drop table filtr_test1");
    query.exec("RENAME filtr_test2  to filtr_test1");
    query.exec("insert into filtr_test1(z1, out, outthesis, awarded0, ef1, ef2)"
               "select 'ИТОГ:', sum(out), sum(outthesis), sum(awarded0),  round((sum(outthesis)/nullif(sum(out),0))*100)||'%',"
               "round(((sum(outthesis)+sum(awarded0))/nullif(sum(out),0))*100)||'%' from filtr_test1");

    list.clear();

}

//----------Фильтр по областям(combobox)-------------//
void MainWindow::on_comboBoxobl_currentIndexChanged(const QString &arg1)
{
    Q_UNUSED(arg1);

    add_elements_to_citfiltr();
}

//----------Фильтр по городам(combobox)-------------//
void MainWindow::on_comboBoxcity_currentIndexChanged(const QString &arg1)
{
   Q_UNUSED(arg1);

   if(curanaliz == 1)
        filtr_test1();
   else if(curanaliz == 3)
        filtr_vyp_vuz();

    ui->use_filtrs_button->setEnabled(true);
}

//-------------Функции добавления элементов в фильтр по отраслям--------//
void MainWindow::add_elements_to_otrfiltr()
{
    QStringList cblist;
    QSqlQuery query;

    query.prepare("SELECT SPECIALITY FROM otrastable");
    query.exec();
    cblist<<"Выкл";
        while(query.next())
        {
            if (query.isValid())
            cblist<<query.value(0).toString();
        }
    ui->comboBoxotr->addItems(cblist);
    cblist.clear();
}

//-------------Функции добавления элементов в фильтр по регионам-----//
void MainWindow::add_elements_to_regfiltr()
{
    QStringList cblist;
    QSqlQuery query;
    query.prepare("SELECT DISTINCT region FROM orgtable ORDER BY region ASC");
    query.exec();
    cblist<<"Выкл";
        while(query.next())
        {
            if (query.isValid())
            cblist<<query.value(0).toString();
        }
    ui->comboBoxreg->addItems(cblist);
    cblist.clear();
}

//-------------Функции добавления элементов в фильтр по областям-----//
void MainWindow::add_elements_to_oblfiltr()
{
    QStringList cblist;
    QSqlQuery query;
    //Если по регионам фильтр выключен
    if(ui->comboBoxreg->currentText()=="Выкл"){
        ui->comboBoxobl->clear();
    query.prepare("SELECT DISTINCT oblname FROM orgtable ORDER BY oblname ASC");
    query.exec();
    cblist<<"Выкл";
        while(query.next())
        {
            if (query.isValid())
            cblist<<query.value(0).toString();
        }
    ui->comboBoxobl->addItems(cblist);
    cblist.clear();
    }
   //Если фильтр по регионам включен
    else{
        ui->comboBoxobl->clear();
        query.prepare("SELECT DISTINCT oblname FROM orgtable where region=(:reg) ORDER BY oblname ASC");
        query.bindValue(":reg", ui->comboBoxreg->currentText());
        query.exec();
        cblist<<"Выкл";
        while(query.next())
        {
            if (query.isValid())
            cblist<<query.value(0).toString();
        }
        ui->comboBoxobl->addItems(cblist);
        cblist.clear();
    }
}

//-------------Функции добавления элементов в фильтр по по городам-----//
void MainWindow::add_elements_to_citfiltr()
{
    QStringList cblist;
    QSqlQuery query;
    QStringList list;
//Текущие фильтры по регионам и областям
    list<<ui->comboBoxreg->currentText()<<ui->comboBoxobl->currentText();

//Если фильтры по регионам и областям отключены
    if(list[0]=="Выкл" && list[1]=="Выкл"){
        ui->comboBoxcity->clear();
        query.prepare("SELECT DISTINCT city FROM orgtable ORDER BY city ASC");
        query.exec();
        cblist<<"Выкл";
            while(query.next())
            {
                if (query.isValid())
                cblist<<query.value(0).toString();
            }
        ui->comboBoxcity->addItems(cblist);
        cblist.clear();
        }

//Если фильтры по регионам включен а по областям отключен
    if(list[0]!="Выкл" && list[1]=="Выкл"){

        ui->comboBoxcity->clear();
        query.prepare("SELECT DISTINCT city FROM orgtable where region=(:reg) ORDER BY city ASC");
        query.bindValue(":reg", list[0]);

        query.exec();
        cblist<<"Выкл";

            while(query.next())
            {
                if (query.isValid())
                cblist<<query.value(0).toString();
            }
        ui->comboBoxcity->addItems(cblist);
        cblist.clear();
    }

//Если фильтры по регионам выключен а по областям включен
    if(list[0]=="Выкл" && list[1]!="Выкл"){
        ui->comboBoxcity->clear();
        query.prepare("SELECT DISTINCT city FROM orgtable where oblname=(:obl) ORDER BY city ASC");
        query.bindValue(":obl", ui->comboBoxobl->currentText());

        query.exec();
        cblist<<"Выкл";

            while(query.next())
            {
                if (query.isValid())
                cblist<<query.value(0).toString();
            }
        ui->comboBoxcity->addItems(cblist);
        cblist.clear();
    }

//Если фильтры по регионам и областям включены
    if(list[0]!="Выкл" && list[1]!="Выкл" && ui->comboBoxobl->currentText()!=""){
        ui->comboBoxcity->clear();
        query.prepare("SELECT DISTINCT city FROM orgtable where oblname=(:obl) ORDER BY city ASC");
        query.bindValue(":obl", ui->comboBoxobl->currentText());

        query.exec();
        cblist<<"Выкл";

            while(query.next())
            {
                if (query.isValid())
                cblist<<query.value(0).toString();
            }
        ui->comboBoxcity->addItems(cblist);
        cblist.clear();
    }
}


//Выключение фильтра по отраслям(по checkbox)
void MainWindow::on_checkBoxotr_clicked(bool checked)
{
    if(checked==true){
        ui->comboBoxreg->setCurrentIndex(0);
        ui->comboBoxobl->setCurrentIndex(0);
        ui->comboBoxcity->setCurrentIndex(0);
    }
    else{
        ui->comboBoxotr->setCurrentIndex(0);
    }
}

//ПЕЧАТЬ ОТЧЕТА
void MainWindow::Otchet()
{
    LimeReport::ReportEngine *report = new LimeReport::ReportEngine(this);

    if(curtable=="testotr_2011" || curtable=="testotr_2012"
                || curtable=="testotr_2013" || curtable=="testotr_2014"
                || curtable=="testotr_2015" || curtable=="testotr_2011_platn"
                || curtable=="testotr_2012_platn" ||  curtable=="testotr_2013_platn"
                ||  curtable=="testotr_2014_platn" ||  curtable=="testotr_2015_platn"){
        curtable = "testotr";
    }
    else if(curtable=="vyp_otr_platn_2011" )
        curtable = "vyp_otr_2011";
    else if(curtable=="vyp_otr_platn_2012" )
        curtable = "vyp_otr_2012";
    else if(curtable=="vyp_otr_platn_2013" )
        curtable = "vyp_otr_2013";
    else if(curtable=="vyp_otr_platn_2014" )
        curtable = "vyp_otr_2014";
    else if(curtable=="vyp_otr_platn_2015" )
        curtable = "vyp_otr_2015";
    else if(curtable=="filtr_vyp_vuz" && ui->comboBox_year->currentText() == "2011" )
        curtable = "vyp_vuz_2011";
    else if(curtable=="filtr_vyp_vuz" && ui->comboBox_year->currentText() == "2012" )
        curtable = "vyp_vuz_2012";
    else if(curtable=="filtr_vyp_vuz" && ui->comboBox_year->currentText() == "2013" )
        curtable = "vyp_vuz_2013";
    else if(curtable=="filtr_vyp_vuz" && ui->comboBox_year->currentText() == "2014" )
        curtable = "vyp_vuz_2014";
    else if(curtable=="filtr_vyp_vuz" && ui->comboBox_year->currentText() == "2015" )
        curtable = "vyp_vuz_2015";

    report->loadFromFile("/home/bigleb32/diplomprog/reports/"+curtable+"_rep.lrxml");

    report->dataManager()->addModel("table_model",ui->tableView->model(), false);


    report->dataManager()->setReportVariable("otr",ui->comboBoxotr->currentText());
    report->dataManager()->setReportVariable("reg",ui->comboBoxreg->currentText());
    report->dataManager()->setReportVariable("obl",ui->comboBoxobl->currentText());
    report->dataManager()->setReportVariable("cit",ui->comboBoxobl->currentText());
    report->dataManager()->setReportVariable("vid",ui->comboBox_vidobuch->currentText());
    report->dataManager()->setReportVariable("year",ui->comboBox_year->currentText());
    report->dataManager()->setReportVariable("column_1_name",ui->tableView->model()->headerData(0, Qt::Horizontal).toString());
    report->dataManager()->setReportVariable("column_2_name",ui->tableView->model()->headerData(1, Qt::Horizontal).toString());
    report->dataManager()->setReportVariable("column_3_name",ui->tableView->model()->headerData(2, Qt::Horizontal).toString());
    report->dataManager()->setReportVariable("column_4_name",ui->tableView->model()->headerData(3, Qt::Horizontal).toString());
    report->dataManager()->setReportVariable("column_5_name",ui->tableView->model()->headerData(4, Qt::Horizontal).toString());
    report->dataManager()->setReportVariable("column_6_name",ui->tableView->model()->headerData(5, Qt::Horizontal).toString());

    report->previewReport();

}

//Кнопка применения фильтров
void MainWindow::on_use_filtrs_button_clicked()
{
    if(curanaliz == 1)
        slotShowAction("filtr_test1");
    else if(curanaliz == 2)
        slotShowAction(curtable);
    else if(curanaliz == 3)
        slotShowAction("filtr_vyp_vuz");
    else if(curanaliz == 4)
        slotShowAction(curtable);

    ui->use_filtrs_button->setEnabled(false);
    ui->report_Button->setEnabled(true);

}

//Открытие АНАЛИЗ-->Выпуск аспирантов...по ВУЗам
void MainWindow::on_actionVypVuz_triggered()
{
    curanaliz = ANALIZ_3;

    ui->checkBoxfiltrs->setHidden(false);
    ui->report_Button->setEnabled(true);
    ui->action->setEnabled(false);



    ui->checkBoxotr->setEnabled(true);
    ui->checkBoxotr->setChecked(false);
    ui->comboBoxreg->setEnabled(true);
    ui->comboBoxobl->setEnabled(true);
    ui->comboBoxcity->setEnabled(true);
    ui->comboBoxotr->setEnabled(false);
    ui->label_2->setEnabled(true);
    ui->label_3->setEnabled(true);
    ui->label_4->setEnabled(true);

    //Возвращение фильтров в состояние по умолчанию при переключении форм анализа
    ui->comboBoxreg->setCurrentIndex(0);
    ui->comboBoxobl->setCurrentIndex(0);
    ui->comboBoxcity->setCurrentIndex(0);
    ui->comboBoxotr->setCurrentIndex(0);
    ui->comboBox_vidobuch->setCurrentIndex(0);
    ui->comboBox_year->setCurrentIndex(0);
    ui->use_filtrs_button->setEnabled(false);

    curvyp_vuz_otr = "vuz";

    ui->comboBox_vidobuch->setCurrentIndex(0);
}

//combobox для выбора года выпуска
void MainWindow::on_comboBox_year_currentIndexChanged(int index)
{
    if(curanaliz==1)
        analiz1_otr();
    else if(curanaliz==3)
        analiz3_otr();
    else if(curanaliz==2){
        if(index==0 && ui->comboBox_vidobuch->currentIndex() == 0)
            curtable = "testotr_2011";
        else if(index==1 && ui->comboBox_vidobuch->currentIndex() == 0)
            curtable = "testotr_2012";
        else if(index==2 && ui->comboBox_vidobuch->currentIndex() == 0)
            curtable = "testotr_2013";
        else if(index==3 && ui->comboBox_vidobuch->currentIndex() == 0)
            curtable = "testotr_2014";
        else if(index==4 && ui->comboBox_vidobuch->currentIndex() == 0)
            curtable = "testotr_2015";
        else if(index==0 && ui->comboBox_vidobuch->currentIndex() == 1)
            curtable = "testotr_2011_platn";
        else if(index==1 && ui->comboBox_vidobuch->currentIndex() == 1)
            curtable = "testotr_2012_platn";
        else if(index==2 && ui->comboBox_vidobuch->currentIndex() == 1)
            curtable = "testotr_2013_platn";
        else if(index==3 && ui->comboBox_vidobuch->currentIndex() == 1)
            curtable = "testotr_2014_platn";
        else if(index==4 && ui->comboBox_vidobuch->currentIndex() == 1)
            curtable = "testotr_2015_platn";
    }
    else if(curanaliz==4){
        if(index==0 && ui->comboBox_vidobuch->currentIndex() == 0)
            curtable = "vyp_otr_2011";
        else if(index==1 && ui->comboBox_vidobuch->currentIndex() == 0)
            curtable = "vyp_otr_2012";
        else if(index==2 && ui->comboBox_vidobuch->currentIndex() == 0)
            curtable = "vyp_otr_2013";
        else if(index==3 && ui->comboBox_vidobuch->currentIndex() == 0)
            curtable = "vyp_otr_2014";
        else if(index==4 && ui->comboBox_vidobuch->currentIndex() == 0)
            curtable = "vyp_otr_2015";
        else if(index==0 && ui->comboBox_vidobuch->currentIndex() == 1)
            curtable = "vyp_otr_platn_2011";
        else if(index==1 && ui->comboBox_vidobuch->currentIndex() == 1)
            curtable = "vyp_otr_platn_2012";
        else if(index==2 && ui->comboBox_vidobuch->currentIndex() == 1)
            curtable = "vyp_otr_platn_2013";
        else if(index==3 && ui->comboBox_vidobuch->currentIndex() == 1)
            curtable = "vyp_otr_platn_2014";
        else if(index==4 && ui->comboBox_vidobuch->currentIndex() == 1)
            curtable = "vyp_otr_platn_2015";
    }


    ui->use_filtrs_button->setEnabled(true);

}

//combobox для выбора вида обучения(платное, бюджетное)
void MainWindow::on_comboBox_vidobuch_currentIndexChanged(int index)
{
    if(curanaliz==1)
        analiz1_otr();
    else if(curanaliz==3)
        analiz3_otr();
    else if(curanaliz==2){
        if(index == 0 && ui->comboBox_year->currentIndex() == 0)
            curtable = "testotr_2011";
        else if(index == 0 && ui->comboBox_year->currentIndex() == 1)
            curtable = "testotr_2012";
        else if(index == 0 && ui->comboBox_year->currentIndex() == 2)
            curtable = "testotr_2013";
        else if(index == 0 && ui->comboBox_year->currentIndex() == 3)
            curtable = "testotr_2014";
        else if(index == 0 && ui->comboBox_year->currentIndex() == 4)
            curtable = "testotr_2015";
        else if(index == 1 && ui->comboBox_year->currentIndex() == 0)
            curtable = "testotr_2011_platn";
        else if(index == 1 && ui->comboBox_year->currentIndex() == 1)
            curtable = "testotr_2012_platn";
        else if(index == 1 && ui->comboBox_year->currentIndex() == 2)
            curtable = "testotr_2013_platn";
        else if(index == 1 && ui->comboBox_year->currentIndex() == 3)
            curtable = "testotr_2014_platn";
        else if(index == 1 && ui->comboBox_year->currentIndex() == 4)
            curtable = "testotr_2015_platn";
    }
    else if(curanaliz==4){
        if(index == 0 && ui->comboBox_year->currentIndex() == 0)
            curtable = "vyp_otr_2011";
        else if(index == 0 && ui->comboBox_year->currentIndex() == 1)
            curtable = "vyp_otr_2012";
        else if(index == 0 && ui->comboBox_year->currentIndex() == 2)
            curtable = "vyp_otr_2013";
        else if(index == 0 && ui->comboBox_year->currentIndex() == 3)
            curtable = "vyp_otr_2014";
        else if(index == 0 && ui->comboBox_year->currentIndex() == 4)
            curtable = "vyp_otr_2015";
        else if(index == 1 && ui->comboBox_year->currentIndex() == 0)
            curtable = "vyp_otr_platn_2011";
        else if(index == 1 && ui->comboBox_year->currentIndex() == 1)
            curtable = "vyp_otr_platn_2012";
        else if(index == 1 && ui->comboBox_year->currentIndex() == 2)
            curtable = "vyp_otr_platn_2013";
        else if(index == 1 && ui->comboBox_year->currentIndex() == 3)
            curtable = "vyp_otr_platn_2014";
        else if(index == 1 && ui->comboBox_year->currentIndex() == 4)
            curtable = "vyp_otr_platn_2015";
    }



    ui->use_filtrs_button->setEnabled(true);

}

//Открытие АНАЛИЗ-->Выпуск аспирантов...по отраслям
void MainWindow::on_actionVypOtr_triggered()
{
    curanaliz = ANALIZ_4;

    ui->checkBoxfiltrs->setHidden(false);
    ui->report_Button->setEnabled(true);
    ui->action->setEnabled(true);



    ui->checkBoxotr->setEnabled(false);
    ui->comboBoxreg->setEnabled(false);
    ui->comboBoxobl->setEnabled(false);
    ui->comboBoxcity->setEnabled(false);
    ui->comboBoxotr->setEnabled(false);
    ui->label_2->setEnabled(false);
    ui->label_3->setEnabled(false);
    ui->label_4->setEnabled(false);


//Возвращение фильтров в состояние по умолчанию при переключении форм анализа
    ui->comboBoxreg->setCurrentIndex(0);
    ui->comboBoxobl->setCurrentIndex(0);
    ui->comboBoxcity->setCurrentIndex(0);
    ui->comboBoxotr->setCurrentIndex(0);
    ui->comboBox_vidobuch->setCurrentIndex(0);
    ui->comboBox_year->setCurrentIndex(0);
    ui->use_filtrs_button->setEnabled(false);

    curvyp_vuz_otr = "otr";
    slotShowAction("vyp_otr_2011");
}



void MainWindow::on_report_Button_clicked()
{
    Otchet();

}


//Возвращение фильтров в состояние по умолчанию при переключении форм анализа
void MainWindow::on_actionEfotr_triggered()
{
    curanaliz = ANALIZ_2;

    ui->checkBoxfiltrs->setHidden(false);
    ui->report_Button->setEnabled(true);
    ui->action->setEnabled(true);

    ui->comboBoxreg->setCurrentIndex(0);
    ui->comboBoxobl->setCurrentIndex(0);
    ui->comboBoxcity->setCurrentIndex(0);
    ui->comboBoxotr->setCurrentIndex(0);

    ui->checkBoxotr->setEnabled(false);
    ui->comboBoxreg->setEnabled(false);
    ui->comboBoxobl->setEnabled(false);
    ui->comboBoxcity->setEnabled(false);
    ui->comboBoxotr->setEnabled(false);
    ui->label_2->setEnabled(false);
    ui->label_3->setEnabled(false);
    ui->label_4->setEnabled(false);


    ui->comboBox_vidobuch->setCurrentIndex(0);
    ui->comboBox_year->setCurrentIndex(0);
    ui->use_filtrs_button->setEnabled(false);
}

//Возвращение фильтров в состояние по умолчанию при переключении форм анализа
void MainWindow::on_actionEfvuz_triggered()
{
    curanaliz = ANALIZ_1;

    ui->checkBoxfiltrs->setHidden(false);
    ui->report_Button->setEnabled(true);
    ui->action->setEnabled(false);



    ui->checkBoxotr->setEnabled(true);
    ui->checkBoxotr->setChecked(false);
    ui->comboBoxreg->setEnabled(true);
    ui->comboBoxobl->setEnabled(true);
    ui->comboBoxcity->setEnabled(true);
    ui->comboBoxotr->setEnabled(false);
    ui->label_2->setEnabled(true);
    ui->label_3->setEnabled(true);
    ui->label_4->setEnabled(true);

    ui->comboBoxreg->setCurrentIndex(0);
    ui->comboBoxobl->setCurrentIndex(0);
    ui->comboBoxcity->setCurrentIndex(0);
    ui->comboBoxotr->setCurrentIndex(0);
    ui->comboBox_vidobuch->setCurrentIndex(0);
    ui->comboBox_year->setCurrentIndex(0);
    ui->use_filtrs_button->setEnabled(false);
}

//фильтрация по отраслям АНАЛИЗ1
void MainWindow::analiz1_otr()
{
    QSqlQuery query;
    QString curind;

    if(ui->comboBoxotr->currentText()=="Выкл"){
        filtr_test1();

    }
    else {
    query.prepare("select substr(ind,1,2) from otrastable where SPECIALITY=(:otr)");
    query.bindValue(":otr", ui->comboBoxotr->currentText());
    query.exec();
    query.first();
    curind = query.value(0).toString();

    query.prepare("delete from filtr_test1");
    query.exec();

    if(ui->comboBox_vidobuch->currentIndex() == 0)
        query.prepare("insert into filtr_test1(codras, out, outthesis, awarded0, ef1, ef2)"
                      "select  codras, sum(OUT), sum(outthesis), sum(AWARDED0), (sum(outthesis)/nullif(sum(out),0))*100,"
                      "((sum(outthesis)+sum(awarded0))/nullif(sum(out),0))*100 "
                      "from table9 where substr(ind,1,2)=(:ind) and year in (:year) group by codras order by codras ASC");
    else if(ui->comboBox_vidobuch->currentIndex() == 1)
        query.prepare("insert into filtr_test1(codras, out, outthesis, awarded0, ef1, ef2)"
                      "select  codras, sum(OUT), sum(outthesis), sum(AWARDED0), (sum(outthesis)/nullif(sum(out),0))*100,"
                      "((sum(outthesis)+sum(awarded0))/nullif(sum(out),0))*100 "
                      "from table10 where substr(ind,1,2)=(:ind) and year in (:year) group by codras order by codras ASC");



    query.bindValue(":ind",curind);
    query.bindValue(":year", ui->comboBox_year->currentText());
    query.exec();

    query.exec("update FILTR_TEST1 set z1 = (select z1 from orgtable where filtr_test1.codras=orgtable.codras)");
    query.exec("update FILTR_TEST1 set region = (select region from orgtable where filtr_test1.codras=orgtable.codras)");
    query.exec("update FILTR_TEST1 set oblname = (select oblname from orgtable where filtr_test1.codras=orgtable.codras)");
    query.exec("update FILTR_TEST1 set city = (select city from orgtable where filtr_test1.codras=orgtable.codras)");
    query.exec("create table filtr_test2 as select z1 as z1, out as out, outthesis as outthesis, awarded0 as awarded0, "
               "round(ef1)||'%' as ef1, round(ef2)||'%' as ef2, region as region, oblname as oblname, city as city, codras as codras "
               "from filtr_test1 order by z1 ASC");

    query.exec("drop table filtr_test1");
    query.exec("RENAME filtr_test2  to filtr_test1");
    query.exec("insert into filtr_test1(z1, out, outthesis, awarded0, ef1, ef2)"
               "select 'ИТОГ:', sum(out), sum(outthesis), sum(awarded0),  round((sum(outthesis)/nullif(sum(out),0))*100)||'%',"
               "round(((sum(outthesis)+sum(awarded0))/nullif(sum(out),0))*100)||'%' from filtr_test1");

    }

}

//фильтрация по отраслям АНАЛИЗ3
void MainWindow::analiz3_otr()
{

    QSqlQuery query;
    QString curind;

    if(ui->comboBoxotr->currentText()=="Выкл"){
        filtr_vyp_vuz();
    }
    else {
    query.prepare("select substr(ind,1,2) from otrastable where SPECIALITY=(:otr)");
    query.bindValue(":otr", ui->comboBoxotr->currentText());
    query.exec();
    query.first();
    curind = query.value(0).toString();

    query.prepare("delete from filtr_vyp_vuz");
    query.exec();

    if(ui->comboBox_vidobuch->currentIndex() == 0)
        query.prepare("insert into filtr_vyp_vuz(codras, out, outthesis,  ef1, awarded0, ef2)"
                      "select  codras, sum(OUT), sum(outthesis), (sum(outthesis)/nullif(sum(out),0))*100, "
                      "sum(AWARDED0), ((sum(outthesis)+sum(awarded0))/nullif(sum(out),0))*100 "
                      "from table9 where substr(ind,1,2)=(:ind) and year in (:year) group by codras order by codras ASC");
    else if(ui->comboBox_vidobuch->currentIndex() == 1)
        query.prepare("insert into filtr_vyp_vuz(codras, out, outthesis,  ef1, awarded0, ef2)"
                      "select  codras, sum(OUT), sum(outthesis), (sum(outthesis)/nullif(sum(out),0))*100, "
                      "sum(AWARDED0), ((sum(outthesis)+sum(awarded0))/nullif(sum(out),0))*100 "
                      "from table10 where substr(ind,1,2)=(:ind) and year in (:year) group by codras order by codras ASC");



    query.bindValue(":ind",curind);
    query.bindValue(":year", ui->comboBox_year->currentText());
    query.exec();

    query.exec("update filtr_vyp_vuz set z1 = (select z1 from orgtable where filtr_vyp_vuz.codras=orgtable.codras)");
    query.exec("update filtr_vyp_vuz set region = (select region from orgtable where filtr_vyp_vuz.codras=orgtable.codras)");
    query.exec("update filtr_vyp_vuz set oblname = (select oblname from orgtable where filtr_vyp_vuz.codras=orgtable.codras)");
    query.exec("update filtr_vyp_vuz set city = (select city from orgtable where filtr_vyp_vuz.codras=orgtable.codras)");
    query.exec("create table filtr_vyp_vuz2 as select z1 as z1, out as out, outthesis as outthesis, "
               "round(ef1)||'%' as ef1, awarded0 as awarded0, round(ef2)||'%' as ef2, region as region, oblname as oblname, city as city, codras as codras "
               "from filtr_vyp_vuz order by z1 ASC");

    query.exec("drop table filtr_vyp_vuz");
    query.exec("RENAME filtr_vyp_vuz2  to filtr_vyp_vuz");
    query.exec("insert into filtr_vyp_vuz(z1, out, outthesis,  ef1, awarded0, ef2)"
               "select 'ИТОГ:', sum(out), sum(outthesis),  round((sum(outthesis)/nullif(sum(out),0))*100)||'%',  sum(awarded0), "
               "round(((sum(outthesis)+sum(awarded0))/nullif(sum(out),0))*100)||'%' from filtr_vyp_vuz");
    }

}

//----------Создание sql запроса на фильтрацию таблицы(по географии) АНАЛИЗ 3-------//
void MainWindow::filtr_vyp_vuz()
{
    QSqlQuery query;
    query.prepare("delete from filtr_vyp_vuz");
    query.exec();

    QStringList list;
    int curyear, curvidobuch;
    curyear = ui->comboBox_year->currentIndex();
    curvidobuch = ui->comboBox_vidobuch->currentIndex();

    list<<ui->comboBoxreg->currentText()<<ui->comboBoxobl->currentText()<<ui->comboBoxcity->currentText();

    if(list[0]=="Выкл" && list[1]=="Выкл" && list[2]=="Выкл"){
        if(curvidobuch == 0){
            switch(curyear){
        case 0:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2011.* from vyp_vuz_2011");
                break;
        case 1:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2012.* from vyp_vuz_2012");
                break;
        case 2:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2013.* from vyp_vuz_2013");
                break;
        case 3:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2014.* from vyp_vuz_2014");
                break;
        case 4:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2015.* from vyp_vuz_2015");
                break;
        }
        }else if(curvidobuch == 1){
            switch(curyear){
        case 0:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2011.* from vyp_vuz_platn_2011");
                break;
        case 1:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2012.* from vyp_vuz_platn_2012");
                break;
        case 2:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2013.* from vyp_vuz_platn_2013");
                break;
        case 3:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2014.* from vyp_vuz_platn_2014");
                break;
        case 4:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2015.* from vyp_vuz_platn_2015");
                break;
        }
        }
      }

    if(list[0]!="Выкл" && list[1]=="Выкл" && list[2]=="Выкл"){
        if(curvidobuch == 0){
            switch(curyear){
        case 0:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2011.* from vyp_vuz_2011 where region=(:reg)");
                break;
        case 1:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2012.* from vyp_vuz_2012 where region=(:reg)");
                break;
        case 2:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2013.* from vyp_vuz_2013 where region=(:reg)");
                break;
        case 3:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2014.* from vyp_vuz_2014 where region=(:reg)");
                break;
        case 4:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2015.* from vyp_vuz_2015 where region=(:reg)");
                break;
        }
        }else if(curvidobuch == 1){
            switch(curyear){
        case 0:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2011.* from vyp_vuz_platn_2011 where region=(:reg)");
                break;
        case 1:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2012.* from vyp_vuz_platn_2012 where region=(:reg)");
                break;
        case 2:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2013.* from vyp_vuz_platn_2013 where region=(:reg)");
                break;
        case 3:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2014.* from vyp_vuz_platn_2014 where region=(:reg)");
                break;
        case 4:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2015.* from vyp_vuz_platn_2015 where region=(:reg)");
                break;
        }
    }
        query.bindValue(":reg",list[0]);

    }

    if(list[0]!="Выкл" && list[1]!="Выкл" && list[2]=="Выкл"){
        if(curvidobuch == 0){
            switch(curyear){
        case 0:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2011.* from vyp_vuz_2011 where oblname=(:obl)");
                break;
        case 1:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2012.* from vyp_vuz_2012 where oblname=(:obl)");
                break;
        case 2:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2013.* from vyp_vuz_2013 where oblname=(:obl)");
                break;
        case 3:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2014.* from vyp_vuz_2014 where oblname=(:obl)");
                break;
        case 4:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2015.* from vyp_vuz_2015 where oblname=(:obl)");
                break;
        }
        }else if(curvidobuch == 1){
            switch(curyear){
        case 0:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2011.* from vyp_vuz_platn_2011 where oblname=(:obl)");
                break;
        case 1:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2012.* from vyp_vuz_platn_2012 where oblname=(:obl)");
                break;
        case 2:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2013.* from vyp_vuz_platn_2013 where oblname=(:obl)");
                break;
        case 3:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2014.* from vyp_vuz_platn_2014 where oblname=(:obl)");
                break;
        case 4:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2015.* from vyp_vuz_platn_2015 where oblname=(:obl)");
                break;
        }
        }

        query.bindValue(":obl",list[1]);
    }

    if(list[0]!="Выкл" && list[1]!="Выкл" && list[2]!="Выкл"){
        if(curvidobuch == 0){
            switch(curyear){
        case 0:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2011.* from vyp_vuz_2011 where city=(:cit)");
                break;
        case 1:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2012.* from vyp_vuz_2012 where city=(:cit)");
                break;
        case 2:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2013.* from vyp_vuz_2013 where city=(:cit)");
                break;
        case 3:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2014.* from vyp_vuz_2014 where city=(:cit)");
                break;
        case 4:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2015.* from vyp_vuz_2015 where city=(:cit)");
                break;
        }
        }else if(curvidobuch == 1){
            switch(curyear){
        case 0:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2011.* from vyp_vuz_platn_2011 where city=(:cit)");
                break;
        case 1:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2012.* from vyp_vuz_platn_2012 where city=(:cit)");
                break;
        case 2:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2013.* from vyp_vuz_platn_2013 where city=(:cit)");
                break;
        case 3:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2014.* from vyp_vuz_platn_2014 where city=(:cit)");
                break;
        case 4:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2015.* from vyp_vuz_platn_2015 where city=(:cit)");
                break;
        }
        }
        query.bindValue(":cit",list[2]);
    }

    if(list[0]=="Выкл" && list[1]=="Выкл" && list[2]!="Выкл"){
        if(curvidobuch == 0){
            switch(curyear){
        case 0:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2011.* from vyp_vuz_2011 where city=(:cit)");
                break;
        case 1:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2012.* from vyp_vuz_2012 where city=(:cit)");
                break;
        case 2:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2013.* from vyp_vuz_2013 where city=(:cit)");
                break;
        case 3:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2014.* from vyp_vuz_2014 where city=(:cit)");
                break;
        case 4:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2015.* from vyp_vuz_2015 where city=(:cit)");
                break;
        }
        }else if(curvidobuch == 1){
            switch(curyear){
        case 0:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2011.* from vyp_vuz_platn_2011 where city=(:cit)");
                break;
        case 1:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2012.* from vyp_vuz_platn_2012 where city=(:cit)");
                break;
        case 2:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2013.* from vyp_vuz_platn_2013 where city=(:cit)");
                break;
        case 3:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2014.* from vyp_vuz_platn_2014 where city=(:cit)");
                break;
        case 4:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2015.* from vyp_vuz_platn_2015 where city=(:cit)");
                break;
        }
        }

        query.bindValue(":cit",list[2]);
    }

    if(list[0]=="Выкл" && list[1]!="Выкл" && list[2]!="Выкл"){
        if(curvidobuch == 0){
            switch(curyear){
        case 0:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2011.* from vyp_vuz_2011 where city=(:cit)");
                break;
        case 1:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2012.* from vyp_vuz_2012 where city=(:cit)");
                break;
        case 2:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2013.* from vyp_vuz_2013 where city=(:cit)");
                break;
        case 3:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2014.* from vyp_vuz_2014 where city=(:cit)");
                break;
        case 4:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2015.* from vyp_vuz_2015 where city=(:cit)");
                break;
        }
        }else if(curvidobuch == 1){
            switch(curyear){
        case 0:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2011.* from vyp_vuz_platn_2011 where city=(:cit)");
                break;
        case 1:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2012.* from vyp_vuz_platn_2012 where city=(:cit)");
                break;
        case 2:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2013.* from vyp_vuz_platn_2013 where city=(:cit)");
                break;
        case 3:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2014.* from vyp_vuz_platn_2014 where city=(:cit)");
                break;
        case 4:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2015.* from vyp_vuz_platn_2015 where city=(:cit)");
                break;
        }
        }
        query.bindValue(":cit",list[2]);
    }

    if(list[0]=="Выкл" && list[1]!="Выкл" && list[2]=="Выкл"){
        if(curvidobuch == 0){
            switch(curyear){
        case 0:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2011.* from vyp_vuz_2011 where oblname=(:obl)");
                break;
        case 1:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2012.* from vyp_vuz_2012 where oblname=(:obl)");
                break;
        case 2:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2013.* from vyp_vuz_2013 where oblname=(:obl)");
                break;
        case 3:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2014.* from vyp_vuz_2014 where oblname=(:obl)");
                break;
        case 4:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2015.* from vyp_vuz_2015 where oblname=(:obl)");
                break;
        }
        }else if(curvidobuch == 1){
            switch(curyear){
        case 0:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2011.* from vyp_vuz_platn_2011 where oblname=(:obl)");
                break;
        case 1:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2012.* from vyp_vuz_platn_2012 where oblname=(:obl)");
                break;
        case 2:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2013.* from vyp_vuz_platn_2013 where oblname=(:obl)");
                break;
        case 3:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2014.* from vyp_vuz_platn_2014 where oblname=(:obl)");
                break;
        case 4:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2015.* from vyp_vuz_platn_2015 where oblname=(:obl)");
                break;
        }
        }
        query.bindValue(":obl",list[1]);
    }

    if(list[0]!="Выкл" && list[1]=="Выкл" && list[2]!="Выкл"){
        if(curvidobuch == 0){
            switch(curyear){
        case 0:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2011.* from vyp_vuz_2011 where city=(:cit)");
                break;
        case 1:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2012.* from vyp_vuz_2012 where city=(:cit)");
                break;
        case 2:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2013.* from vyp_vuz_2013 where city=(:cit)");
                break;
        case 3:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2014.* from vyp_vuz_2014 where city=(:cit)");
                break;
        case 4:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_2015.* from vyp_vuz_2015 where city=(:cit)");
                break;
        }
        }else if(curvidobuch == 1){
            switch(curyear){
        case 0:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2011.* from vyp_vuz_platn_2011 where city=(:cit)");
                break;
        case 1:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2012.* from vyp_vuz_platn_2012 where city=(:cit)");
                break;
        case 2:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2013.* from vyp_vuz_platn_2013 where city=(:cit)");
                break;
        case 3:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2014.* from vyp_vuz_platn_2014 where city=(:cit)");
                break;
        case 4:
                query.prepare("insert into FILTR_VYP_VUZ select vyp_vuz_platn_2015.* from vyp_vuz_platn_2015 where city=(:cit)");
                break;
        }
        }
        query.bindValue(":cit",list[2]);
    }

    query.exec();

    query.exec("create table filtr_vyp_vuz2 as select z1 as z1, out as out, outthesis as outthesis, "
               "ef1 as ef1, awarded0 as awarded0, ef2 as ef2, codras as codras, "
               "region as region, oblname as oblname, city as city from filtr_vyp_vuz order by z1 ASC");
    query.exec("drop table filtr_vyp_vuz");
    query.exec("RENAME  filtr_vyp_vuz2  to  filtr_vyp_vuz");
    query.exec("insert into  filtr_vyp_vuz(z1, out, outthesis,  ef1, awarded0, ef2)"
               "select 'ИТОГ:', sum(out), sum(outthesis),  round((sum(outthesis)/nullif(sum(out),0))*100)||'%',"
               "sum(awarded0), round(((sum(outthesis)+sum(awarded0))/nullif(sum(out),0))*100)||'%' from filtr_vyp_vuz");

    list.clear();

}


void MainWindow::on_actionTable9_triggered()
{
    ui->checkBoxfiltrs->setHidden(true);
    ui->checkBoxfiltrs->setChecked(false);
    ui->framefiltrs->setHidden(true);
    ui->report_Button->setEnabled(false);

}

void MainWindow::on_actionTable10_2_triggered()
{
    ui->checkBoxfiltrs->setHidden(true);
    ui->checkBoxfiltrs->setChecked(false);
    ui->framefiltrs->setHidden(true);
    ui->report_Button->setEnabled(false);

}

void MainWindow::on_actionOrgtable_triggered()
{
    ui->checkBoxfiltrs->setHidden(true);
    ui->checkBoxfiltrs->setChecked(false);
    ui->framefiltrs->setHidden(true);
    ui->report_Button->setEnabled(false);

}

void MainWindow::on_actionSpectable_triggered()
{
    ui->checkBoxfiltrs->setHidden(true);
    ui->checkBoxfiltrs->setChecked(false);
    ui->framefiltrs->setHidden(true);
    ui->report_Button->setEnabled(false);

}

void MainWindow::on_action_triggered()
{
    QSqlQuery query;

    MainWindowGrafik *wg = new MainWindowGrafik();
    QSqlTableModel* model = new QSqlTableModel( this, db );

    QModelIndex id;
    QString cell;


    if(curanaliz == 2){

         int nbrows = ui->tableView->model()->rowCount();
         for(int i = 0; i < nbrows-1; ++i) {

             id = ui->tableView->model()->index(i, 1);
             wg->x << ui->tableView->model()->data(id,0).toString();

             id = ui->tableView->model()->index(i, 5);
             cell = ui->tableView->model()->data(id,0).toString().mid(0,2);
             wg->fossilData1.push_back( cell.toDouble());

             id = ui->tableView->model()->index(i, 6);
             cell = ui->tableView->model()->data(id,0).toString().mid(0,2);
             qDebug() << cell;
             wg->fossilData2.push_back(  cell.toDouble());

         }


    }else if(curanaliz == 4){
        int nbrows = ui->tableView->model()->rowCount();
        for(int i = 0; i < nbrows-1; ++i) {

            id = ui->tableView->model()->index(i, 0);
            wg->x << ui->tableView->model()->data(id,0).toString();

            id = ui->tableView->model()->index(i, 3);
            cell = ui->tableView->model()->data(id,0).toString().mid(0,2);
            wg->fossilData1.push_back( cell.toDouble());

            id = ui->tableView->model()->index(i, 5);
            cell = ui->tableView->model()->data(id,0).toString().mid(0,2);
            qDebug() << cell;
            wg->fossilData2.push_back(  cell.toDouble());
    }
}



    wg->setupPlot();




    wg->showMaximized();




}
