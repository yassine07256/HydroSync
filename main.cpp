#include <QApplication>
#include "equipmentmanagement.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    EquipmentManagement window;
    window.resize(1200, 700);
    window.show();

    return a.exec();
}