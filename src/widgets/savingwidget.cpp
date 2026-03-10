#include "widgets/savingwidget.h"
#include "views/mainwindow.h"
#include "./ui_savingwidget.h"
#include <time.h>
#include <QDateTime>
#include <QCoreApplication>

extern QString user_id;
static QLocale locale(QLocale::German);

SavingView::SavingView(QWidget *parent) 
    : QWidget(parent),
      ui(new Ui::SavingView) {

    ui->setupUi(this);
    this->setFixedSize(1080, 650);

    transactionsModel = DatabaseManager::instance().getTransactionsModel(this);

    ui->titleLabel->setStyleSheet("font-size: 16pt");
    
    connect(ui->backButton, &QPushButton::clicked, this, &SavingView::backButtonWasPressed);
    
    initView();
}

SavingView::~SavingView() {
    delete ui;
}

void SavingView::backButtonWasPressed(){
    emit backbutton_was_pressed();
}

void SavingView::updateSummary(){
    int totalSavings = 0;
    for(int i_month = 0; i_month < numberOfMonthInAYear; i_month++){
        qDebug() <<"Saving month: "<< i_month<<" "<<moneyByMonth[i_month].totals.savings;
        totalSavings += moneyByMonth[i_month].totals.savings;
    }
       
    if(totalSavings < 0){
        qDebug()<<"total Saving: "<< totalSavings;
        ui->labelTotalAmount->setText("0");
    }else{
        ui->labelTotalAmount->setText(locale().toString(totalSavings));
    }

    ui->labelAverageAmount->setText(locale().toString(totalSavings / numberOfMonthInAYear));

    auto compareBySaving = [](const Ui::MONEY&  a, const Ui::MONEY& b) {
        return a.totals.savings < b.totals.savings;
    };

    auto it_max = std::max_element(moneyByMonth.begin(), moneyByMonth.end(), compareBySaving);
    ui->labelHigherAmount->setText(QString::number((it_max->totals.savings < 0) ? 0 : (it_max->totals.savings)));

    auto it_min = std::min_element(moneyByMonth.begin(), moneyByMonth.end(), compareBySaving);
    ui->labelLowerAmount->setText(QString::number((it_min->totals.savings < 0) ? 0 : (it_min->totals.savings)));

    int total_incomes = 0;
    for(const auto &money: moneyByMonth){
        total_incomes+=money.totals.incomes;
    }

    double ratio = ((double)(totalSavings) / std::abs(total_incomes)) * 100.0;
    ui->labelRatioAmount->setText(locale().toString(ratio, 'f', 1) + "%");

    if(ratio < 0){
        ui->labelRatioAmount->setStyleSheet("color: red;");
    }else if(ratio > 0 && ratio < 20){
        ui->labelRatioAmount->setStyleSheet("color: yellow;");
    }else{
        ui->labelRatioAmount->setStyleSheet("color: green;");
    }
 
}

void SavingView::updateBarGraph(){
    QBarSet *savingsSet = new QBarSet("Savings");
    //QBarSet *expensesSet = new QBarSet("Expenses");
    QStringList monthsList;

    for (auto it = moneyByMonth.rbegin(); it != moneyByMonth.rend(); ++it) {
        *savingsSet << it->totals.savings;
        //*expensesSet << it->expenses;
        monthsList << QDate::fromString(it->date, "yyyy-MM-dd").toString("MM-yy");
    }

    QBarSeries *series = new QBarSeries();
    series->append(savingsSet);
    //series->append(expensesSet);

    QChart *chart = new QChart();
    savingsSet->setBrush(QBrush(QColor("#a0eb74cc")));
    chart->addSeries(series);
    chart->setTitle("Monthly Savings");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(monthsList);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    chart->setTheme(QChart::ChartThemeDark);

    QValueAxis *axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(false);
    chart->legend()->setAlignment(Qt::AlignBottom);

    ui->chartView->setChart(chart);
    ui->chartView->setRenderHint(QPainter::Antialiasing);

    connect(series, &QBarSeries::doubleClicked, this, [this, axisX](int index, QBarSet *) {
        QDate month = QDate::fromString("01-" + axisX->at(index), "dd-MM-yy", 2000);
        emit monthSelected(month);
    });

}


void SavingView::initView() {
    
    QString fechaFormateada = QString("Current Date/Time:\n%1").arg(QDateTime::currentDateTime().toString("dd-MM-yyyy  HH:mm"));
    ui->labelCurrentTime->setText(fechaFormateada);

    moneyByMonth.resize(numberOfMonthInAYear);

    QDate initDate = QDate::currentDate();

    for(int i_month = 0; i_month < 12; i_month++){
        MonthlyReportService service(initDate, user_id);

        QString date = QDate(initDate.year(), initDate.month(), 1).toString("yyyy-MM-dd");
        
        transactionsModel->select();

        moneyByMonth[i_month].totals = service.getComputeTotals(service.getAmountByCategory());
        moneyByMonth[i_month].date = date;

        initDate = initDate.addMonths(-1);
    }

    updateSummary();
    updateBarGraph();
}
