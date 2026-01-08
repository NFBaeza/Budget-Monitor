#include "dialogs/formdialog.h"
#include "./ui_formdialog.h"

FormDialog::FormDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FormDialog)
{
    ui->setupUi(this);
    ui->DeleteButton->setVisible(false);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &FormDialog::onAcceptClicked);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &FormDialog::onCancelClicked);
    connect(ui->IncomeRadioButton, &QRadioButton::toggled, this, &FormDialog::updateComboText);
    initView();
}

FormDialog::FormDialog(int transactionId, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::FormDialog),
      editingTransactionId(transactionId) {

    ui->setupUi(this);
    ui->DeleteButton->setVisible(true);
    ui->TitleFormDialog->setText("Edit Transaction");

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &FormDialog::onAcceptClicked);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &FormDialog::onCancelClicked);
    connect(ui->IncomeRadioButton, &QRadioButton::toggled, this, &FormDialog::updateComboText);

    initView();
    loadTransactionData(transactionId);
}

FormDialog::~FormDialog() {
    delete ui;
}

void FormDialog::loadTransactionData(int transactionId) {
    QSqlQuery query(db);
    query.prepare("SELECT date, amount, category, account, description "
                  "FROM money_transactions WHERE id = :id");
    query.bindValue(":id", transactionId);

    if (query.exec() && query.next()) {
        QString dateStr = query.value("date").toString();
        QDateTime dateTime = QDateTime::fromString(dateStr, "yyyy-MM-dd HH:mm:ss");
        ui->DateTimeSelected->setDateTime(dateTime);

        int amount = query.value("amount").toInt();
        ui->InputAmountText->setText(QString::number(amount));

        QString description = query.value("description").toString();
        ui->DescriptionText->setText(description);

        int categoryId = query.value("category").toInt();
        int accountId = query.value("account").toInt();

        // Primero determinar el tipo para filtrar las categorías correctamente
        QSqlQuery typeQuery(db);
        typeQuery.prepare("SELECT type FROM categories WHERE id = :id");
        typeQuery.bindValue(":id", categoryId);
        if (typeQuery.exec() && typeQuery.next()) {
            QString type = typeQuery.value("type").toString();
            if (type == "income") {
                ui->IncomeRadioButton->setChecked(true);
            } else {
                ui->ExpenseRadioButton->setChecked(true);
            }
            // updateComboText() se llamará automáticamente por el signal toggled
        }

        // Ahora buscar el índice de la categoría en el modelo filtrado
        for (int i = 0; i < categoryModel->rowCount(); ++i) {
            if (categoryModel->index(i, 0).data().toInt() == categoryId) {
                ui->ListCategoryDialog->setCurrentIndex(i);
                break;
            }
        }

        // Cargar la cuenta
        for (int i = 0; i < accountModel->rowCount(); ++i) {
            if (accountModel->index(i, 0).data().toInt() == accountId) {
                ui->ListAccountDialog->setCurrentIndex(i);
                break;
            }
        }
    }
}

void FormDialog::onAcceptClicked() {
    // Validar campos obligatorios
    if (ui->InputAmountText->text().isEmpty() || ui->DateTimeSelected->text().isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill all the options");
        return;
    }

    // Insertar la transacción
    if (!insertTransaction()) {
        QMessageBox::critical(this, "Error", "Failed to save transaction. Check console for details.");
        return;
    }

    // Mostrar mensaje de éxito
    QMessageBox::information(this, "Success", "Transaction saved successfully!");

    // Emitir señal para notificar al dashboard
    emit dataInserted();

    // Cerrar el diálogo
    accept();
}

void FormDialog::onCancelClicked() {
    reject();
}

QString FormDialog::getAmount() const {
    return ui->InputAmountText->text();
}

QString FormDialog::getDate() const {
    return ui->DateTimeSelected->dateTime().toString("yyyy-MM-dd HH:mm:ss");
}

QString FormDialog::getCategory() const {
    return ui->ListCategoryDialog->currentText();
}

QString FormDialog::getAcount() const {
    return ui->ListAccountDialog->currentText();
}

QString FormDialog::getDescription() const {
    return ui->DescriptionText->text();
}

void FormDialog::updateComboText(){
    //qDebug()<< "EL toggle fue activado con texto igual a  " << ;
    if(ui->IncomeRadioButton->isChecked()){
        categoryModel->setFilter("type == 'income'");
    }else{
        categoryModel->setFilter("type == 'expense'");
    }
    
    categoryModel->select();
    
    ui->ListCategoryDialog->setModel(categoryModel);
    ui->ListCategoryDialog->setModelColumn(1); 
    return;
}

void FormDialog::initView(){
    ui->TitleFormDialog->setText("Add new data");

    // Configurar modelo de categorías
    categoryModel->setTable("categories");
    categoryModel->select();
    updateComboText();

    // Configurar modelo de métodos de pago
    accountModel->setTable("payment_methods");
    accountModel->select();
    ui->ListAccountDialog->setModel(accountModel);
    ui->ListAccountDialog->setModelColumn(1);
}

bool FormDialog::insertTransaction() {
   // Obtener datos del formulario
    int categoryRow = ui->ListCategoryDialog->currentIndex();
    int categoryId = categoryModel->index(categoryRow, 0).data().toInt();
    
    int accountRow = ui->ListAccountDialog->currentIndex();
    int accountId = accountModel->index(accountRow, 0).data().toInt();
    
    QString date = ui->DateTimeSelected->dateTime().toString("yyyy-MM-dd HH:mm:ss");
    int amount = ui->InputAmountText->text().toDouble();
    QString description = ui->DescriptionText->text();

    QSqlQuery query(db);
    
    // Si estamos editando (id != -1), hacer UPDATE
    if (editingTransactionId != -1) {
        query.prepare("UPDATE money_transactions "
                      "SET date = :date, amount = :amount, category = :category, "
                      "account = :account, description = :description "
                      "WHERE id = :id");
        query.bindValue(":id", editingTransactionId);
    } else {
        query.prepare("INSERT INTO money_transactions (date, amount, category, account, description) "
                      "VALUES (:date, :amount, :category, :account, :description)");
    }
    
    query.bindValue(":date", date);
    query.bindValue(":amount", amount);
    query.bindValue(":category", categoryId);
    query.bindValue(":account", accountId);
    query.bindValue(":description", description);

    if (!query.exec()) {
        qDebug() << "[insertTransaction] ERROR:" << query.lastError().text();
        return false;
    }

    qDebug() << "[insertTransaction] Operación exitosa!";
    return true;
}