#include "widgets/dashboardwidget.h"
#include "views/mainwindow.h"
#include "./ui_dashboardwidget.h"
#include "dialogs/formdialog.h"
#include <time.h>
#include <QDateTime>
#include <QCoreApplication>


void PrintTable(QAbstractItemModel *model) {
    if (!model) {
        qDebug() << "El modelo es nulo.";
        return;
    }

    int rows = model->rowCount();
    int cols = model->columnCount();

    qDebug() << "--- Imprimiendo Tabla ---";
    qDebug() << "Filas:" << rows << "| Columnas:" << cols;

    // 1. Imprimir Encabezados
    QString headerLine = "| ";
    for (int c = 0; c < cols; ++c) {
        headerLine += model->headerData(c, Qt::Horizontal).toString() + " | ";
    }
    qDebug() << headerLine;
    qDebug() << QString("-").repeated(headerLine.length());

    // 2. Imprimir Datos
    for (int r = 0; r < rows; ++r) {
        QString rowLine = "| ";
        for (int c = 0; c < cols; ++c) {
            rowLine += model->data(model->index(r, c)).toString() + " | ";
        }
        qDebug() << rowLine;
    }
    qDebug() << "-------------------------";
}

MonthView::MonthView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MonthView) {
    ui->setupUi(this);
    this->setMinimumSize(1080, 650);

    month_name = QLocale().monthName(QDate::currentDate().month());
    connect(ui->BackButton, &QPushButton::clicked, this, &MonthView::BackButtonWasPressed);
    connect(ui->AddEntryButton, &QPushButton::clicked, this, &MonthView::onAddButtonClicked);

    InitView();
}

MonthView::~MonthView() {
    delete ui;
}

void MonthView::BackButtonWasPressed(){
    emit backbutton_was_pressed();

}

void  MonthView::UpdateTransactions(bool update_view){
    relational_model->clear();
    relational_model->setTable("money_transactions");
    relational_model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    relational_model->setRelation(3, QSqlRelation("categories", "id", "category"));
    relational_model->setRelation(4, QSqlRelation("payment_methods", "id", "method"));
    relational_model->setFilter(MonthFilter);
    relational_model->select();

    if(update_view){
        ui->TableViewLastEntry->setModel(relational_model);
        ui->TableViewLastEntry->setItemDelegate(new QSqlRelationalDelegate(ui->TableViewLastEntry));
        ui->TableViewLastEntry->resizeColumnsToContents();
        ui->TableViewLastEntry->horizontalHeader()->setStretchLastSection(true);
        ui->TableViewLastEntry->setColumnHidden(0,true);
    }

}

void  MonthView::UpdateCategories(){
    return;

}

void  MonthView::UpdateAccounts(){
   return;

}

void MonthView::onAddButtonClicked() {
    FormDialog dialog(this);

    // Mostrar el diálogo (bloquea hasta que se cierre)
    if (dialog.exec() == QDialog::Accepted) {
        qDebug() << "[onAddButtonClicked] Diálogo aceptado, refrescando vistas...";

        UpdateTransactions(true);

        AmountByCategoryMap.clear();
        SumAmountByCategory(AmountByCategoryMap);
        UpdateLabelsFromFilter(simple_model, "type = 'income'", "Income");
        UpdateLabelsFromFilter(simple_model, "type = 'expense'", "Expense");
        UpdateExpensesIncomesAmountView(); 
        UpdateSummary();
        UpdatePieChart();

        this->update();
        this->repaint();
        QCoreApplication::processEvents();
    }
}

void  MonthView::SumAmountByCategory(QMap<QString, int> &data_by_category) {
        QString originalFilter = relational_model->filter();
        
        int max_row = simple_model->rowCount();
        
        for (int i = 0; i < max_row; ++i) {
            QString category_idx  = simple_model->index(i,0).data().toString();
            QString category_name = simple_model->index(i,1).data().toString();    
            QString category_type = simple_model->index(i,2).data().toString();  

            QString filter = QString("money_transactions.category = '%1'").arg(category_idx);
            relational_model->setFilter(filter);

            if(relational_model->select()){
                for(int j = 0; j < relational_model->rowCount(); j++){
                    int amount = relational_model->data(relational_model->index(j, 2)).toInt();
                    data_by_category[category_name] += amount; 
                }
            }
        }
        
        relational_model->setFilter(originalFilter);
        relational_model->select();
}

QString MonthView::GetTypeFromCategory(QString category) {
    for (int i = 0; i < simple_model->rowCount(); ++i) {
        if (simple_model->record(i).value("category").toString() == category) {
            return simple_model->record(i).value("type").toString();
        }
    }
    return "";
}

QWidget* MonthView::FindWidgetByTexto(QLayout *layout, const QString &textoBuscado) {
    if (!layout) return nullptr;    
    for (int i = 0; i < layout->count(); ++i) {
        QWidget *widget = layout->itemAt(i)->widget();
        if (widget) {
            QLabel *label = qobject_cast<QLabel*>(widget);
                        
            if (label && label->text().trimmed() == textoBuscado.trimmed()) {
                return widget;
            }
        }
    }
    return nullptr; 
}


