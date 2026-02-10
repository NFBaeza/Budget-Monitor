#include "dialogs/formdialog.h"
#include "./ui_formdialog.h"

extern QString user_id;

FormDialog::FormDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FormDialog)
{
    ui->setupUi(this);
    ui->DeleteButton->setVisible(false);

    categoryModel = DatabaseManager::instance().getCategoryModel(this);
    accountModel = DatabaseManager::instance().getAccountModel(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &FormDialog::onAcceptClicked);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &FormDialog::onCancelClicked);
    connect(ui->IncomeRadioButton, &QRadioButton::toggled, this, &FormDialog::updateComboText);
    ui->DateTimeSelected->setDate(QDate::currentDate());
    initView();
}

FormDialog::FormDialog(int transactionId, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::FormDialog),
      editingTransactionId(transactionId) {

    ui->setupUi(this);
    ui->DeleteButton->setVisible(true);

    categoryModel = DatabaseManager::instance().getCategoryModel(this);
    accountModel = DatabaseManager::instance().getAccountModel(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &FormDialog::onAcceptClicked);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &FormDialog::onCancelClicked);
    connect(ui->DeleteButton, &QPushButton::pressed, this, &FormDialog::onDeleteClicked);
    connect(ui->IncomeRadioButton, &QRadioButton::toggled, this, &FormDialog::updateComboText);

    initView();
    loadTransactionData(transactionId);
}

FormDialog::~FormDialog() {
    delete ui;
}

void FormDialog::onDeleteClicked() {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, 
        "Confirm Delete", 
        "Are you sure you want to delete this transaction?",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply != QMessageBox::Yes) {
        return; 
    }
    
    QSqlQuery query(db);
    query.prepare("DELETE FROM money_transactions WHERE id = :id");
    query.bindValue(":id", editingTransactionId);
    
    if (!query.exec()) {  
        qDebug() << "[onDeleteClicked] ERROR:" << query.lastError().text();
        QMessageBox::critical(this, "Error", "Failed to delete transaction");
        return;
    }
    
    QMessageBox::information(this, "Success", "Transaction deleted successfully!");
    emit dataDeleted();  
    accept();         
}

void FormDialog::loadTransactionData(int transactionId) {
    QSqlQuery query(db);
    query.prepare("SELECT date, amount, category_id, account_id, description "
                  "FROM money_transactions WHERE id = :id AND user_id = :user_id");
    query.bindValue(":id", transactionId);
    query.bindValue(":user_id", user_id);

    if (query.exec() && query.next()) {
        QString dateStr = query.value("date").toString();
        QDateTime dateTime = QDateTime::fromString(dateStr, Qt::ISODate);
        ui->DateTimeSelected->setDateTime(dateTime);

        int amount = query.value("amount").toInt();
        ui->InputAmountText->setText(QString::number(amount));

        QString description = query.value("description").toString();
        ui->DescriptionText->setText(description);

        int categoryId = query.value("category_id").toInt();
        int accountId = query.value("account_id").toInt();

        QSqlQuery typeQuery(db);
        typeQuery.prepare("SELECT type FROM categories WHERE id = :id AND user_id = :user_id");
        typeQuery.bindValue(":id", categoryId);
        typeQuery.bindValue(":user_id", user_id);
        if (typeQuery.exec() && typeQuery.next()) {
            QString type = typeQuery.value("type").toString();
            if (type == "income") {
                ui->IncomeRadioButton->setChecked(true);
            } else {
                ui->ExpenseRadioButton->setChecked(true);
            }
        }

        for (int i = 0; i < categoryModel->rowCount(); ++i) {
            if (categoryModel->index(i, 0).data().toInt() == categoryId) {
                ui->ListCategoryDialog->setCurrentIndex(i);
                break;
            }
        }

        for (int i = 0; i < accountModel->rowCount(); ++i) {
            if (accountModel->index(i, 0).data().toInt() == accountId) {
                ui->ListAccountDialog->setCurrentIndex(i);
                break;
            }
        }
    }
}

void FormDialog::onAcceptClicked() {
    if (ui->InputAmountText->text().isEmpty() || ui->DateTimeSelected->text().isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill all the options");
        return;
    }
    
    if (!insertTransaction()) {
        QMessageBox::critical(this, "Error", "Failed to save transaction. Check console for details.");
        return;
    }

    QMessageBox::information(this, "Success", "Transaction saved successfully!");

    emit dataInserted();
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
    if(ui->IncomeRadioButton->isChecked()){
        categoryModel->setFilter(QString("type = 'income'AND user_id = '%1'").arg(user_id));
    }else{
        categoryModel->setFilter(QString("type = 'expense'AND user_id = '%1'").arg(user_id));
    }

    categoryModel->select();

    ui->ListCategoryDialog->setModel(categoryModel);
    ui->ListCategoryDialog->setModelColumn(2);
}

void FormDialog::initView(){
    if(editingTransactionId >= 0){
        ui->TitleFormDialog->setText("Edit Transaction");
    }else{
        ui->TitleFormDialog->setText("Add new data");
    }

    categoryModel->select();

    updateComboText();

    accountModel->select();
    ui->ListAccountDialog->setModel(accountModel);
    ui->ListAccountDialog->setModelColumn(2);
}

bool FormDialog::insertTransaction() {
    int categoryRow = ui->ListCategoryDialog->currentIndex();
    int categoryId = categoryModel->index(categoryRow, 0).data().toInt();
    qDebug()<<"[insertTransaction] categoryId antes del push es "<<categoryId;
    
    int accountRow = ui->ListAccountDialog->currentIndex();
    int accountId = accountModel->index(accountRow, 0).data().toInt();
    qDebug()<<"[insertTransaction] accountId antes del push  es "<<accountId;

    QString date = ui->DateTimeSelected->dateTime().toString("yyyy-MM-dd HH:mm:ss");
    int amount = ui->InputAmountText->text().toDouble();
    QString description = ui->DescriptionText->text();

    QSqlQuery query(db);

    if (editingTransactionId != -1) {
        query.prepare("UPDATE money_transactions "
                      "SET user_id = :user_id, date = :date, amount = :amount, category_id = :category, "
                      "account_id = :account, description = :description "
                      "WHERE id = :id");
        query.bindValue(":id", editingTransactionId);
    } else {
        query.prepare("INSERT INTO money_transactions (user_id, date, amount, category_id, account_id, description) "
                      "VALUES (:user_id, :date, :amount, :category, :account, :description)");
    }
    query.bindValue(":user_id", user_id);
    query.bindValue(":date", date);
    query.bindValue(":amount", amount);
    query.bindValue(":category", categoryId);
    query.bindValue(":account", accountId);
    query.bindValue(":description", description);

    if (!query.exec()) {
        qDebug() << "[insertTransaction] ERROR:" << query.lastError().text();
        return false;
    }

    qDebug() << "[insertTransaction] Operation successful!";
    return true;
}