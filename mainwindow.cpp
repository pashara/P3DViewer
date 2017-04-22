#include "mainwindow.h"
#include "newwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QStringList>
#include "includes.h"
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    ui->addModelButton->setToolTip("Добавляет модель в проект");
    connect(ui->addModelButton, SIGNAL (released()),this, SLOT (loadModel_ButtonHandler()));

    connect(ui->runButton, SIGNAL (released()),this, SLOT (startLoad_ButtonHandler()));

    ui->progressBar->setVisible(false);
	//this->startLoad_ButtonHandler();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startLoad_ButtonHandler()
{
	//ui->filesTable->model()->rowCount();
	//if (data->size() <= 0) {
	/*if (ui->filesTable->model()->rowCount() <= 0) {
		QMessageBox Msgbox;
		Msgbox.setText("Но вы же не выбрали ни одного файла....");
		Msgbox.exec();
		return;
	} else {
		QMessageBox Msgbox;
		int asds = ui->filesTable->rowCount();
		Msgbox.setText(QString(asds));
		Msgbox.exec();
	}*/


	ui->progressBar->setVisible(true);
	ui->progressBar->setValue(0);

	for (int j = 0; j < 100; j += 2) {
		ui->progressBar->setValue(j);
	}






    this->hide();


	objectfile insertedFile("c://123//asd.3ds", "Mamonth");
	data->insert(data->end(), insertedFile);



    NewWindow* newWindow = new NewWindow(data); // Be sure to destroy you window somewhere
    newWindow->show();
}

void MainWindow::loadModel_ButtonHandler()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Загрузка 3Ds файлов"), "",
    tr("3DS files (*.3ds)"));

    if (fileName != "") {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            // error message
        } else {
            path a(fileName.toUtf8().constData());

			objectfile insertedFile(fileName.toUtf8().constData(), a.stem().string().c_str());
			data->insert(data->end(), insertedFile);


            cout<<a.stem()<<endl;
            cout<<a.extension().string()<<endl;

            ui->filesTable->setRowCount(ui->filesTable->rowCount() + 1);
            ui->filesTable->setColumnCount(2);
            ui->filesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
            ui->filesTable->setSelectionMode(QAbstractItemView::NoSelection);


            ui->filesTable->setItem(ui->filesTable->rowCount()-1,0,new QTableWidgetItem(a.stem().string().c_str()));
            ui->filesTable->setItem(ui->filesTable->rowCount()-1,1,new QTableWidgetItem(a.extension().string().c_str()));

            QStringList m_TableHeader;
            m_TableHeader <<"#"<<"Name";
            ui->filesTable->setHorizontalHeaderLabels(m_TableHeader);
            file.close();
			
			//ui->addModelButton->setText(ui->filesTable->model()->data(ui->filesTable->model()->index(0, 0)).toString());
			//cout << ui->filesTable->model()->data(ui->filesTable->model()->index(0, 2)).toString().toStdString() << endl;
        }
    }
}
