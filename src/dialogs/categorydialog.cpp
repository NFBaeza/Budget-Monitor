#include "dialogs/categorydialog.h"
#include "database/databaseworker.h"
#include "./ui_categorydialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QWidget>
#include <QApplication>

extern QString user_id;

CategoryDialog::CategoryDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CategoryDialog)
{
    ui->setupUi(this);

    categoryModel = DatabaseManager::instance().getCategoryModel(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &CategoryDialog::onSaveClicked);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &CategoryDialog::onCancelClicked);

    connect(ui->AddButton, &QToolButton::clicked, this, &CategoryDialog::onAddClicked);
    connect(ui->RemoveButton, &QToolButton::clicked, this, &CategoryDialog::onDeleteClicked);

    initView();
}

CategoryDialog::~CategoryDialog() {
    categoryModel->clear();
    delete ui;
}

void CategoryDialog::initView() {
    categoryModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    loadCategories();
}

void CategoryDialog::loadCategories() {
    categoryModel->select();
    rebuildCategoryRows();
}

void CategoryDialog::runWorkerOp(const QString &opName,
                                  const QString &errorLabel,
                                  std::function<void(DatabaseWorker*)> invoke)
{
    setButtonsEnabled(false);
    auto *worker = DatabaseManager::instance().worker();

    connect(worker, &DatabaseWorker::operationFinished, this,
            [this, opName](const QString &op) {
        if (op == opName) {
            loadCategories();
            setButtonsEnabled(true);
            emit dataUpdated();
        }
    }, Qt::SingleShotConnection);

    connect(worker, &DatabaseWorker::operationError, this,
            [this, opName, errorLabel](const QString &op, const QString &err) {
        if (op == opName) {
            QMessageBox::critical(this, "Error",
                QString("Failed to %1: ").arg(errorLabel) + err);
            setButtonsEnabled(true);
        }
    }, Qt::SingleShotConnection);

    invoke(worker);
}

void CategoryDialog::rebuildCategoryRows() {
    auto *layout = ui->categoriesLayout;
    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (auto *w = item->widget()) {
            w->deleteLater();
        }
        delete item;
    }
    categoryRows.clear();

    const int rowCount = categoryModel->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        const int id = categoryModel->index(row, 0).data().toInt();
        const QString name = categoryModel->index(row, 2).data().toString();
        const QString type = categoryModel->index(row, 3).data().toString();

        auto *rowWidget = new QWidget(ui->scrollAreaWidgetContents);
        auto *rowLayout = new QHBoxLayout(rowWidget);
        rowLayout->setContentsMargins(0, 0, 0, 0);

        auto *nameEdit = new QLineEdit(name, rowWidget);
        auto *typeCombo = new QComboBox(rowWidget);
        typeCombo->addItems({"income", "expense", "investment"});
        const int idx = typeCombo->findText(type);
        if (idx >= 0) {
            typeCombo->setCurrentIndex(idx);
        }

        rowLayout->addWidget(nameEdit);
        rowLayout->addWidget(typeCombo);
        layout->addWidget(rowWidget);

        categoryRows.append({id, name, type, nameEdit, typeCombo});
    }
    layout->addStretch();
}

void CategoryDialog::setButtonsEnabled(bool enabled) {
    ui->buttonBox->setEnabled(enabled);
    ui->AddButton->setEnabled(enabled);
    ui->RemoveButton->setEnabled(enabled);
}

// ==================== ADD / DELETE ====================

void CategoryDialog::onAddClicked() {
    QDialog dlg(this);
    dlg.setWindowTitle("Add Category");
    auto *form = new QFormLayout(&dlg);

    auto *nameEdit = new QLineEdit(&dlg);
    auto *typeCombo = new QComboBox(&dlg);
    typeCombo->addItems({"income", "expense", "investment"});

    form->addRow("Name:", nameEdit);
    form->addRow("Type:", typeCombo);

    auto *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) {
        return;
    }

    const QString categoryName = nameEdit->text().trimmed();
    const QString categoryType = typeCombo->currentText();
    if (categoryName.isEmpty()) {
        return;
    }

    runWorkerOp("addCategory", "add category",
        [categoryName, categoryType](DatabaseWorker *worker) {
            QMetaObject::invokeMethod(worker, "addCategory", Qt::QueuedConnection,
                                      Q_ARG(QString, user_id),
                                      Q_ARG(QString, categoryName),
                                      Q_ARG(QString, categoryType));
        });
}

void CategoryDialog::onDeleteClicked() {
    QWidget *focus = QApplication::focusWidget();
    int targetId = -1;
    QString targetName;
    for (const auto &row : categoryRows) {
        const bool focused = focus &&
            (focus == row.nameEdit || focus == row.typeCombo
             || row.nameEdit->isAncestorOf(focus)
             || row.typeCombo->isAncestorOf(focus));
        if (focused) {
            targetId = row.id;
            targetName = row.originalName;
            break;
        }
    }

    if (targetId < 0) {
        QMessageBox::warning(this, "Warning", "Please select a category to delete");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirm Delete",
        QString("Delete '%1' category?\n\nWarning: This may affect existing transactions.").arg(targetName),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        runWorkerOp("deleteCategory", "delete category",
            [targetId](DatabaseWorker *worker) {
                QMetaObject::invokeMethod(worker, "deleteCategory", Qt::QueuedConnection,
                                          Q_ARG(int, targetId));
            });
    }
}

void CategoryDialog::onDoubleClicked(const QModelIndex &index) {
    Q_UNUSED(index);
}

// ==================== SAVE / CANCEL ====================

void CategoryDialog::onSaveClicked() {
    bool dirty = false;
    for (const auto &row : categoryRows) {
        const QString newName = row.nameEdit->text();
        const QString newType = row.typeCombo->currentText();
        if (newName == row.originalName && newType == row.originalType) {
            continue;
        }

        for (int i = 0; i < categoryModel->rowCount(); ++i) {
            if (categoryModel->index(i, 0).data().toInt() != row.id) {
                continue;
            }
            if (newName != row.originalName) {
                categoryModel->setData(categoryModel->index(i, 2), newName);
            }
            if (newType != row.originalType) {
                categoryModel->setData(categoryModel->index(i, 3), newType);
            }
            dirty = true;
            break;
        }
    }

    if (dirty && !categoryModel->submitAll()) {
        QMessageBox::critical(this, "Error",
            "Failed to save changes: " + categoryModel->lastError().text());
        categoryModel->revertAll();
        return;
    }

    if (dirty) {
        emit dataUpdated();
    }
    accept();
}

void CategoryDialog::onCancelClicked() {
    reject();
}
