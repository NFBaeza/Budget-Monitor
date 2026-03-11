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
    emit closeWasPressed();
}


void MonthSelectorDialog::onButtonClicked(QDate date) {
    emit monthClicked(date);
}


void MonthSelectorDialog::initView(QDate date) {
    int maxColGrid = 3;

    for(int monthNumber = 0; monthNumber < numberOfMonthInAYear; monthNumber++){
        
        QString month_name = QLocale().monthName(date.month());
       
        m_buttons[monthNumber] = new QPushButton(this);
        m_buttons[monthNumber]->setText(QString("%1 %2").arg(month_name).arg(date.year()));

        ui->gridLayout->addWidget(m_buttons[monthNumber], 1+(monthNumber/maxColGrid), (monthNumber%maxColGrid));

        connect(m_buttons[monthNumber], &QPushButton::clicked, this, [this, date]() {
            onButtonClicked(date);
        }); 
   
        date = date.addMonths(-1);
    }

    QLabel *titleDialog = new QLabel("Select a Month",this);
    ui->gridLayout->addWidget(titleDialog, 0, 0, 1, maxColGrid, Qt::AlignHCenter);
}