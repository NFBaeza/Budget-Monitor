#include "dialogs/categorydialog.h"
#include "./ui_categorydialog.h"

CategoryDialog::CategoryDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CategoryDialog)
{
    ui->setupUi(this);

    incomeModel = DatabaseManager::instance().getIncomeModel(this);
    expenseModel = DatabaseManager::instance().getExpenseModel(this);
    accountModel = DatabaseManager::instance().getAccountModel(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &CategoryDialog::onSaveClicked);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &CategoryDialog::onCancelClicked);

    // Connect Income buttons
    connect(ui->IncomeAddButton, &QToolButton::clicked, this, &CategoryDialog::onAddIncomeClicked);
    connect(ui->IncomeRemoveButton, &QToolButton::clicked, this, &CategoryDialog::onDeleteIncomeClicked);
    connect(ui->listIncomeView, &QListView::doubleClicked, this, &CategoryDialog::onIncomeDoubleClicked);

    // Connect Expense buttons
    connect(ui->ExpenseAddButton, &QToolButton::clicked, this, &CategoryDialog::onAddExpenseClicked);
    connect(ui->ExpenseRemoveButton, &QToolButton::clicked, this, &CategoryDialog::onDeleteExpenseClicked);
    connect(ui->listExpenseView, &QListView::doubleClicked, this, &CategoryDialog::onExpenseDoubleClicked);

    // Connect Account buttons
    connect(ui->AccountAddButton, &QToolButton::clicked, this, &CategoryDialog::onAddAccountClicked);
    connect(ui->AccountRemoveButton, &QToolButton::clicked, this, &CategoryDialog::onDeleteAccountClicked);
    connect(ui->listAccountView, &QListView::doubleClicked, this, &CategoryDialog::onAccountDoubleClicked);

    initView();
}

CategoryDialog::~CategoryDialog() {
    delete ui;
}

void CategoryDialog::initView() {
    incomeModel->select();
    expenseModel->select();
    accountModel->select();

    ui->listIncomeView->setModel(incomeModel);
    ui->listIncomeView->setModelColumn(1);

    ui->listExpenseView->setModel(expenseModel);
    ui->listExpenseView->setModelColumn(1);

    ui->listAccountView->setModel(accountModel);
    ui->listAccountView->setModelColumn(1);
}

void CategoryDialog::loadCategories() {
    incomeModel->select();
    expenseModel->select();
}

void CategoryDialog::loadAccounts() {
    accountModel->select();
}

void CategoryDialog::onAddIncomeClicked() {
    bool ok;
    QString categoryName = QInputDialog::getText(this, "Add Income Category",
                                                  "Category name:",
                                                  QLineEdit::Normal, "", &ok);

    if (ok && !categoryName.isEmpty()) {
        if (addCategory(categoryName, "income")) {
            loadCategories();
            QMessageBox::information(this, "Success", "Income category added!");
        }
    }
}

void CategoryDialog::onDeleteIncomeClicked() {
    QModelIndex currentIndex = ui->listIncomeView->currentIndex();

    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "Warning", "Please select a category to delete");
        return;
    }

    int row = currentIndex.row();
    int id = incomeModel->index(row, 0).data().toInt();
    QString categoryName = incomeModel->index(row, 1).data().toString();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirm Delete",
        QString("Delete '%1' category?\n\nWarning: This may affect existing transactions.").arg(categoryName),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (deleteCategory(id)) {
            loadCategories();
            QMessageBox::information(this, "Success", "Category deleted!");
        }
    }
}

void CategoryDialog::onIncomeDoubleClicked(const QModelIndex &index) {
    int row = index.row();
    int id = incomeModel->index(row, 0).data().toInt();
    QString oldName = incomeModel->index(row, 1).data().toString();

    bool ok;
    QString newName = QInputDialog::getText(this, "Edit Income Category",
                                            "Category name:",
                                            QLineEdit::Normal, oldName, &ok);

    if (ok && !newName.isEmpty() && newName != oldName) {
        if (updateCategory(id, newName)) {
            loadCategories();
        }
    }
}

void CategoryDialog::onAddExpenseClicked() {
    bool ok;
    QString categoryName = QInputDialog::getText(this, "Add Expense Category",
                                                  "Category name:",
                                                  QLineEdit::Normal, "", &ok);

    if (ok && !categoryName.isEmpty()) {
        if (addCategory(categoryName, "expense")) {
            loadCategories();
            QMessageBox::information(this, "Success", "Expense category added!");
        }
    }
}

void CategoryDialog::onDeleteExpenseClicked() {
    QModelIndex currentIndex = ui->listExpenseView->currentIndex();

    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "Warning", "Please select a category to delete");
        return;
    }

    int row = currentIndex.row();
    int id = expenseModel->index(row, 0).data().toInt();
    QString categoryName = expenseModel->index(row, 1).data().toString();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirm Delete",
        QString("Delete '%1' category?\n\nWarning: This may affect existing transactions.").arg(categoryName),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (deleteCategory(id)) {
            loadCategories();
            QMessageBox::information(this, "Success", "Category deleted!");
        }
    }
}

