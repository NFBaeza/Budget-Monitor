#include "dialogs/loginsignindialog.h"
#include "database/databaseworker.h"
#include "./ui_loginsignindialog.h"

extern QString user_id;

LogInSignInDialog::LogInSignInDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LogInSignInDialog)
{
    ui->setupUi(this);


}


LogInSignInDialog::~LogInSignInDialog() {
    delete ui;
}