void MonthView::UpdateLabelsFromFilter(QSqlTableModel *model, const QString &filter, const QString &labelPrefix) {
    model->setFilter(filter);

    if (!model->select()) {
        qDebug() << "[UpdateLabelsFromFilter] Error al filtrar" << filter << ":" << model->lastError().text();
        return;
    }

    int nameCol = model->record().indexOf("category");

    for (int i = 0; i < model->rowCount(); ++i) {
        QString objectCategory = QString("%1%2").arg(labelPrefix).arg(i + 1);
        QString categoryName = model->index(i, nameCol).data().toString();
        QLabel *label = this->findChild<QLabel*>(objectCategory);

        if (label) {
            QString category = QString::number(i);
            label->setText(categoryName);
            label->setVisible(true);
        } else {
            qDebug() << "    ❌ Label NO encontrado!";
        }
    }
    model->setFilter("");
    model->select();
    this->update();
    this->repaint();
    QCoreApplication::processEvents();
}

void MonthView::UpdateSummary(){
    totalIncomes = 0.0;
    totalExpenses = 0.0;

    for (const auto& [categoria, monto] : AmountByCategoryMap.asKeyValueRange()) {
        QString tipo = GetTypeFromCategory(categoria);
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

void MonthView::UpdatePieChart() {
    if(AmountByCategoryMap.isEmpty()) {UpdateSummary();}
    QPieSeries *series = new QPieSeries();
    for (const auto& [categoria, monto] : AmountByCategoryMap.asKeyValueRange()) {
        if(GetTypeFromCategory(categoria) == "expense"){
            series->append(categoria, monto);
        } 
    }

    if (totalIncomes > totalExpenses) {
        QPieSlice *slice = series->append("Disponible", savings);
        slice->setBrush(Qt::lightGray); // Color gris para lo que sobra
    } else if (totalIncomes == 0 && totalExpenses > 0) {
        // Si no hay ingresos cargados, el gráfico será solo de gastos (100% gastos)
        //qDebug() << "Aviso: No hay ingresos, mostrando solo distribución de gastos.";
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Money Distribution");
    chart->legend()->hide();
    chart->setAnimationOptions(QChart::SeriesAnimations);

    for (QPieSlice *slice : series->slices()) {
        if (slice->value() <= 0) {
            slice->setLabelVisible(false); // Oculta la etiqueta
            slice->setExploded(false);
        } else {
            double porcentaje = 100 * slice->percentage();
            slice->setLabel(QString("%1: %2%").arg(slice->label()).arg(porcentaje, 0, 'f', 1));
            slice->setLabelVisible(true);
        }
    }

    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setChart(chart); 
    QCoreApplication::processEvents(); 
}


void MonthView::UpdateExpensesIncomesAmountView() {
    int max_row = simple_model->rowCount();
    for (int i = 0; i < max_row; ++i) {
        QString category_idx  = simple_model->index(i,0).data().toString();
        QString category_name = simple_model->index(i,1).data().toString();
        QString category_type = simple_model->index(i,2).data().toString();

        QLayout* layout = nullptr;
        if (category_type == "income") {
            layout = ui->IncomesLayout;
        } else if (category_type == "expense") {
            layout = ui->ExpensesLayout;
        }

        QWidget* name_widget = FindWidgetByTexto(layout, category_name);

        if (name_widget) {;
            QString widget_id = QString("%1Amount").arg(name_widget->objectName());
            QLabel* label_monto = this->findChild<QLabel*>(widget_id);

            if (label_monto) {
                int amount = AmountByCategoryMap[category_name];
                label_monto->setText(QString::number(amount));
                label_monto->repaint();
            } else {
                qDebug() << "    ❌ No se encontró el label de monto con ID:" << widget_id;
            }
        } else {
            qDebug() << "    ❌ No se encontró la categoría:" << category_name << "en el layout" << category_type;
        }
    }
}

void MonthView::InitView(){
    QString fechaFormateada = QDateTime::currentDateTime().toString("dd-MM-yyyy      HH:mm");
    ui->DateNowLabel->setText(fechaFormateada);
    ui->MonthNameLabel->setText(month_name);

    QDate hoy = QDate::currentDate();
    QString firstDate = QDate(hoy.year(), hoy.month(), 1).toString("yyyy-MM-dd");
    QString lastDate = QDate(hoy.year(), hoy.month(), hoy.daysInMonth()).toString("yyyy-MM-dd");
    
    MonthFilter = QString("money_transactions.date >= '%1' AND money_transactions.date <= '%2'").arg(firstDate).arg(lastDate);

    simple_model->setTable("categories");
    simple_model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    if (!simple_model->select()) {
        qDebug() << "Error SQL:" << simple_model->lastError().text();
    } 

    UpdateLabelsFromFilter(simple_model, "type = 'income'", "Income");
    UpdateLabelsFromFilter(simple_model, "type = 'expense'", "Expense");

    UpdateTransactions(true);
    SumAmountByCategory(AmountByCategoryMap);
    UpdateExpensesIncomesAmountView();
    UpdateSummary();
    UpdatePieChart();
}