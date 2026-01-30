#include "widgets/dashboardwidget.h"
#include "views/mainwindow.h"
#include "./ui_dashboardwidget.h"
#include "dialogs/formdialog.h"
#include "dialogs/categorydialog.h"
#include "dialogs/addingfiledialog.h"
#include <time.h>
#include <QDateTime>
#include <QCoreApplication>


MonthView::MonthView(QDate date_selected, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MonthView),
    dateViewSelected(date_selected) {
    
    ui->setupUi(this);
    this->setMinimumSize(1080, 650);

    month_name_ = QLocale().monthName(dateViewSelected.month());
    month_name_[0] = month_name_[0].toUpper();
    ui->MonthNameLabel->setText(QString("%1 %2").arg(month_name_).arg(dateViewSelected.year()));

    categoryModel = DatabaseManager::instance().getCategoryModel(this);
    incomesModel= DatabaseManager::instance().getIncomeModel(this);
    expensesModel = DatabaseManager::instance().getExpenseModel(this);
    transactionModel = DatabaseManager::instance().getTransactionsModel(this);
    
    connect(ui->BackButton, &QPushButton::clicked, this, &MonthView::backButtonWasPressed);
    connect(ui->AddEntryButton, &QPushButton::clicked, this, &MonthView::onAddButtonClicked);
    connect(ui->TableViewLastEntry, &QTableView::doubleClicked, this, &MonthView::onTableRowDoubleClicked);
    connect(ui->EditCategoryButton, &QPushButton::clicked, this, &MonthView::onEditButtonCliked);
    connect(ui->addFileButton, &QPushButton::clicked, this, &MonthView::onAddFileButtonCliked);

    initView();
}

MonthView::~MonthView() {
    delete ui;
}

void MonthView::backButtonWasPressed(){
    emit backbutton_was_pressed();

}

void MonthView::onAddFileButtonCliked() {
    AddingFileDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {
        qDebug() << "[onAddFileButtonCliked] Adding File, refreshing...";
        
        updateAllViews();
    }
}

void MonthView::onEditButtonCliked() {
    CategoryDialog dialog(this);

    connect(&dialog, &CategoryDialog::dataUpdated, this, [this]() {
        qDebug() << "[OnEditButtonCliked] Categories Updated, refreshing...";
        updateCategories();
    });

    if (dialog.exec() == QDialog::Accepted) {
        qDebug() << "[OnEditButtonCliked] Categories edited, refreshing...";
        
        updateAllViews();
    }
}

void MonthView::onTableRowDoubleClicked(const QModelIndex &index) {
    int row = index.row();

    // Get transaction ID (hidden column 0)
    int transactionId = transactionModel->data(
        transactionModel->index(row, 0)).toInt();

    qDebug() << "Editing transaction ID:" << transactionId;

    FormDialog dialog(transactionId, this);

    connect(&dialog, &FormDialog::dataDeleted, this, [this]() {
        qDebug() << "[OnTableRowDoubleClicked] Transaction deleted, refreshing...";
        updateAllViews();
    });

    if (dialog.exec() == QDialog::Accepted) {
        qDebug() << "[onTableRowDoubleClicked] Transaction edited, refreshing...";
    updateAllViews();
    }
}

void  MonthView::updateTransactions(){
    transactionModel->setFilter(MonthFilter);
    if(!transactionModel->select()){return;};

    // Solo configurar el modelo y delegate una vez
    if (ui->TableViewLastEntry->model() != transactionModel) {
        ui->TableViewLastEntry->setModel(transactionModel);
        ui->TableViewLastEntry->setItemDelegate(new QSqlRelationalDelegate(ui->TableViewLastEntry));
    }

    ui->TableViewLastEntry->resizeColumnsToContents();
    ui->TableViewLastEntry->horizontalHeader()->setStretchLastSection(true);
    ui->TableViewLastEntry->setColumnHidden(0,true);
}

void  MonthView::updateCategories(){
    updateLabelsFromFilter(incomesModel, "Income");
    updateLabelsFromFilter(expensesModel, "Expense");
    updateAmountView(incomesModel, ui->IncomesLayout); 
    updateAmountView(expensesModel, ui->ExpensesLayout);
}


void MonthView::updateAllViews(){
    updateTransactions();
    amountByCategoryMap.clear();
    setAmountByCategory();
    updateLabelsFromFilter(incomesModel, "Income");
    updateLabelsFromFilter(expensesModel, "Expense");
    updateAmountView(incomesModel, ui->IncomesLayout); 
    updateAmountView(expensesModel, ui->ExpensesLayout);
    updateSummary();
    updatePieChart();

    this->update();
    this->repaint();
}
void MonthView::onAddButtonClicked() {
    FormDialog dialog(this);

    // Show dialog (blocks until closed)
    if (dialog.exec() == QDialog::Accepted) {
        qDebug() << "[onAddButtonClicked] Dialog accepted, refreshing views...";
        updateAllViews();
    }
}

