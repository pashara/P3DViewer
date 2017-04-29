
#include <QApplication>
#include "includes.h"
#include "mainwindow.h"
#include "3dloader.h"

#include <Windows.h>




int main(int argc, char *argv[])
{

	AllocConsole();
	FILE *pFileCon = NULL;
	pFileCon = freopen("CONOUT$", "w", stdout);

	COORD coordInfo;
	coordInfo.X = 130;
	coordInfo.Y = 9000;

	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coordInfo);
	SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ENABLE_QUICK_EDIT_MODE | ENABLE_EXTENDED_FLAGS);





    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
