#include "dialogs/categorydialog.h"
#include "database/databaseworker.h"
#include "./ui_categorydialog.h"

extern QString user_id;

CategoryDialog::CategoryDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CategoryDialog)
{
    ui->setupUi(this);

    incomeModel = DatabaseManager::instance().getIncomeModel(this);
    expenseModel = DatabaseManager::instance().getExpenseModel(this);

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

    initView();
}

CategoryDialog::~CategoryDialog() {
    incomeModel->clear();
    expenseModel->clear();
    delete ui;
}

void CategoryDialog::initView() {
    incomeModel->select();
    expenseModel->select();

    ui->listIncomeView->setModel(incomeModel);
    ui->listIncomeView->setModelColumn(2);

    ui->listExpenseView->setModel(expenseModel);
    ui->listExpenseView->setModelColumn(2);

}

void CategoryDialog::loadCategories() {
    incomeModel->select();
    expenseModel->select();
}

void CategoryDialog::setButtonsEnabled(bool enabled) {
    ui->buttonBox->setEnabled(enabled);
    ui->IncomeAddButton->setEnabled(enabled);
    ui->IncomeRemoveButton->setEnabled(enabled);
    ui->ExpenseAddButton->setEnabled(enabled);
    ui->ExpenseRemoveButton->setEnabled(enabled);

}

// ==================== INCOME ====================

void CategoryDialog::onAddIncomeClicked() {
    bool ok;
    QString categoryName = QInputDialog::getText(this, "Add Income Category",
                                                  "Category name:",
                                                  QLineEdit::Normal, "", &ok);

    if (ok && !categoryName.isEmpty()) {
        setButtonsEnabled(false);
        auto *worker = DatabaseManager::instance().worker();

        connect(worker, &DatabaseWorker::operationFinished, this, [this](const QString &op) {
            if (op == "addCategory") {
                loadCategories();
                setButtonsEnabled(true);
                emit dataUpdated();
            }
        }, Qt::SingleShotConnection);

        connect(worker, &DatabaseWorker::operationError, this, [this](const QString &op, const QString &err) {
            if (op == "addCategory") {
                QMessageBox::critical(this, "Error", "Failed to add category: " + err);
                setButtonsEnabled(true);
            }
        }, Qt::SingleShotConnection);

        QMetaObject::invokeMethod(worker, "addCategory", Qt::QueuedConnection,
                                  Q_ARG(QString, user_id),
                                  Q_ARG(QString, categoryName),
                                  Q_ARG(QString, "income"));
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
    QString categoryName = incomeModel->index(row, 2).data().toString();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirm Delete",
        QString("Delete '%1' category?\n\nWarning: This may affect existing transactions.").arg(categoryName),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        setButtonsEnabled(false);
        auto *worker = DatabaseManager::instance().worker();

        connect(worker, &DatabaseWorker::operationFinished, this, [this](const QString &op) {
            if (op == "deleteCategory") {
                loadCategories();
                setButtonsEnabled(true);
                emit dataUpdated();
            }
        }, Qt::SingleShotConnection);

        connect(worker, &DatabaseWorker::operationError, this, [this](const QString &op, const QString &err) {
            if (op == "deleteCategory") {
                QMessageBox::critical(this, "Error", "Failed to delete category: " + err);
                setButtonsEnabled(true);
            }
        }, Qt::SingleShotConnection);

        QMetaObject::invokeMethod(worker, "deleteCategory", Qt::QueuedConnection,
                                  Q_ARG(int, id));
    }
}

void CategoryDialog::onIncomeDoubleClicked(const QModelIndex &index) {
    int row = index.row();
    int id = incomeModel->index(row, 0).data().toInt();
    QString oldName = incomeModel->index(row, 2).data().toString();

    bool ok;
    QString newName = QInputDialog::getText(this, "Edit Income Category",
                                            "Category name:",
                                            QLineEdit::Normal, oldName, &ok);

    if (ok && !newName.isEmpty() && newName != oldName) {
        setButtonsEnabled(false);
        auto *worker = DatabaseManager::instance().worker();

        connect(worker, &DatabaseWorker::operationFinished, this, [this](const QString &op) {
            if (op == "updateCategory") {
                loadCategories();
                setButtonsEnabled(true);
                emit dataUpdated();
            }
        }, Qt::SingleShotConnection);

        connect(worker, &DatabaseWorker::operationError, this, [this](const QString &op, const QString &err) {
            if (op == "updateCategory") {
                QMessageBox::critical(this, "Error", "Failed to update category: " + err);
                setButtonsEnabled(true);
            }
        }, Qt::SingleShotConnection);

        QMetaObject::invokeMethod(worker, "updateCategory", Qt::QueuedConnection,
                                  Q_ARG(int, id),
                                  Q_ARG(QString, newName));
    }
}

