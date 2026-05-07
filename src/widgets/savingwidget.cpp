#include "widgets/savingwidget.h"
#include "views/mainwindow.h"
#include "./ui_savingwidget.h"
#include <QDateTime>

extern QString user_id;
static QLocale s_locale(QLocale::German);

SavingView::SavingView(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::SavingView) {

    ui->setupUi(this);
    setMinimumSize(1080, 650);

    ui->titleLabel->setStyleSheet("font-size: 16pt");

    connect(ui->backButton, &QPushButton::clicked, this, &SavingView::backButtonWasPressed);

    auto connectMonthButton = [this](QRadioButton *btn, int months) {
        connect(btn, &QRadioButton::clicked, this, [this, months]() {
            displayMonths = months;
            refreshView();
        });
    };

    connectMonthButton(ui->threeMonthsViewRadioButton, 3);
    connectMonthButton(ui->sixMonthsViewRadioButton,   6);
    connectMonthButton(ui->nineMonthsViewRadioButton,  9);
    connectMonthButton(ui->twelveMonthsViewRadioButton, 12);

    precomputeData();
    refreshView();
}

SavingView::~SavingView() {
    delete ui;
}

void SavingView::backButtonWasPressed(){
    emit backbutton_was_pressed();
}

void SavingView::precomputeData(){
    m_monthData.resize(MAX_MONTHS);

    QDate currentDate = QDate::currentDate();
    MonthlyReportService service(currentDate, user_id);

    for (int i = 0; i < MAX_MONTHS; i++) {
        QDate monthDate = currentDate.addMonths(-i);
        QDate firstOfMonth(monthDate.year(), monthDate.month(), 1);

        m_monthData[i].date   = firstOfMonth;
        m_monthData[i].totals = service.getComputeTotals(service.getAmountByCategoryAndTypeOfCard("debit",firstOfMonth));
        for(const auto [creditCard, totals] : service.getTotalsByTypeCard("credit").asKeyValueRange()){
            m_monthData[i].totals.expenses += totals.expenses;
        };
    }
}

void SavingView::refreshView(){
    QString fechaFormateada = QString("Current Date/Time:\n%1")
        .arg(QDateTime::currentDateTime().toString("dd-MM-yyyy  HH:mm"));
    ui->labelCurrentTime->setText(fechaFormateada);

    updateSummary();
    updateBarGraph();
}

void SavingView::updateSummary(){
    int totalSavings = 0;
    int totalIncomes = 0;
    int totalInvestment = 0;

    for (int i = 0; i < displayMonths; i++) {
        totalSavings += m_monthData[i].totals.savings;
        totalIncomes += m_monthData[i].totals.incomes;
    }

    for (int i = 0; i < MAX_MONTHS; i++) {
        totalInvestment += m_monthData[i].totals.investment;
    }

    ui->labelInvestmentAmount->setText(s_locale.toString(totalInvestment));
    ui->labelTotalAmount->setText(totalSavings < 0 ? "0" : s_locale.toString(totalSavings));
    ui->labelAverageAmount->setText(s_locale.toString(totalSavings / displayMonths));

    auto compareBySaving = [](const MonthData &a, const MonthData &b) {
        return a.totals.savings < b.totals.savings;
    };

    auto begin = m_monthData.begin();
    auto end   = m_monthData.begin() + displayMonths;

    auto it_max = std::max_element(begin, end, compareBySaving);
    ui->labelHigherAmount->setText(s_locale.toString(
        (it_max->totals.savings < 0) ? 0.0 : it_max->totals.savings, 'f', 0));

    auto it_min = std::min_element(begin, end, compareBySaving);
    ui->labelLowerAmount->setText(s_locale.toString(
        (it_min->totals.savings < 0) ? 0.0 : it_min->totals.savings, 'f', 0));

    if (totalIncomes != 0) {
        double ratio = ((double)totalSavings / std::abs(totalIncomes)) * 100.0;
        ui->labelRatioAmount->setText(s_locale.toString(ratio, 'f', 1) + "%");

        if (ratio < 0) {
            ui->labelRatioAmount->setStyleSheet("color: red;");
        } else if (ratio < 20) {
            ui->labelRatioAmount->setStyleSheet("color: yellow;");
        } else {
            ui->labelRatioAmount->setStyleSheet("color: green;");
        }
    } else {
        ui->labelRatioAmount->setText("0%");
        ui->labelRatioAmount->setStyleSheet("color: red;");
    }
}

void SavingView::updateBarGraph(){
    QBarSet *savingsSet = new QBarSet("Savings");
    QStringList monthsList;

    for (int i = displayMonths - 1; i >= 0; i--) {
        *savingsSet << m_monthData[i].totals.savings;
        monthsList << m_monthData[i].date.toString("MM-yy");
    }

    QBarSeries *series = new QBarSeries();
    series->append(savingsSet);

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

    connect(series, &QBarSeries::doubleClicked, this, [this](int index, QBarSet *) {
        int dataIndex = displayMonths - 1 - index;
        emit monthSelected(m_monthData[dataIndex].date);
    });
}
