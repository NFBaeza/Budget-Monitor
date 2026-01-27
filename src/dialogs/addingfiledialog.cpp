#include "dialogs/addingfiledialog.h"
#include "./ui_addingfiledialog.h"

AddingFileDialog::AddingFileDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddingFileDialog)
{
    ui->setupUi(this);

    incomeModel = DatabaseManager::instance().getIncomeModel(this);
    expenseModel = DatabaseManager::instance().getExpenseModel(this);
    accountModel = DatabaseManager::instance().getAccountModel(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &AddingFileDialog::onAcceptClicked);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &AddingFileDialog::onCancelClicked);
    connect(ui->fileAddButton, &QToolButton::clicked, this, [this]() {
        QString filePath = QFileDialog::getOpenFileName(
            this, "Select a file", QString(), "Excel (*.xlsx *.xls)");
        if (!filePath.isEmpty()) {
            ui->filePathLineEdit->setText(filePath);
        }
    });

    ui->filePathLineEdit->setReadOnly(true); 
    ui->filePathLineEdit->setPlaceholderText("Select a file ...");
       
    ui->accountComboBox->setModel(accountModel);
    ui->accountComboBox->setModelColumn(1);
}

AddingFileDialog::~AddingFileDialog() {
    delete ui;
}

bool AddingFileDialog::uploadDataToDataBase(std::unique_ptr<Bank>& bank){
    if(bank->transactions.empty()){
        bank->readBankMovements();
    }
    if(bank->transactions.empty()){
        qDebug() << "No transactions to upload";
        return false;
    }

    for(auto& t : bank->transactions){
        QSqlQuery categoryQuery;
        categoryQuery.prepare("SELECT id FROM categories WHERE category = :category");
        categoryQuery.bindValue(":category", t.category);
        
        if (!categoryQuery.exec()) {
            qDebug() << "Error SELECT category:" << categoryQuery.lastError().text();
            return false;
        }
        
        int categoryId;
        if (!categoryQuery.next()) {
            QSqlQuery insertCat;
            insertCat.prepare("INSERT INTO categories (category, type) VALUES (:category, :type)");
            insertCat.bindValue(":category", t.category);
            insertCat.bindValue(":type", (t.category.toLower() == "Abono") ? "income" : "expense");
            if (!insertCat.exec()) {
                qDebug() << "Error INSERT category:" << insertCat.lastError().text();
                return false;
            }
            categoryId = insertCat.lastInsertId().toInt();
        } else {
            categoryId = categoryQuery.value(0).toInt();
        }

       
        QSqlQuery accountQuery;
        accountQuery.prepare("SELECT id FROM payment_methods WHERE method = :account"); 
        accountQuery.bindValue(":account", t.account);
        if (!accountQuery.exec()) {
            qDebug() << "Error SELECT account:" << accountQuery.lastError().text();
            return false;
        }
        
        int accountId;
        if (!accountQuery.next()) {
            QSqlQuery insertAcc;
            insertAcc.prepare("INSERT INTO payment_methods (method) VALUES (:method)");
            insertAcc.bindValue(":method", t.account);
            if (!insertAcc.exec()) {
                qDebug() << "Error INSERT account:" << insertAcc.lastError().text();
                return false;
            }
            accountId = insertAcc.lastInsertId().toInt();
        } else {
            accountId = accountQuery.value(0).toInt();
        }

        QSqlQuery insertQuery;
        insertQuery.prepare("INSERT INTO money_transactions (date, amount, category, account, description) "
                            "VALUES (:date, :amount, :category, :account, :description)");
        insertQuery.bindValue(":date", t.date);
        insertQuery.bindValue(":amount", t.amount);
        insertQuery.bindValue(":category", categoryId);
        insertQuery.bindValue(":account", accountId);
        insertQuery.bindValue(":description", t.description);
        
        if (!insertQuery.exec()) {
            qDebug() << "Error INSERT transaction:" << insertQuery.lastError().text();
            return false;
        }
    }
    
    qDebug() << "DB path:" << QSqlDatabase::database().databaseName();
    qDebug() << "Transactions count:" << bank->transactions.size();
    qDebug() << "[uploadDataToDataBase] Success!" << bank->transactions.size() << "transactions inserted";
    return true;
}

void AddingFileDialog::onAcceptClicked() {
    if (ui->filePathLineEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Error", "Please add a file");
        return;
    }

    QStringList bankSelect = ui->accountComboBox->currentText().split(" ");
    if (bankSelect.size() < 2) {
        QMessageBox::warning(this, "Error", "Invalid account selection");
        return;
    }

    QString filePath = ui->filePathLineEdit->text();
    auto bank = BankFactory::create(bankSelect[0], bankSelect[1], filePath);

    if (!bank) {
        QMessageBox::warning(this, "Error", "Could not create bank reader");
        return;
    }

    bank->filePath = filePath;
    if (uploadDataToDataBase(bank)) {
        qDebug() << "------------------------";
        accept();
    }
}

void AddingFileDialog::onCancelClicked() {
    reject();
}

