#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QFile>
#include "includes.h"
#include "objectfile.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void load();

private:
    Ui::MainWindow *ui;
    int objectsInScene = 0;
	std::vector<objectfile>* data = new std::vector<objectfile>();
private slots:
    void loadModel_ButtonHandler();
    void startLoad_ButtonHandler();
};
