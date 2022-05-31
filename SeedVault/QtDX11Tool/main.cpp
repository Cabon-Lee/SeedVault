#include "pch.h"
#include "QMain.h"
#include <QtWidgets/QApplication>

QMain* g_MainWindow;
QApplication* g_App;

int main(int argc, char *argv[])
{
    g_App = new QApplication(argc, argv);
    g_MainWindow = new QMain();
    g_App->setStyle("Fusion");

    g_MainWindow->show();
    return g_App->exec();
}
