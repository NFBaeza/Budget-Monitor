#include "widgets/dashboardwidget.h"
#include "views/mainwindow.h"
#include "./ui_dashboardwidget.h"
#include "dialogs/formdialog.h"
#include "dialogs/categorydialog.h"
#include "dialogs/addingfiledialog.h"

extern QString user_id;
static QLocale s_locale(QLocale::German);

MonthView::MonthView(QDate dateSelected, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MonthView),
    ReportService(dateSelected, user_id),
    dateViewSelected(dateSelected)
{
    ui->setupUi(this);
    this->setMinimumSize(1080, 650);
    ui->typeAccountView->setCurrentIndex(0);

    monthName = QLocale().monthName(dateViewSelected.month());
    monthName[0] = monthName[0].toUpper();
    ui->MonthNameLabel->setText(QString("%1 %2").arg(monthName).arg(dateViewSelected.year()));

    transactionModel = DatabaseManager::instance().getTransactionsModel(this);
    
    connect(ui->BackButton, &QPushButton::clicked, this, &MonthView::backButtonWasPressed);
    connect(ui->AddEntryButton, &QPushButton::clicked, this, &MonthView::onAddButtonClicked);
    connect(ui->TableViewLastEntry, &QTableView::doubleClicked, this, &MonthView::onTableRowDoubleClicked);
    connect(ui->EditCategoryButton, &QPushButton::clicked, this, &MonthView::onEditButtonClicked);
    connect(ui->addFileButton, &QPushButton::clicked, this, &MonthView::onAddFileButtonClicked);

    initView();
}

MonthView::~MonthView() {
    delete ui;
}

void MonthView::backButtonWasPressed(){
    emit backbutton_was_pressed();

}

void MonthView::onAddButtonClicked() {
    FormDialog dialog(this);

    connect(&dialog, &FormDialog::dataInserted, this, [this]() {
        updateView();
    });

    dialog.exec();
}

void MonthView::onAddFileButtonClicked() {
    AddingFileDialog dialog(this);

    connect(&dialog, &AddingFileDialog::dataInserted, this, [this]() {
        updateView();
    });

    dialog.exec();
}

void MonthView::onEditButtonClicked() {
    CategoryDialog dialog(this);

    connect(&dialog, &CategoryDialog::dataUpdated, this, [this]() {
        updateView();
    });

    dialog.exec();
}

void MonthView::onTableRowDoubleClicked(const QModelIndex &index) {
    int row = index.row();

    int transactionId = transactionModel->data(
        transactionModel->index(row, 0)).toInt();

    FormDialog dialog(transactionId, this);

    connect(&dialog, &FormDialog::dataDeleted, this, [this]() {
        updateView();
    });

    connect(&dialog, &FormDialog::dataInserted, this, [this]() {
        updateView();
    });

     connect(&dialog, &FormDialog::dataUpdated, this, [this]() {
        updateView();
    });

    dialog.exec();
}

void  MonthView::updateTransactions(){
    ui->TableViewLastEntry->resizeColumnsToContents();
    ui->TableViewLastEntry->horizontalHeader()->setStretchLastSection(true);
    ui->TableViewLastEntry->setColumnHidden(0,true);
    ui->TableViewLastEntry->setColumnHidden(1,true);
    
    ui->TableViewLastEntry->setColumnHidden(7,true);
    ui->TableViewLastEntry->setColumnHidden(8,true);
    ui->TableViewLastEntry->setColumnHidden(9,true);
}

void MonthView::updateCreditReview(){
    QList creditCardSumary = ReportService.getCreditSummaries(numberOfCreditCards);
    for(int i = 0; i < creditCardSumary.size(); i++){
        m_labels[i*numberOfLabelPerCreditCard]->setTextFormat(Qt::MarkdownText);
        m_labels[i*numberOfLabelPerCreditCard]->setText(QString("**%1**").arg(creditCardSumary[i].bankName));
        m_labels[i*numberOfLabelPerCreditCard+2]->setText(QString::number(creditCardSumary[i].usedCredit));
        m_labels[i*numberOfLabelPerCreditCard+4]->setText(QString::number(creditCardSumary[i].availableCredit));
        m_labels[i*numberOfLabelPerCreditCard+6]->setText(QString::number(creditCardSumary[i].limitCredit));

        for(int posY = 2; posY < numberOfLabelPerCreditCard; posY+=2){
            m_labels[i*numberOfLabelPerCreditCard+posY]->setAlignment(Qt::AlignRight);
        }
    }
}

void MonthView::updateView(){
    transactionModel->setFilter(monthFilter);
    updateTransactions();

    amountByCategoryMap = ReportService.getAmountByCategory();

    updateLabelsFromFilter("Income");
    updateLabelsFromFilter("Expense");

    updateCreditReview();

    updateSummary();
    updatePieChart();

    this->update();
}



QPair<QDate, QDate> MonthView::monthDateRange() const {
    return {
        QDate(dateViewSelected.year(), dateViewSelected.month(), 1),
        QDate(dateViewSelected.year(), dateViewSelected.month(), dateViewSelected.daysInMonth())
    };
}