void  MonthView::setAmountByCategory() {
    for (int i = 0; i < categoryModel->rowCount(); ++i) {
        QString category_idx  = categoryModel->index(i,0).data().toString();
        QString category_name = categoryModel->index(i,1).data().toString();
        if (!category_name.isEmpty()) {
            category_name[0] = category_name[0].toUpper();
        } 

        QString filter = QString("%1 AND money_transactions.category = '%2'").arg(MonthFilter).arg(category_idx);
        transactionModel->setFilter(filter);

        if(transactionModel->select()){
            for(int j = 0; j < transactionModel->rowCount(); j++){
                int amount = transactionModel->data(transactionModel->index(j, 2)).toInt();
                amountByCategoryMap[category_name.toLower()] += amount; 
            }
        }
    }
    
    transactionModel->setFilter(MonthFilter);
    transactionModel->select();
}

QString MonthView::getTypeFromCategory(const QString& category) {
    for (int i = 0; i < incomesModel->rowCount(); ++i) {
        if (incomesModel->record(i).value("category").toString().toLower() == category) {
            return incomesModel->record(i).value("type").toString();
        }
    }
    return QStringLiteral("expense");
}

QWidget* MonthView::findWidgetByTexto(QLayout *layout, const QString &textoBuscado) {
    if (!layout) return nullptr;
    for (int i = 0; i < layout->count(); ++i) {
        QLayoutItem *item = layout->itemAt(i);
        if (!item) continue;

        QWidget *widget = item->widget();
        if (widget) {
            QLabel *label = qobject_cast<QLabel*>(widget);

            if (label && label->text().trimmed() == textoBuscado.trimmed()) {
                return widget;
            }
        }
    }
    return nullptr;
}


void MonthView::updateLabelsFromFilter(QSqlTableModel *model, const QString &labelPrefix) {
    for (int i = 0; i < model->rowCount(); ++i) {
        QString objectCategory = QString("%1%2").arg(labelPrefix).arg(i + 1);
        QString categoryName = model->index(i, 1).data().toString();
        QLabel *label = this->findChild<QLabel*>(objectCategory);

        if (label) {
            if (!categoryName.isEmpty()) {
                categoryName[0] = categoryName[0].toUpper();
            }
            label->setText(categoryName);
            label->setVisible(true);
        } else {
            qDebug() << "Label not found:" << objectCategory;
        }
    }

    this->update();
    this->repaint();
}

void MonthView::updateSummary(){
    totalIncomes = 0.0;
    totalExpenses = 0.0;

    for (const auto& [categoria, monto] : amountByCategoryMap.asKeyValueRange()) {
        QString tipo = getTypeFromCategory(categoria.toLower());
        if(tipo == "expense"){
            totalExpenses += monto;
        }else{
            totalIncomes += monto;
        }
    }

    savings = totalIncomes - totalExpenses;

    ui->TotalSavingsAmount->setText(QString::number(savings));
    ui->TotalIncomesAmount->setText(QString::number(totalIncomes));
    ui->TotalExpensesAmount->setText(QString::number(totalExpenses));
}

void MonthView::updatePieChart() {
    if(amountByCategoryMap.isEmpty()) {updateSummary();}

    auto *series = new QPieSeries();
    for (const auto& [categoria, monto] : amountByCategoryMap.asKeyValueRange()) {
        if(getTypeFromCategory(categoria.toLower()) == "expense"){
            series->append(categoria, monto);
        }
    }

    if (totalIncomes > totalExpenses) {
        QPieSlice *slice = series->append("Disponible", savings);
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
            double porcentaje = 100 * slice->percentage();
            slice->setLabel(QString("%1: %2%").arg(slice->label()).arg(porcentaje, 0, 'f', 1));
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


void MonthView::updateAmountView(QSqlTableModel *model, QLayout* layout) {
    for (int i = 0; i < model->rowCount(); ++i) {
        QString category_name = model->index(i,1).data().toString();
        if (category_name.isEmpty()) continue;

        category_name[0] = category_name[0].toUpper();
        QWidget* name_widget = findWidgetByTexto(layout, category_name);

        if (name_widget) {;
            QString widget_id = QString("%1Amount").arg(name_widget->objectName());
            QLabel* label_monto = this->findChild<QLabel*>(widget_id);

            if (label_monto) {
                label_monto->setText(QString::number(amountByCategoryMap[category_name.toLower()]));
                label_monto->repaint();
            } else {
                qDebug() << " Amount label not found with ID:" << widget_id;
            }
        } else {
            qDebug() << " Category not found:" << category_name << "in layout" << layout->objectName();
        }
    }
}

void MonthView::initView(){
    QString currentDateTime = QDateTime::currentDateTime().toString("dd-MM-yyyy HH:mm");
    ui->DateNowLabel->setText(QString("Current time:\n %1").arg(currentDateTime));

    QString firstDate = QDate(dateViewSelected.year(), dateViewSelected.month(), 1).toString("yyyy-MM-dd");
    QString lastDate = QDate(dateViewSelected.year(), dateViewSelected.month(), dateViewSelected.daysInMonth()).toString("yyyy-MM-dd");

    MonthFilter = QString("money_transactions.date >= '%1' AND money_transactions.date <= '%2'").arg(firstDate).arg(lastDate);
    
    updateTransactions();
    updateLabelsFromFilter(incomesModel, "Income");
    updateLabelsFromFilter(expensesModel, "Expense");

    setAmountByCategory();
    updateAmountView(incomesModel, ui->IncomesLayout);
    updateAmountView(expensesModel, ui->ExpensesLayout);
    updateSummary();
    updatePieChart();
    QCoreApplication::processEvents();
}