/***************************************************************************
**                                                                        **
**  QCustomPlot, an easy to use, modern plotting widget for Qt            **
**  Copyright (C) 2011-2015 Emanuel Eichhammer                            **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Emanuel Eichhammer                                   **
**  Website/Contact: http://www.qcustomplot.com/                          **
**             Date: 22.12.15                                             **
**          Version: 1.3.2                                                **
****************************************************************************/

#ifndef MAINWINDOWGRAFIK_H
#define MAINWINDOWGRAFIK_H

#include <QMainWindow>
#include <QTextDocument>
#include <QFileDialog>
#include "qcpdocumentobject.h"

namespace Ui {
class MainWindowGrafik;
}

class MainWindowGrafik : public QMainWindow
{
  Q_OBJECT
  
public:
  explicit MainWindowGrafik(QWidget *parent = 0);
  ~MainWindowGrafik();

  QCPBars *fossil, *fossil2;
  
  void setupPlot();

  void setosi();

  QVector<double> fossilData1, fossilData2;

  QStringList x, y1, y2;

  
private slots:
  void on_actionInsert_Plot_triggered();
  void on_actionSave_Document_triggered();
  
private:
  Ui::MainWindowGrafik *ui;
};

#endif // MAINWINDOWGRAFIK_H
