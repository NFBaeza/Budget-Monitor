#include "widgets/dashboardwidget.h"
#include "views/mainwindow.h"
#include "./ui_dashboardwidget.h"
#include "dialogs/formdialog.h"
#include "dialogs/categorydialog.h"
#include "dialogs/accountdialog.h"
#include "dialogs/addingfiledialog.h"
#include <QMessageBox>

extern QString user_id;
static QLocale s_locale(QLocale::German);

static int countUserRows(const QString &table) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    if (!query.prepare(QString("SELECT COUNT(*) FROM %1 WHERE user_id = :user").arg(table))) {
        qDebug() << "[countUserRows] PREPARE ERROR:" << query.lastError().text();
        return 0;
    }
    query.bindValue(":user", user_id);
    if (!query.exec() || !query.next()) {
        qDebug() << "[countUserRows] EXEC ERROR:" << query.lastError().text();
        return 0;
    }
    int n = query.value(0).toInt();
    query.finish();
    return n;
}

MonthView::MonthView(QDate dateSelected, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MonthView),
    ReportService(dateSelected, user_id),
    dateViewSelected(dateSelected)
{
    ui->setupUi(this);
    this->setMinimumSize(1080, 650);

    monthName = QLocale().monthName(dateViewSelected.month());
    monthName[0] = monthName[0].toUpper();
    ui->MonthNameLabel->setText(QString("%1 %2").arg(monthName).arg(dateViewSelected.year()));

    transactionModel = DatabaseManager::instance().getTransactionsModel(this);
    
    connect(ui->checkCreditCardButton, &QPushButton::clicked, this, [this](){
        emit creditcardButtonWasPressed();
    });
    connect(ui->BackButton, &QPushButton::clicked, this, [this](){
        emit backButtonWasPressed();
    });
    connect(ui->AddEntryButton, &QPushButton::clicked, this, &MonthView::onAddButtonClicked);
    connect(ui->TableViewLastEntry, &QTableView::doubleClicked, this, &MonthView::onTableRowDoubleClicked);
    connect(ui->EditCategoryButton, &QPushButton::clicked, this, &MonthView::onEditButtonClicked);
    connect(ui->EditAccountButton, &QPushButton::clicked, this, [this](){
        AccountManager dialog(this);
        connect(&dialog, &AccountManager::dataUpdated, this, [this]() {
            updateView();
        });
        dialog.exec();
    });
    connect(ui->addFileButton, &QPushButton::clicked, this, &MonthView::onAddFileButtonClicked);

    initView();
}

MonthView::~MonthView() {
    delete ui;
}

void MonthView::onAddButtonClicked() {
    if (countUserRows("categories") == 0 || countUserRows("accounts") == 0) {
        QMessageBox::warning(this, "Cannot add entry",
            "Please create at least one category and one account before adding entries.");
        return;
    }

    FormDialog dialog(this);

    connect(&dialog, &FormDialog::dataInserted, this, [this]() {
        updateView();
    });

    dialog.exec();
}

void MonthView::onAddFileButtonClicked() {
    if (countUserRows("accounts") == 0) {
        QMessageBox::warning(this, "Cannot import file",
            "Please add at least one account before importing transactions.");
        return;
    }

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


void MonthView::updateView(){
    transactionModel->setFilter(monthFilter);
    updateTransactions();

    amountByCategoryMap = ReportService.getAmountByCategory();

    updateLabelsFromFilter("Income");
    updateLabelsFromFilter("Expense");

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
    QMap<QString, int> subCategoryAmount;
    for(const auto& [category, amount] : amountByCategoryMap.asKeyValueRange()){
        if((category.toLower() == "tranfer") && (category.toLower() == "investment")){
            continue;
        }
        subCategoryAmount[category] = amount;
    }
    totals = ReportService.getComputeTotals(subCategoryAmount);
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
    ui->DateNowLabel->setText(QString("Current time: %1").arg(currentDateTime));

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

    for (const QString &prefix : {QString("Income"), QString("Expense")}) {
        for (int i = 1; i <= 6; ++i) {
            QString name = QString("%1%2").arg(prefix).arg(i);
            m_labelCache[name]            = findChild<QLabel*>(name);
            m_labelCache[name + "Amount"] = findChild<QLabel*>(name + "Amount");
        }
    }

    updateView();
}