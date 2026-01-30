#include "dialogs/monthselectordialog.h"
#include "./ui_monthselectordialog.h"

MonthSelectorDialog::MonthSelectorDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MonthSelectorDialog)
{
    ui->setupUi(this);
    
    QDate initDate = QDate::currentDate();
    initView(initDate);
}

MonthSelectorDialog::~MonthSelectorDialog() {
    delete ui;
}

void MonthSelectorDialog::done(int result) {
    emit close_was_pressed();
}


void MonthSelectorDialog::onButtonClicked(QDate date) {
    emit month_clicked(date);
}


void MonthSelectorDialog::initView(QDate date) {
    for(int id = 12; id > 0; id--){
        QString firstDate = QDate(date.year(), date.month(), 1).toString("yyyy-MM-dd");
        QString month_name = QLocale().monthName(date.month());
        QString buttonName = QString("pushButton_%1").arg(id);
        QPushButton *buttonObj = this->findChild<QPushButton*>(buttonName);
        if (!buttonObj) {
            qDebug() << "[initView] Button not found:" << buttonName;
            continue;
        }
        buttonObj->setText(QString("%1 %2").arg(month_name).arg(date.year()));
        connect(buttonObj, &QPushButton::clicked, this, [this, date]() {
            onButtonClicked(date);
        });        
        date = date.addMonths(-1);
    }
}