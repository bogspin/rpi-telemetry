#include "addbroker.h"
#include "src_autogen/include/ui_addbroker.h"

AddBroker::AddBroker(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::AddBroker)
{
    ui->setupUi(this);
}
