/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QFormLayout *formLayout;
    QLabel *TitleView;
    QPushButton *CurrentMonthButton;
    QVBoxLayout *verticalLayout;
    QPushButton *PastMonthsButton;
    QPushButton *SavingsViewButton;
    QPushButton *SettingsButton;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(646, 283);
        MainWindow->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
        centralwidget->setSizePolicy(sizePolicy);
        centralwidget->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        formLayout = new QFormLayout(centralwidget);
        formLayout->setObjectName("formLayout");
        formLayout->setFieldGrowthPolicy(QFormLayout::FieldGrowthPolicy::AllNonFixedFieldsGrow);
        formLayout->setLabelAlignment(Qt::AlignmentFlag::AlignCenter);
        formLayout->setFormAlignment(Qt::AlignmentFlag::AlignCenter);
        formLayout->setHorizontalSpacing(10);
        formLayout->setVerticalSpacing(50);
        formLayout->setContentsMargins(12, -1, -1, -1);
        TitleView = new QLabel(centralwidget);
        TitleView->setObjectName("TitleView");
        TitleView->setAlignment(Qt::AlignmentFlag::AlignCenter);

        formLayout->setWidget(0, QFormLayout::SpanningRole, TitleView);

        CurrentMonthButton = new QPushButton(centralwidget);
        CurrentMonthButton->setObjectName("CurrentMonthButton");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(CurrentMonthButton->sizePolicy().hasHeightForWidth());
        CurrentMonthButton->setSizePolicy(sizePolicy1);
        CurrentMonthButton->setMinimumSize(QSize(170, 90));
        QIcon icon(QIcon::fromTheme(QIcon::ThemeIcon::ViewRestore));
        CurrentMonthButton->setIcon(icon);

        formLayout->setWidget(1, QFormLayout::LabelRole, CurrentMonthButton);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(12);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
        verticalLayout->setContentsMargins(20, -1, 20, -1);
        PastMonthsButton = new QPushButton(centralwidget);
        PastMonthsButton->setObjectName("PastMonthsButton");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(PastMonthsButton->sizePolicy().hasHeightForWidth());
        PastMonthsButton->setSizePolicy(sizePolicy2);
        PastMonthsButton->setMinimumSize(QSize(200, 0));

        verticalLayout->addWidget(PastMonthsButton);

        SavingsViewButton = new QPushButton(centralwidget);
        SavingsViewButton->setObjectName("SavingsViewButton");
        QSizePolicy sizePolicy3(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(SavingsViewButton->sizePolicy().hasHeightForWidth());
        SavingsViewButton->setSizePolicy(sizePolicy3);
        SavingsViewButton->setMinimumSize(QSize(200, 0));

        verticalLayout->addWidget(SavingsViewButton);

        SettingsButton = new QPushButton(centralwidget);
        SettingsButton->setObjectName("SettingsButton");

        verticalLayout->addWidget(SettingsButton);


        formLayout->setLayout(1, QFormLayout::FieldRole, verticalLayout);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        TitleView->setText(QCoreApplication::translate("MainWindow", "Welcome to Budget Monitor! ", nullptr));
        CurrentMonthButton->setText(QCoreApplication::translate("MainWindow", "Current Month", nullptr));
        PastMonthsButton->setText(QCoreApplication::translate("MainWindow", "Past Month", nullptr));
        SavingsViewButton->setText(QCoreApplication::translate("MainWindow", "Saving", nullptr));
        SettingsButton->setText(QCoreApplication::translate("MainWindow", "Settings", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