void MonthView::updateLabelsFromFilter(const QString type) {
    const int maxLabels = 6;

    // Collect categories from the model with their amounts
    QList<QPair<QString, int>> sorted;
    for (const auto& [category, amount] : amountByCategoryMap.asKeyValueRange()) {
        if(ReportService.getCategoryType(category) == type.toLower()){
            QString categoryUpper = category;
            categoryUpper[0] = categoryUpper[0].toUpper();
            sorted.append({categoryUpper, amount});
        }    
    }

    std::sort(sorted.begin(), sorted.end(), [](const QPair<QString, int> &a, const QPair<QString, int> &b) {
        return a.second > b.second;
    });

    for (int i = 0; i < maxLabels; ++i) {
        QString objectName = QString("%1%2").arg(type).arg(i + 1);
        QLabel *nameLabel   = m_labelCache.value(objectName, nullptr);
        QLabel *amountLabel = m_labelCache.value(objectName + "Amount", nullptr);

        if (i < sorted.size()) {
            if (nameLabel) {
                nameLabel->setText(sorted[i].first);
                nameLabel->setVisible(true);
            }
            if (amountLabel) {
                amountLabel->setText(s_locale.toString(sorted[i].second));
                amountLabel->setVisible(true);
            }
        } else {
            if (nameLabel) {
                nameLabel->setText("");
                nameLabel->setVisible(false);
            }
            if (amountLabel) {
                amountLabel->setText("0");
                amountLabel->setVisible(false);
            }
        }
    }
}



void MonthView::updateSummary(){
    totals = ReportService.getComputeTotals(amountByCategoryMap);
    ui->TotalSavingsAmount->setText(s_locale.toString(totals.savings));
    ui->TotalIncomesAmount->setText(s_locale.toString(totals.incomes));
    ui->TotalExpensesAmount->setText(s_locale.toString(totals.expenses));
}

void MonthView::updatePieChart() {
    auto *series = new QPieSeries();
    for (const auto& [category, amount] : amountByCategoryMap.asKeyValueRange()) {
        if(ReportService.getCategoryType(category) == "expense"){
            series->append(category, amount);
        }
    }

    if (totals.incomes > totals.expenses) {
        QPieSlice *slice = series->append("Available balance", totals.savings);
        slice->setBrush(Qt::lightGray);
    }

    auto *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Money Distribution");
    chart->legend()->hide();
    chart->setAnimationOptions(QChart::SeriesAnimations);

    for (QPieSlice *slice : series->slices()) {
        if (slice->value() <= 0) {
            slice->setLabelVisible(false);
            slice->setExploded(false);
        } else {
            double percentage = 100 * slice->percentage();
            slice->setLabel(QString("%1: %2%").arg(slice->label()).arg(percentage, 0, 'f', 1));
            slice->setLabelVisible(true);
        }
    }

    ui->graphicsView->setRenderHint(QPainter::Antialiasing);

    QChart *oldChart = ui->graphicsView->chart();
    ui->graphicsView->setChart(chart);
    if (oldChart) {
        delete oldChart;
    }
}

void MonthView::initView(){
    QString currentDateTime = QDateTime::currentDateTime().toString("dd-MM-yyyy HH:mm");
    ui->DateNowLabel->setText(QString("Current time:\n %1").arg(currentDateTime));

    auto [firstDate, lastDate] = monthDateRange();
    monthFilter = QString("money_transactions.date >= '%1' AND money_transactions.date <= '%2' AND money_transactions.user_id = '%3'")
        .arg(firstDate.toString("yyyy-MM-dd"))
        .arg(lastDate.toString("yyyy-MM-dd"))
        .arg(user_id);

    transactionModel->setFilter(monthFilter);
    transactionModel->setHeaderData(4, Qt::Horizontal, "category");
    transactionModel->setHeaderData(5, Qt::Horizontal, "account");
    if(!transactionModel->select()){qDebug()<<"[updateTransactions] error:" << transactionModel->lastError().text();return;};
    
    if (ui->TableViewLastEntry->model() != transactionModel) {
        ui->TableViewLastEntry->setModel(transactionModel);
        ui->TableViewLastEntry->setItemDelegate(new QSqlRelationalDelegate(ui->TableViewLastEntry));
    }

    numberOfCreditCards = ReportService.getCreditCardNumber();

    m_labels.resize(numberOfCreditCards * numberOfLabelPerCreditCard);

    for (int newLabelNumber = 0; newLabelNumber < numberOfCreditCards*numberOfLabelPerCreditCard; newLabelNumber++) {
        m_labels[newLabelNumber] = new QLabel(this);
        m_labels[newLabelNumber]->setVisible(true);
        m_labels[newLabelNumber]->setText(labels[newLabelNumber % numberOfLabelPerCreditCard]);
        
        ui->gridLayout_2->addWidget(m_labels[newLabelNumber], newLabelNumber%numberOfLabelPerCreditCard, newLabelNumber/numberOfLabelPerCreditCard);
        
    }
    ui->gridLayout_2->setHorizontalSpacing(50);

    for (const QString &prefix : {QString("Income"), QString("Expense")}) {
        for (int i = 1; i <= 6; ++i) {
            QString name = QString("%1%2").arg(prefix).arg(i);
            m_labelCache[name]            = findChild<QLabel*>(name);
            m_labelCache[name + "Amount"] = findChild<QLabel*>(name + "Amount");
        }
    }

    updateView();
}