void CategoryDialog::onExpenseDoubleClicked(const QModelIndex &index) {
    int row = index.row();
    int id = expenseModel->index(row, 0).data().toInt();
    QString oldName = expenseModel->index(row, 1).data().toString();

    bool ok;
    QString newName = QInputDialog::getText(this, "Edit Expense Category",
                                            "Category name:",
                                            QLineEdit::Normal, oldName, &ok);

    if (ok && !newName.isEmpty() && newName != oldName) {
        if (updateCategory(id, newName)) {
            loadCategories();
        }
    }
}

// ==================== ACCOUNTS ====================

void CategoryDialog::onAddAccountClicked() {
    bool ok;
    QString accountName = QInputDialog::getText(this, "Add Payment Method",
                                                "Method name:",
                                                QLineEdit::Normal, "", &ok);

    if (ok && !accountName.isEmpty()) {
        if (addAccount(accountName)) {
            loadAccounts();
            QMessageBox::information(this, "Success", "Payment method added!");
        }
    }
}

void CategoryDialog::onDeleteAccountClicked() {
    QModelIndex currentIndex = ui->listAccountView->currentIndex();

    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "Warning", "Please select an account to delete");
        return;
    }

    int row = currentIndex.row();
    int id = accountModel->index(row, 0).data().toInt();
    QString accountName = accountModel->index(row, 1).data().toString();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirm Delete",
        QString("Delete '%1' payment method?\n\nWarning: This may affect existing transactions.").arg(accountName),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (deleteAccount(id)) {
            loadAccounts();
            QMessageBox::information(this, "Success", "Payment method deleted!");
        }
    }
}

void CategoryDialog::onAccountDoubleClicked(const QModelIndex &index) {
    int row = index.row();
    int id = accountModel->index(row, 0).data().toInt();
    QString oldName = accountModel->index(row, 1).data().toString();

    bool ok;
    QString newName = QInputDialog::getText(this, "Edit Payment Method",
                                            "Method name:",
                                            QLineEdit::Normal, oldName, &ok);

    if (ok && !newName.isEmpty() && newName != oldName) {
        if (updateAccount(id, newName)) {
            loadAccounts();
        }
    }
}

// ==================== DATABASE OPERATIONS ====================

bool CategoryDialog::addCategory(const QString &categoryName, const QString &type) {
    QSqlQuery query(db);
    query.prepare("INSERT INTO categories (category, type) VALUES (:category, :type)");
    query.bindValue(":category", categoryName);
    query.bindValue(":type", type);

    if (!query.exec()) {
        qDebug() << "[addCategory] ERROR:" << query.lastError().text();
        QMessageBox::critical(this, "Error", "Failed to add category: " + query.lastError().text());
        return false;
    }

    return true;
}

bool CategoryDialog::updateCategory(int id, const QString &newName) {
    QSqlQuery query(db);
    query.prepare("UPDATE categories SET category = :category WHERE id = :id");
    query.bindValue(":category", newName);
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "[updateCategory] ERROR:" << query.lastError().text();
        QMessageBox::critical(this, "Error", "Failed to update category");
        return false;
    }

    return true;
}

bool CategoryDialog::deleteCategory(int id) {
    QSqlQuery query(db);
    query.prepare("DELETE FROM categories WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "[deleteCategory] ERROR:" << query.lastError().text();
        QMessageBox::critical(this, "Error", "Failed to delete category");
        return false;
    }

    return true;
}

bool CategoryDialog::addAccount(const QString &accountName) {
    QSqlQuery query(db);
    query.prepare("INSERT INTO payment_methods (method) VALUES (:method)");
    query.bindValue(":method", accountName);

    if (!query.exec()) {
        qDebug() << "[addAccount] ERROR:" << query.lastError().text();
        QMessageBox::critical(this, "Error", "Failed to add payment method");
        return false;
    }

    return true;
}

bool CategoryDialog::updateAccount(int id, const QString &newName) {
    QSqlQuery query(db);
    query.prepare("UPDATE payment_methods SET method = :method WHERE id = :id");
    query.bindValue(":method", newName);
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "[updateAccount] ERROR:" << query.lastError().text();
        QMessageBox::critical(this, "Error", "Failed to update payment method");
        return false;
    }

    return true;
}

bool CategoryDialog::deleteAccount(int id) {
    QSqlQuery query(db);
    query.prepare("DELETE FROM payment_methods WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "[deleteAccount] ERROR:" << query.lastError().text();
        QMessageBox::critical(this, "Error", "Failed to delete payment method");
        return false;
    }

    return true;
}

void CategoryDialog::onSaveClicked() {
    emit dataUpdated();
    accept();
}

void CategoryDialog::onCancelClicked() {
    reject();
}
