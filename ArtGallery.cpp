#include "MainWindow.h"

int LaunchMainWindow(int& argc, char* argv[])
{
    QApplication app(argc, argv);
    auto win = new MainWindow;
    win->show();
    return QApplication::exec();
}

int main(int argc, char* argv[])
{
   LaunchMainWindow(argc, argv);
}
