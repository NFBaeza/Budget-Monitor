#include "widgets/creditcardwidget.h"
#include "reportservices/monthlyreportservice.h"
#include "views/mainwindow.h"
#include "./ui_creditcardwidget.h"
#include <QTimer>

extern QString user_id;
static QLocale s_locale(QLocale::German);

CreditCardView::CreditCardView(QDate date, QWidget *parent) :
    QWidget(parent),
    initDate(date),
    service(date, user_id),
    ui(new Ui::CreditCardWidget)
{
    ui->setupUi(this);

    accountsModel = DatabaseManager::instance().getAccountModel(this);
    accountsModel->setFilter(QString("accounts.type = 'credit' AND user_id = '%1'").arg(user_id));
    accountsModel->select();

    qDebug()<<"number of credit accounts: "<<accountsModel->rowCount();
    if(accountsModel->rowCount() < 1){
        QMessageBox::warning(this, "Error", "User doesn't Have any Credit Account");
        QTimer::singleShot(0, this, [this]() { emit backToMain(); });
    }

    this->setMinimumSize(1080, 650);
    connect(ui->backToMainButton, &QPushButton::clicked, this,[this](){emit backToMain();});

    initView();
}

CreditCardView::~CreditCardView() {
    delete ui;
}

void CreditCardView::initView(){
    int maxMonthsView = 6;
    QString prefixMonthLabel = "monthLabel";

    for(int month_i = 1; month_i <= maxMonthsView; month_i++){
        QString monthLabel = QString("%1_%2").arg(prefixMonthLabel).arg(month_i);
        QLabel *label = findChild<QLabel*>(monthLabel);
        if (label){
            label->setText(initDate.toString("MM/yy"));
            initDate = initDate.addMonths(1);
        }
            service.getAmountByTypeCard("credit");
    }
}