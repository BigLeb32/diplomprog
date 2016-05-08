#include "mainwindowgrafik.h"
#include "ui_mainwindowgrafik.h"

MainWindowGrafik::MainWindowGrafik(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindowGrafik)
{

  this->setWindowTitle("Uh");
  ui->setupUi(this);


  ui->plot->axisRect()->setMinimumSize(300, 180);
//  setupPlot();
  
  // register the plot document object (only needed once, no matter how many plots will be in the QTextDocument):
  QCPDocumentObject *plotObjectHandler = new QCPDocumentObject(this);
  ui->textEdit->document()->documentLayout()->registerHandler(QCPDocumentObject::PlotTextFormat, plotObjectHandler);

  ui->tabWidget->setCurrentIndex(0);
}

MainWindowGrafik::~MainWindowGrafik()
{
  delete ui;
}

void MainWindowGrafik::setupPlot()
{

  connect(ui->plot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->plot->xAxis2, SLOT(setRange(QCPRange)));
  connect(ui->plot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->plot->yAxis2, SLOT(setRange(QCPRange)));


  fossil = new QCPBars(ui->plot->xAxis, ui->plot->yAxis);
  fossil2 = new QCPBars(ui->plot->xAxis, ui->plot->yAxis);

  QCPBarsGroup *group = new QCPBarsGroup(ui->plot);
  group->append(fossil);
  group->append(fossil2);



  ui->plot->addPlottable(fossil);
  ui->plot->addPlottable(fossil2);

  // Установки цвета:
  QPen pen;
  pen.setWidthF(1.5);//Толщина контура столбца
  fossil->setName(QString::fromUtf8("Эффективность 1")); // Легенда
  pen.setColor(QColor(50, 50, 100));// Цвет контура столбца
  fossil->setPen(pen);
  // Цвет самого столбца, четвертый параметр - прозрачность
  fossil->setBrush(QColor(50, 50, 250, 70));

  fossil2->setName(QString::fromUtf8("Эффективность 2")); // Легенда
  pen.setColor(QColor(50, 50, 100));// Цвет контура столбца
  fossil2->setPen(pen);
  // Цвет самого столбца, четвертый параметр - прозрачность
  fossil2->setBrush(QColor(50, 250, 250, 70));

  fossil->setWidth(0.4);

  fossil2->setWidth(0.4);


  // Установки значений оси X:
  QVector<double> ticks;
  QVector<QString> labels;


  int i = 1;
  foreach (QString str, this->x) {

      ticks << i;
      labels << str;
      i++;
  }

  QFont font1( "Newyork", 9 );


  ui->plot->xAxis->setAutoTicks(false);
  ui->plot->xAxis->setAutoTickLabels(false);
  ui->plot->xAxis->setTickVector(ticks);
  ui->plot->xAxis->setTickVectorLabels(labels);
  ui->plot->xAxis->setTickLabelFont(font1);
  ui->plot->xAxis->setTickLabelRotation(-70);

  ui->plot->xAxis->setSubTickCount(0);
  ui->plot->xAxis->setTickLength(0, 4);
  ui->plot->xAxis->grid()->setVisible(true);
  ui->plot->xAxis->setAutoTickStep(true);

  // Установки значений оси Y:
  ui->plot->yAxis->setPadding(5);
  ui->plot->yAxis->setLabel(QString::fromUtf8("Эффективность %"));
  ui->plot->yAxis->grid()->setSubGridVisible(true);
  QPen gridPen;
  gridPen.setStyle(Qt::SolidLine);
  gridPen.setColor(QColor(0, 0, 0, 25));
  ui->plot->yAxis->grid()->setPen(gridPen);
  gridPen.setStyle(Qt::DotLine);
  ui->plot->yAxis->grid()->setSubGridPen(gridPen);


  fossil->setData(ticks, this->fossilData1);
  fossil2->setData(ticks, this->fossilData2);

  ui->plot->rescaleAxes(true);

  // Легенда:
  ui->plot->legend->setVisible(true);
 // ui->plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignHCenter);
  ui->plot->legend->setBrush(QColor(255, 255, 255, 200));
  QPen legendPen;
  legendPen.setColor(QColor(130, 130, 130, 200));
  ui->plot->legend->setBorderPen(legendPen);
  QFont legendFont = font();
  legendFont.setPointSize(10);
  ui->plot->legend->setFont(legendFont);
  ui->plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
  ui->plot->replot();
}

void MainWindowGrafik::on_actionInsert_Plot_triggered()
{
  QTextCursor cursor = ui->textEdit->textCursor();
  
  // insert the current plot at the cursor position. QCPDocumentObject::generatePlotFormat creates a
  // vectorized snapshot of the passed plot (with the specified width and height) which gets inserted
  // into the text document.

  cursor.insertText(QString(QChar::ObjectReplacementCharacter), QCPDocumentObject::generatePlotFormat(ui->plot, 0, 0));

  ui->textEdit->setTextCursor(cursor);
}

void MainWindowGrafik::on_actionSave_Document_triggered()
{
  QString fileName = QFileDialog::getSaveFileName(this, "Save document...", qApp->applicationDirPath(), "*.pdf");
  if (!fileName.isEmpty())
  {
    QPrinter printer;
    printer.setFullPage(true);
    printer.setPaperSize(QPrinter::A4);
    printer.setOrientation(QPrinter::Portrait);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    ui->textEdit->document()->print(&printer);

  }
}


void MainWindowGrafik::setosi()
{



}





