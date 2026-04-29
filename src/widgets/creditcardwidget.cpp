#include "widgets/creditcardwidget.h"
#include "widgets/dashboardwidget.h"
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
    

    if(service.getCreditCardNumber() < 1){
        QMessageBox::warning(this, "Error", "User doesn't Have any Credit Account");
        QTimer::singleShot(0, this, [this]() { emit backToMain(); });
    }

    this->setMinimumSize(750, 600);

    accountsModel = DatabaseManager::instance().getAccountModel(nullptr);
    QString creditAccountFilter = QString("accounts.type = '%1' AND accounts.user_id = '%2'").arg("credit").arg(user_id);
    accountsModel->setFilter(creditAccountFilter);
    accountsModel->select();

    ui->selectBankComboBox->setModel(accountsModel);
    ui->selectBankComboBox->setModelColumn(2);

    connect(ui->backToMainButton, &QPushButton::clicked, this,[this](){emit backToMain();});
    connect(ui->goToCurrentMonthButton, &QPushButton::clicked, this, [this](){
        emit goToCurrentMonth();
    });
    connect(ui->selectBankComboBox, &QComboBox::currentIndexChanged, this, &CreditCardView::updateViewByComboBox);

    initView();
}

CreditCardView::~CreditCardView() {
    delete ui;
}


void CreditCardView::updateViewByComboBox(){
    updateSummary();
    updatePieChart();

}

void CreditCardView::updatePieChart() {
    auto *series = new QPieSeries();
    QList<MonthlyReportService::CreditSummary> summaryCreditCard = service.getAllCreditSummaries();
    int accountRow = ui->selectBankComboBox->currentIndex();
    QString accountName = accountsModel->index(accountRow, 2).data().toString();

    for (const auto& cardDetails : summaryCreditCard) {
        if(cardDetails.bankName == accountName){
            series->append("Used", cardDetails.used);
            QPieSlice *slice = series->append("Available", cardDetails.limit - cardDetails.used);
            slice->setBrush(Qt::lightGray);
        }
    }
    
    auto *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Usage of Credit Card");
    chart->legend()->hide();
    chart->setAnimationOptions(QChart::SeriesAnimations);

    for (QPieSlice *slice : series->slices()) {
        double percentage = 100 * slice->percentage();
        slice->setLabel(QString("%1: %2%").arg(slice->label()).arg(percentage, 0, 'f', 1));
        slice->setLabelVisible(true);
    }

    ui->pieChartView->setRenderHint(QPainter::Antialiasing);

    QChart *oldChart = ui->pieChartView->chart();
    ui->pieChartView->setChart(chart);
    if (oldChart) {
        delete oldChart;
    }
}
void CreditCardView::updateSummary(){
    auto cards = service.getAllCreditSummaries();
    int accountRow = ui->selectBankComboBox->currentIndex();
    QString accountName = accountsModel->index(accountRow, 2).data().toString();
    for(const auto card : cards){
        if(card.bankName == accountName){
            ui->usedCreditLabel->setText(QString::number(card.used));
            ui->availableCreditLabel->setText(QString::number(card.available));
            ui->limitCreditLabel->setText(QString::number(card.limit));
        }
    }
}



void CreditCardView::initView() {
    int maxMonthsView = 6;
    QDate date = initDate;

    ui->tableAmountView->setColumnCount(maxMonthsView);
    for (int col = 0; col < maxMonthsView; col++) {
        ui->tableAmountView->setHorizontalHeaderItem(col, new QTableWidgetItem(date.toString("MM/yy")));
        date = date.addMonths(1);
    }

    date = initDate;
    updateSummary();
    for (int col = 0; col < maxMonthsView; col++) {
        auto data = service.getTotalsByTypeCard("credit", date);
        int row = 0;
        for (const auto &[card, totals] : data.asKeyValueRange()) {
            if (ui->tableAmountView->rowCount() <= row){
                ui->tableAmountView->insertRow(row);
            }   
            ui->tableAmountView->setVerticalHeaderItem(row, new QTableWidgetItem(card));
            ui->tableAmountView->setItem(row, col, new QTableWidgetItem(QString::number(totals.expenses)));
            row++;
        }
        date = date.addMonths(1);
    }
    ui->tableAmountView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    updatePieChart();
}
