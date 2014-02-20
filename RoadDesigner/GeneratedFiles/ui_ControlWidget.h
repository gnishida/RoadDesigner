/********************************************************************************
** Form generated from reading UI file 'ControlWidget.ui'
**
** Created: Thu Feb 20 12:26:45 2014
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONTROLWIDGET_H
#define UI_CONTROLWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDockWidget>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ControlWidget
{
public:
    QWidget *dockWidgetContents;
    QGroupBox *groupBox_3;
    QPushButton *pushButtonGenerateGrid;
    QRadioButton *radioButtonGridPattern1;
    QRadioButton *radioButtonGridPattern1_2;
    QGroupBox *groupBox_6;
    QPushButton *pushButtonGenerateRadial;
    QCheckBox *checkBoxRoadTypeAvenue;
    QCheckBox *checkBoxRoadTypeLocalStreet;
    QGroupBox *groupBox_7;
    QPushButton *pushButtonGenerateGeneric;
    QGroupBox *groupBox_8;
    QPushButton *pushButtonGenerateKDE;
    QLabel *label;
    QLineEdit *lineEditIteration;
    QCheckBox *checkBoxLocalStreets;
    QPushButton *pushButtonClear;
    QButtonGroup *buttonGroupGridPattern;

    void setupUi(QDockWidget *ControlWidget)
    {
        if (ControlWidget->objectName().isEmpty())
            ControlWidget->setObjectName(QString::fromUtf8("ControlWidget"));
        ControlWidget->resize(190, 831);
        ControlWidget->setMinimumSize(QSize(190, 240));
        ControlWidget->setStyleSheet(QString::fromUtf8("background-color: rgb(181, 181, 181);"));
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        groupBox_3 = new QGroupBox(dockWidgetContents);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(10, 160, 171, 111));
        pushButtonGenerateGrid = new QPushButton(groupBox_3);
        pushButtonGenerateGrid->setObjectName(QString::fromUtf8("pushButtonGenerateGrid"));
        pushButtonGenerateGrid->setGeometry(QRect(20, 70, 131, 31));
        radioButtonGridPattern1 = new QRadioButton(groupBox_3);
        buttonGroupGridPattern = new QButtonGroup(ControlWidget);
        buttonGroupGridPattern->setObjectName(QString::fromUtf8("buttonGroupGridPattern"));
        buttonGroupGridPattern->addButton(radioButtonGridPattern1);
        radioButtonGridPattern1->setObjectName(QString::fromUtf8("radioButtonGridPattern1"));
        radioButtonGridPattern1->setGeometry(QRect(30, 20, 101, 17));
        radioButtonGridPattern1_2 = new QRadioButton(groupBox_3);
        buttonGroupGridPattern->addButton(radioButtonGridPattern1_2);
        radioButtonGridPattern1_2->setObjectName(QString::fromUtf8("radioButtonGridPattern1_2"));
        radioButtonGridPattern1_2->setGeometry(QRect(30, 40, 101, 17));
        groupBox_6 = new QGroupBox(dockWidgetContents);
        groupBox_6->setObjectName(QString::fromUtf8("groupBox_6"));
        groupBox_6->setGeometry(QRect(10, 280, 171, 61));
        pushButtonGenerateRadial = new QPushButton(groupBox_6);
        pushButtonGenerateRadial->setObjectName(QString::fromUtf8("pushButtonGenerateRadial"));
        pushButtonGenerateRadial->setGeometry(QRect(20, 20, 131, 31));
        checkBoxRoadTypeAvenue = new QCheckBox(dockWidgetContents);
        checkBoxRoadTypeAvenue->setObjectName(QString::fromUtf8("checkBoxRoadTypeAvenue"));
        checkBoxRoadTypeAvenue->setGeometry(QRect(10, 10, 70, 17));
        checkBoxRoadTypeLocalStreet = new QCheckBox(dockWidgetContents);
        checkBoxRoadTypeLocalStreet->setObjectName(QString::fromUtf8("checkBoxRoadTypeLocalStreet"));
        checkBoxRoadTypeLocalStreet->setGeometry(QRect(80, 10, 91, 17));
        groupBox_7 = new QGroupBox(dockWidgetContents);
        groupBox_7->setObjectName(QString::fromUtf8("groupBox_7"));
        groupBox_7->setGeometry(QRect(10, 420, 171, 61));
        pushButtonGenerateGeneric = new QPushButton(groupBox_7);
        pushButtonGenerateGeneric->setObjectName(QString::fromUtf8("pushButtonGenerateGeneric"));
        pushButtonGenerateGeneric->setGeometry(QRect(20, 20, 131, 31));
        groupBox_8 = new QGroupBox(dockWidgetContents);
        groupBox_8->setObjectName(QString::fromUtf8("groupBox_8"));
        groupBox_8->setGeometry(QRect(10, 350, 171, 61));
        pushButtonGenerateKDE = new QPushButton(groupBox_8);
        pushButtonGenerateKDE->setObjectName(QString::fromUtf8("pushButtonGenerateKDE"));
        pushButtonGenerateKDE->setGeometry(QRect(20, 20, 131, 31));
        label = new QLabel(dockWidgetContents);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 60, 46, 16));
        lineEditIteration = new QLineEdit(dockWidgetContents);
        lineEditIteration->setObjectName(QString::fromUtf8("lineEditIteration"));
        lineEditIteration->setGeometry(QRect(100, 60, 61, 20));
        lineEditIteration->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        checkBoxLocalStreets = new QCheckBox(dockWidgetContents);
        checkBoxLocalStreets->setObjectName(QString::fromUtf8("checkBoxLocalStreets"));
        checkBoxLocalStreets->setGeometry(QRect(20, 90, 131, 17));
        pushButtonClear = new QPushButton(dockWidgetContents);
        pushButtonClear->setObjectName(QString::fromUtf8("pushButtonClear"));
        pushButtonClear->setGeometry(QRect(30, 120, 131, 31));
        ControlWidget->setWidget(dockWidgetContents);

        retranslateUi(ControlWidget);

        QMetaObject::connectSlotsByName(ControlWidget);
    } // setupUi

    void retranslateUi(QDockWidget *ControlWidget)
    {
        groupBox_3->setTitle(QApplication::translate("ControlWidget", "Grid Pattern", 0, QApplication::UnicodeUTF8));
        pushButtonGenerateGrid->setText(QApplication::translate("ControlWidget", "Generate", 0, QApplication::UnicodeUTF8));
        radioButtonGridPattern1->setText(QApplication::translate("ControlWidget", "Grid Pattern 1", 0, QApplication::UnicodeUTF8));
        radioButtonGridPattern1_2->setText(QApplication::translate("ControlWidget", "Grid Pattern 2", 0, QApplication::UnicodeUTF8));
        groupBox_6->setTitle(QApplication::translate("ControlWidget", "Radial Pattern", 0, QApplication::UnicodeUTF8));
        pushButtonGenerateRadial->setText(QApplication::translate("ControlWidget", "Generate", 0, QApplication::UnicodeUTF8));
        checkBoxRoadTypeAvenue->setText(QApplication::translate("ControlWidget", "Avenues", 0, QApplication::UnicodeUTF8));
        checkBoxRoadTypeLocalStreet->setText(QApplication::translate("ControlWidget", "Local Streets", 0, QApplication::UnicodeUTF8));
        groupBox_7->setTitle(QApplication::translate("ControlWidget", "Generic Pattern", 0, QApplication::UnicodeUTF8));
        pushButtonGenerateGeneric->setText(QApplication::translate("ControlWidget", "Generate", 0, QApplication::UnicodeUTF8));
        groupBox_8->setTitle(QApplication::translate("ControlWidget", "KDE Pattern", 0, QApplication::UnicodeUTF8));
        pushButtonGenerateKDE->setText(QApplication::translate("ControlWidget", "Generate", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ControlWidget", "Iteration:", 0, QApplication::UnicodeUTF8));
        checkBoxLocalStreets->setText(QApplication::translate("ControlWidget", "Generate Local Streets", 0, QApplication::UnicodeUTF8));
        pushButtonClear->setText(QApplication::translate("ControlWidget", "Clear", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(ControlWidget);
    } // retranslateUi

};

namespace Ui {
    class ControlWidget: public Ui_ControlWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTROLWIDGET_H
