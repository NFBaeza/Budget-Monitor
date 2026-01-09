/********************************************************************************
** Form generated from reading UI file 'dashboardwidget.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DASHBOARDWIDGET_H
#define UI_DASHBOARDWIDGET_H

#include <QtCharts/QChartView>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MonthView
{
public:
    QGridLayout *gridLayout;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *AddEntryButton;
    QFormLayout *IncomesLayout;
    QLabel *label;
    QLabel *Income1;
    QLabel *Income1Amount;
    QLabel *Income2;
    QLabel *Income2Amount;
    QLabel *Income3;
    QLabel *Income3Amount;
    QLabel *Income4;
    QLabel *Income4Amount;
    QLabel *Income5;
    QLabel *Income5Amount;
    QFormLayout *ExpensesLayout;
    QLabel *Expenses_title;
    QLabel *Expense1;
    QLabel *Expense1Amount;
    QLabel *Expense2;
    QLabel *Expense2Amount;
    QLabel *Expense3;
    QLabel *Expense3Amount;
    QLabel *Expense4;
    QLabel *Expense4Amount;
    QLabel *Expense5;
    QLabel *Expense5Amount;
    QChartView *graphicsView;
    QLabel *DateNowLabel;
    QLabel *MonthNameLabel;
    QVBoxLayout *verticalLayout_5;
    QLabel *label_4;
    QFrame *line_2;
    QLabel *TotalSavingsLabel;
    QLabel *TotalSavingsAmount;
    QLabel *TotalIncomesLabel;
    QLabel *TotalIncomesAmount;
    QLabel *TotalExpensesLabel;
    QLabel *TotalExpensesAmount;
    QVBoxLayout *verticalLayout;
    QLabel *label_3;
    QTableView *TableViewLastEntry;
    QSpacerItem *verticalSpacer_2;
    QVBoxLayout *verticalLayout_2;
    QPushButton *BackButton;
    QSpacerItem *verticalSpacer_3;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *EditCategoryButton;

    void setupUi(QWidget *MonthView)
    {
        if (MonthView->objectName().isEmpty())
            MonthView->setObjectName("MonthView");
        MonthView->setWindowModality(Qt::WindowModality::WindowModal);
        MonthView->resize(1020, 680);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MonthView->sizePolicy().hasHeightForWidth());
        MonthView->setSizePolicy(sizePolicy);
        gridLayout = new QGridLayout(MonthView);
        gridLayout->setSpacing(10);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setSizeConstraint(QLayout::SizeConstraint::SetDefaultConstraint);
        gridLayout->setContentsMargins(-1, 10, -1, 20);
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        gridLayout->addItem(verticalSpacer, 5, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(50);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(50, -1, 50, -1);
        AddEntryButton = new QPushButton(MonthView);
        AddEntryButton->setObjectName("AddEntryButton");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(AddEntryButton->sizePolicy().hasHeightForWidth());
        AddEntryButton->setSizePolicy(sizePolicy1);
        AddEntryButton->setMinimumSize(QSize(150, 0));

        horizontalLayout_2->addWidget(AddEntryButton);


        gridLayout->addLayout(horizontalLayout_2, 7, 2, 1, 2);

        IncomesLayout = new QFormLayout();
        IncomesLayout->setObjectName("IncomesLayout");
        IncomesLayout->setFieldGrowthPolicy(QFormLayout::FieldGrowthPolicy::AllNonFixedFieldsGrow);
        IncomesLayout->setRowWrapPolicy(QFormLayout::RowWrapPolicy::WrapLongRows);
        IncomesLayout->setVerticalSpacing(15);
        IncomesLayout->setContentsMargins(0, 20, 0, 0);
        label = new QLabel(MonthView);
        label->setObjectName("label");
        label->setFrameShadow(QFrame::Shadow::Raised);
        label->setAlignment(Qt::AlignmentFlag::AlignBottom|Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft);

        IncomesLayout->setWidget(0, QFormLayout::LabelRole, label);

        Income1 = new QLabel(MonthView);
        Income1->setObjectName("Income1");
        Income1->setEnabled(true);
        Income1->setStyleSheet(QString::fromUtf8(""));
        Income1->setScaledContents(false);

        IncomesLayout->setWidget(1, QFormLayout::LabelRole, Income1);

        Income1Amount = new QLabel(MonthView);
        Income1Amount->setObjectName("Income1Amount");
        Income1Amount->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        Income1Amount->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        IncomesLayout->setWidget(1, QFormLayout::FieldRole, Income1Amount);

        Income2 = new QLabel(MonthView);
        Income2->setObjectName("Income2");

        IncomesLayout->setWidget(2, QFormLayout::LabelRole, Income2);

        Income2Amount = new QLabel(MonthView);
        Income2Amount->setObjectName("Income2Amount");
        Income2Amount->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        IncomesLayout->setWidget(2, QFormLayout::FieldRole, Income2Amount);

        Income3 = new QLabel(MonthView);
        Income3->setObjectName("Income3");

        IncomesLayout->setWidget(3, QFormLayout::LabelRole, Income3);

        Income3Amount = new QLabel(MonthView);
        Income3Amount->setObjectName("Income3Amount");
        Income3Amount->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        IncomesLayout->setWidget(3, QFormLayout::FieldRole, Income3Amount);

        Income4 = new QLabel(MonthView);
        Income4->setObjectName("Income4");

        IncomesLayout->setWidget(4, QFormLayout::LabelRole, Income4);

        Income4Amount = new QLabel(MonthView);
        Income4Amount->setObjectName("Income4Amount");
        Income4Amount->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        IncomesLayout->setWidget(4, QFormLayout::FieldRole, Income4Amount);

        Income5 = new QLabel(MonthView);
        Income5->setObjectName("Income5");

        IncomesLayout->setWidget(5, QFormLayout::LabelRole, Income5);

        Income5Amount = new QLabel(MonthView);
        Income5Amount->setObjectName("Income5Amount");
        Income5Amount->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        IncomesLayout->setWidget(5, QFormLayout::FieldRole, Income5Amount);


        gridLayout->addLayout(IncomesLayout, 6, 0, 1, 1);

        ExpensesLayout = new QFormLayout();
        ExpensesLayout->setObjectName("ExpensesLayout");
        ExpensesLayout->setSizeConstraint(QLayout::SizeConstraint::SetDefaultConstraint);
        ExpensesLayout->setRowWrapPolicy(QFormLayout::RowWrapPolicy::DontWrapRows);
        ExpensesLayout->setFormAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignTop);
        ExpensesLayout->setHorizontalSpacing(5);
        ExpensesLayout->setVerticalSpacing(15);
        ExpensesLayout->setContentsMargins(0, 20, 0, 0);
        Expenses_title = new QLabel(MonthView);
        Expenses_title->setObjectName("Expenses_title");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(Expenses_title->sizePolicy().hasHeightForWidth());
        Expenses_title->setSizePolicy(sizePolicy2);
        Expenses_title->setFrameShape(QFrame::Shape::NoFrame);
        Expenses_title->setFrameShadow(QFrame::Shadow::Raised);
        Expenses_title->setAlignment(Qt::AlignmentFlag::AlignBottom|Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft);

        ExpensesLayout->setWidget(0, QFormLayout::LabelRole, Expenses_title);

        Expense1 = new QLabel(MonthView);
        Expense1->setObjectName("Expense1");
        Expense1->setEnabled(true);

        ExpensesLayout->setWidget(1, QFormLayout::LabelRole, Expense1);

        Expense1Amount = new QLabel(MonthView);
        Expense1Amount->setObjectName("Expense1Amount");
        Expense1Amount->setEnabled(true);
        Expense1Amount->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        Expense1Amount->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        ExpensesLayout->setWidget(1, QFormLayout::FieldRole, Expense1Amount);

        Expense2 = new QLabel(MonthView);
        Expense2->setObjectName("Expense2");
        Expense2->setEnabled(true);

        ExpensesLayout->setWidget(2, QFormLayout::LabelRole, Expense2);

        Expense2Amount = new QLabel(MonthView);
        Expense2Amount->setObjectName("Expense2Amount");
        Expense2Amount->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        ExpensesLayout->setWidget(2, QFormLayout::FieldRole, Expense2Amount);

        Expense3 = new QLabel(MonthView);
        Expense3->setObjectName("Expense3");
        Expense3->setEnabled(true);

        ExpensesLayout->setWidget(3, QFormLayout::LabelRole, Expense3);

        Expense3Amount = new QLabel(MonthView);
        Expense3Amount->setObjectName("Expense3Amount");
        Expense3Amount->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        ExpensesLayout->setWidget(3, QFormLayout::FieldRole, Expense3Amount);

        Expense4 = new QLabel(MonthView);
        Expense4->setObjectName("Expense4");
        Expense4->setEnabled(true);

        ExpensesLayout->setWidget(4, QFormLayout::LabelRole, Expense4);

        Expense4Amount = new QLabel(MonthView);
        Expense4Amount->setObjectName("Expense4Amount");
        Expense4Amount->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        ExpensesLayout->setWidget(4, QFormLayout::FieldRole, Expense4Amount);

        Expense5 = new QLabel(MonthView);
        Expense5->setObjectName("Expense5");
        Expense5->setEnabled(true);

        ExpensesLayout->setWidget(5, QFormLayout::LabelRole, Expense5);

        Expense5Amount = new QLabel(MonthView);
        Expense5Amount->setObjectName("Expense5Amount");
        Expense5Amount->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        ExpensesLayout->setWidget(5, QFormLayout::FieldRole, Expense5Amount);


        gridLayout->addLayout(ExpensesLayout, 6, 1, 1, 1);

        graphicsView = new QChartView(MonthView);
        graphicsView->setObjectName("graphicsView");
        graphicsView->setMinimumSize(QSize(250, 300));
        graphicsView->setFrameShape(QFrame::Shape::Box);
        graphicsView->setFrameShadow(QFrame::Shadow::Raised);

        gridLayout->addWidget(graphicsView, 4, 0, 1, 2);

        DateNowLabel = new QLabel(MonthView);
        DateNowLabel->setObjectName("DateNowLabel");
        DateNowLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        gridLayout->addWidget(DateNowLabel, 1, 2, 1, 2);

        MonthNameLabel = new QLabel(MonthView);
        MonthNameLabel->setObjectName("MonthNameLabel");
        MonthNameLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        gridLayout->addWidget(MonthNameLabel, 1, 0, 1, 2);

        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setSpacing(10);
        verticalLayout_5->setObjectName("verticalLayout_5");
        verticalLayout_5->setSizeConstraint(QLayout::SizeConstraint::SetDefaultConstraint);
        verticalLayout_5->setContentsMargins(20, 50, 30, 0);
        label_4 = new QLabel(MonthView);
        label_4->setObjectName("label_4");
        label_4->setFrameShadow(QFrame::Shadow::Raised);

        verticalLayout_5->addWidget(label_4);

        line_2 = new QFrame(MonthView);
        line_2->setObjectName("line_2");
        line_2->setFrameShape(QFrame::Shape::HLine);
        line_2->setFrameShadow(QFrame::Shadow::Sunken);

        verticalLayout_5->addWidget(line_2);

        TotalSavingsLabel = new QLabel(MonthView);
        TotalSavingsLabel->setObjectName("TotalSavingsLabel");

        verticalLayout_5->addWidget(TotalSavingsLabel);

        TotalSavingsAmount = new QLabel(MonthView);
        TotalSavingsAmount->setObjectName("TotalSavingsAmount");
        TotalSavingsAmount->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        verticalLayout_5->addWidget(TotalSavingsAmount);

        TotalIncomesLabel = new QLabel(MonthView);
        TotalIncomesLabel->setObjectName("TotalIncomesLabel");
        TotalIncomesLabel->setFrameShape(QFrame::Shape::NoFrame);
        TotalIncomesLabel->setFrameShadow(QFrame::Shadow::Plain);
        TotalIncomesLabel->setLineWidth(0);
        TotalIncomesLabel->setTextFormat(Qt::TextFormat::AutoText);
        TotalIncomesLabel->setScaledContents(false);

        verticalLayout_5->addWidget(TotalIncomesLabel);

        TotalIncomesAmount = new QLabel(MonthView);
        TotalIncomesAmount->setObjectName("TotalIncomesAmount");
        TotalIncomesAmount->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        verticalLayout_5->addWidget(TotalIncomesAmount);

        TotalExpensesLabel = new QLabel(MonthView);
        TotalExpensesLabel->setObjectName("TotalExpensesLabel");

        verticalLayout_5->addWidget(TotalExpensesLabel);

        TotalExpensesAmount = new QLabel(MonthView);
        TotalExpensesAmount->setObjectName("TotalExpensesAmount");
        TotalExpensesAmount->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        verticalLayout_5->addWidget(TotalExpensesAmount);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(5);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
        verticalLayout->setContentsMargins(0, -1, -1, -1);
        label_3 = new QLabel(MonthView);
        label_3->setObjectName("label_3");
        label_3->setFrameShadow(QFrame::Shadow::Raised);

        verticalLayout->addWidget(label_3);

        TableViewLastEntry = new QTableView(MonthView);
        TableViewLastEntry->setObjectName("TableViewLastEntry");
        TableViewLastEntry->setMinimumSize(QSize(500, 0));
        TableViewLastEntry->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
        TableViewLastEntry->setSortingEnabled(true);

        verticalLayout->addWidget(TableViewLastEntry);


        verticalLayout_5->addLayout(verticalLayout);


        gridLayout->addLayout(verticalLayout_5, 4, 2, 3, 2);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        gridLayout->addItem(verticalSpacer_2, 0, 0, 1, 1);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(180, 12, 180, -1);
        BackButton = new QPushButton(MonthView);
        BackButton->setObjectName("BackButton");
        sizePolicy2.setHeightForWidth(BackButton->sizePolicy().hasHeightForWidth());
        BackButton->setSizePolicy(sizePolicy2);

        verticalLayout_2->addWidget(BackButton);


        gridLayout->addLayout(verticalLayout_2, 8, 1, 1, 2);

        verticalSpacer_3 = new QSpacerItem(20, 10, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        gridLayout->addItem(verticalSpacer_3, 2, 0, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(120, -1, 120, -1);
        EditCategoryButton = new QPushButton(MonthView);
        EditCategoryButton->setObjectName("EditCategoryButton");
        QSizePolicy sizePolicy3(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Minimum);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(EditCategoryButton->sizePolicy().hasHeightForWidth());
        EditCategoryButton->setSizePolicy(sizePolicy3);
        EditCategoryButton->setMinimumSize(QSize(150, 0));

        horizontalLayout_3->addWidget(EditCategoryButton);


        gridLayout->addLayout(horizontalLayout_3, 7, 0, 1, 2);

#if QT_CONFIG(shortcut)
#endif // QT_CONFIG(shortcut)

        retranslateUi(MonthView);

        QMetaObject::connectSlotsByName(MonthView);
    } // setupUi

    void retranslateUi(QWidget *MonthView)
    {
        MonthView->setWindowTitle(QCoreApplication::translate("MonthView", "Month View", nullptr));
        AddEntryButton->setText(QCoreApplication::translate("MonthView", "Add Entry", nullptr));
        label->setText(QCoreApplication::translate("MonthView", "Incomes", nullptr));
#if QT_CONFIG(statustip)
        Income1->setStatusTip(QString());
#endif // QT_CONFIG(statustip)
#if QT_CONFIG(whatsthis)
        Income1->setWhatsThis(QString());
#endif // QT_CONFIG(whatsthis)
#if QT_CONFIG(accessibility)
        Income1->setAccessibleName(QString());
#endif // QT_CONFIG(accessibility)
#if QT_CONFIG(accessibility)
        Income1->setAccessibleDescription(QString());
#endif // QT_CONFIG(accessibility)
        Income1->setText(QCoreApplication::translate("MonthView", "Income1", nullptr));
        Income1Amount->setText(QString());
        Income2->setText(QCoreApplication::translate("MonthView", "Income2", nullptr));
        Income2Amount->setText(QString());
        Income3->setText(QCoreApplication::translate("MonthView", "Income3", nullptr));
        Income3Amount->setText(QString());
        Income4->setText(QCoreApplication::translate("MonthView", "Income4", nullptr));
        Income4Amount->setText(QString());
        Income5->setText(QCoreApplication::translate("MonthView", "Income5", nullptr));
        Income5Amount->setText(QString());
        Expenses_title->setText(QCoreApplication::translate("MonthView", "Expenses", nullptr));
        Expense1->setText(QCoreApplication::translate("MonthView", "Expense1", nullptr));
        Expense1Amount->setText(QString());
        Expense2->setText(QCoreApplication::translate("MonthView", "Expense2", nullptr));
        Expense2Amount->setText(QString());
        Expense3->setText(QCoreApplication::translate("MonthView", "Expense3", nullptr));
        Expense3Amount->setText(QString());
        Expense4->setText(QCoreApplication::translate("MonthView", "Expense4", nullptr));
        Expense4Amount->setText(QString());
        Expense5->setText(QCoreApplication::translate("MonthView", "Expense5", nullptr));
        Expense5Amount->setText(QString());
        DateNowLabel->setText(QCoreApplication::translate("MonthView", "TextLabel", nullptr));
        MonthNameLabel->setText(QCoreApplication::translate("MonthView", "Month", nullptr));
        label_4->setText(QCoreApplication::translate("MonthView", "Sumary", nullptr));
        TotalSavingsLabel->setText(QCoreApplication::translate("MonthView", "Total of Savings", nullptr));
        TotalSavingsAmount->setText(QCoreApplication::translate("MonthView", "0", nullptr));
        TotalIncomesLabel->setText(QCoreApplication::translate("MonthView", "Total of Incomes", nullptr));
        TotalIncomesAmount->setText(QCoreApplication::translate("MonthView", "0", nullptr));
        TotalExpensesLabel->setText(QCoreApplication::translate("MonthView", "Total of Expenses", nullptr));
        TotalExpensesAmount->setText(QCoreApplication::translate("MonthView", "0", nullptr));
        label_3->setText(QCoreApplication::translate("MonthView", "Last Entry", nullptr));
        BackButton->setText(QCoreApplication::translate("MonthView", "Back", nullptr));
        EditCategoryButton->setText(QCoreApplication::translate("MonthView", "Edit", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MonthView: public Ui_MonthView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DASHBOARDWIDGET_H
