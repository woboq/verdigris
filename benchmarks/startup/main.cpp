#include <QObject>

int main()
{
    QObject o;
    return 1;
}

/*
 clang++ main.cpp -I/usr/include/qt4 -I/usr/include/qt4/QtCore -lQtCore -lQtGui -std=c++14 -O2 -o qt4
 clang++ main.cpp -I $CS_HOME/include -I $CS_HOME/include/QtCore -L $CS_HOME/lib -lCsCore1 -lCsGui1 -std=c++14 -O2 -o cs
 clang++ main.cpp -I/usr/include/qt -I/usr/include/qt/QtCore -lQt5Core -lQt5Gui -lQt5Widgets -std=c++14 -O2 -fPIC -o qt5
*/