// ==================== EXPENSE ====================

void CategoryDialog::onAddExpenseClicked() {
    bool ok;
    QString categoryName = QInputDialog::getText(this, "Add Expense Category",
                                                  "Category name:",
                                                  QLineEdit::Normal, "", &ok);

    if (ok && !categoryName.isEmpty()) {
        setButtonsEnabled(false);
        auto *worker = DatabaseManager::instance().worker();

        connect(worker, &DatabaseWorker::operationFinished, this, [this](const QString &op) {
            if (op == "addCategory") {
                loadCategories();
                setButtonsEnabled(true);
                emit dataUpdated();
            }
        }, Qt::SingleShotConnection);

        connect(worker, &DatabaseWorker::operationError, this, [this](const QString &op, const QString &err) {
            if (op == "addCategory") {
                QMessageBox::critical(this, "Error", "Failed to add category: " + err);
                setButtonsEnabled(true);
            }
        }, Qt::SingleShotConnection);

        QMetaObject::invokeMethod(worker, "addCategory", Qt::QueuedConnection,
                                  Q_ARG(QString, user_id),
                                  Q_ARG(QString, categoryName),
                                  Q_ARG(QString, "expense"));
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
    QString categoryName = expenseModel->index(row, 2).data().toString();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirm Delete",
        QString("Delete '%1' category?\n\nWarning: This may affect existing transactions.").arg(categoryName),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        setButtonsEnabled(false);
        auto *worker = DatabaseManager::instance().worker();

        connect(worker, &DatabaseWorker::operationFinished, this, [this](const QString &op) {
            if (op == "deleteCategory") {
                loadCategories();
                setButtonsEnabled(true);
                emit dataUpdated();
            }
        }, Qt::SingleShotConnection);

        connect(worker, &DatabaseWorker::operationError, this, [this](const QString &op, const QString &err) {
            if (op == "deleteCategory") {
                QMessageBox::critical(this, "Error", "Failed to delete category: " + err);
                setButtonsEnabled(true);
            }
        }, Qt::SingleShotConnection);

        QMetaObject::invokeMethod(worker, "deleteCategory", Qt::QueuedConnection,
                                  Q_ARG(int, id));
    }
}

void CategoryDialog::onExpenseDoubleClicked(const QModelIndex &index) {
    int row = index.row();
    int id = expenseModel->index(row, 0).data().toInt();
    QString oldName = expenseModel->index(row, 2).data().toString();

    bool ok;
    QString newName = QInputDialog::getText(this, "Edit Expense Category",
                                            "Category name:",
                                            QLineEdit::Normal, oldName, &ok);

    if (ok && !newName.isEmpty() && newName != oldName) {
        setButtonsEnabled(false);
        auto *worker = DatabaseManager::instance().worker();

        connect(worker, &DatabaseWorker::operationFinished, this, [this](const QString &op) {
            if (op == "updateCategory") {
                loadCategories();
                setButtonsEnabled(true);
                emit dataUpdated();
            }
        }, Qt::SingleShotConnection);

        connect(worker, &DatabaseWorker::operationError, this, [this](const QString &op, const QString &err) {
            if (op == "updateCategory") {
                QMessageBox::critical(this, "Error", "Failed to update category: " + err);
                setButtonsEnabled(true);
            }
        }, Qt::SingleShotConnection);

        QMetaObject::invokeMethod(worker, "updateCategory", Qt::QueuedConnection,
                                  Q_ARG(int, id),
                                  Q_ARG(QString, newName));
    }
}

// ==================== SAVE / CANCEL ====================

void CategoryDialog::onSaveClicked() {
    accept();
}

void CategoryDialog::onCancelClicked() {
    reject();
}
