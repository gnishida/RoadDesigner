/********************************************************************************
** Form generated from reading UI file 'ControlWidget.ui'
**
** Created: Thu Feb 27 15:51:37 2014
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
#include <QtGui/QDial>
#include <QtGui/QDockWidget>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ControlWidget
{
public:
    QWidget *dockWidgetContents;
    QCheckBox *checkBoxRoadTypeAvenue;
    QCheckBox *checkBoxRoadTypeLocalStreet;
    QGroupBox *groupBox_8;
    QPushButton *pushButtonGenerateKDE;
    QPushButton *pushButtonClear;
    QPushButton *pushButtonConnect;
    QDial *dialOrientation;
    QLabel *label_2;
    QGroupBox *groupBox;
    QCheckBox *checkBoxAddAvenuesOnBoundary;
    QLabel *label;
    QLineEdit *lineEditIteration;
    QCheckBox *checkBoxLocalStreets;
    QCheckBox *checkBoxRoadTypeHighway;
    QCheckBox *checkBoxRoadTypeBoulevard;
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
        checkBoxRoadTypeAvenue = new QCheckBox(dockWidgetContents);
        checkBoxRoadTypeAvenue->setObjectName(QString::fromUtf8("checkBoxRoadTypeAvenue"));
        checkBoxRoadTypeAvenue->setGeometry(QRect(10, 30, 70, 17));
        checkBoxRoadTypeLocalStreet = new QCheckBox(dockWidgetContents);
        checkBoxRoadTypeLocalStreet->setObjectName(QString::fromUtf8("checkBoxRoadTypeLocalStreet"));
        checkBoxRoadTypeLocalStreet->setGeometry(QRect(90, 30, 91, 17));
        groupBox_8 = new QGroupBox(dockWidgetContents);
        groupBox_8->setObjectName(QString::fromUtf8("groupBox_8"));
        groupBox_8->setGeometry(QRect(10, 180, 171, 241));
        pushButtonGenerateKDE = new QPushButton(groupBox_8);
        pushButtonGenerateKDE->setObjectName(QString::fromUtf8("pushButtonGenerateKDE"));
        pushButtonGenerateKDE->setGeometry(QRect(20, 120, 131, 31));
        pushButtonClear = new QPushButton(groupBox_8);
        pushButtonClear->setObjectName(QString::fromUtf8("pushButtonClear"));
        pushButtonClear->setGeometry(QRect(20, 160, 131, 31));
        pushButtonConnect = new QPushButton(groupBox_8);
        pushButtonConnect->setObjectName(QString::fromUtf8("pushButtonConnect"));
        pushButtonConnect->setGeometry(QRect(20, 200, 131, 31));
        dialOrientation = new QDial(groupBox_8);
        dialOrientation->setObjectName(QString::fromUtf8("dialOrientation"));
        dialOrientation->setGeometry(QRect(70, 20, 91, 81));
        dialOrientation->setMaximum(359);
        dialOrientation->setSingleStep(10);
        dialOrientation->setValue(180);
        dialOrientation->setOrientation(Qt::Vertical);
        dialOrientation->setInvertedAppearance(false);
        dialOrientation->setInvertedControls(false);
        dialOrientation->setNotchTarget(3.7);
        dialOrientation->setNotchesVisible(true);
        label_2 = new QLabel(groupBox_8);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 20, 61, 16));
        groupBox = new QGroupBox(dockWidgetContents);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 60, 171, 101));
        checkBoxAddAvenuesOnBoundary = new QCheckBox(groupBox);
        checkBoxAddAvenuesOnBoundary->setObjectName(QString::fromUtf8("checkBoxAddAvenuesOnBoundary"));
        checkBoxAddAvenuesOnBoundary->setGeometry(QRect(10, 50, 151, 17));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 20, 46, 16));
        lineEditIteration = new QLineEdit(groupBox);
        lineEditIteration->setObjectName(QString::fromUtf8("lineEditIteration"));
        lineEditIteration->setGeometry(QRect(70, 20, 91, 20));
        lineEditIteration->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        checkBoxLocalStreets = new QCheckBox(groupBox);
        checkBoxLocalStreets->setObjectName(QString::fromUtf8("checkBoxLocalStreets"));
        checkBoxLocalStreets->setGeometry(QRect(10, 70, 131, 17));
        checkBoxRoadTypeHighway = new QCheckBox(dockWidgetContents);
        checkBoxRoadTypeHighway->setObjectName(QString::fromUtf8("checkBoxRoadTypeHighway"));
        checkBoxRoadTypeHighway->setGeometry(QRect(10, 10, 70, 17));
        checkBoxRoadTypeBoulevard = new QCheckBox(dockWidgetContents);
        checkBoxRoadTypeBoulevard->setObjectName(QString::fromUtf8("checkBoxRoadTypeBoulevard"));
        checkBoxRoadTypeBoulevard->setGeometry(QRect(90, 10, 91, 17));
        ControlWidget->setWidget(dockWidgetContents);

        retranslateUi(ControlWidget);

        QMetaObject::connectSlotsByName(ControlWidget);
    } // setupUi

    void retranslateUi(QDockWidget *ControlWidget)
    {
        checkBoxRoadTypeAvenue->setText(QApplication::translate("ControlWidget", "Avenues", 0, QApplication::UnicodeUTF8));
        checkBoxRoadTypeLocalStreet->setText(QApplication::translate("ControlWidget", "Local Streets", 0, QApplication::UnicodeUTF8));
        groupBox_8->setTitle(QApplication::translate("ControlWidget", "KDE Pattern", 0, QApplication::UnicodeUTF8));
        pushButtonGenerateKDE->setText(QApplication::translate("ControlWidget", "Generate", 0, QApplication::UnicodeUTF8));
        pushButtonClear->setText(QApplication::translate("ControlWidget", "Clear", 0, QApplication::UnicodeUTF8));
        pushButtonConnect->setText(QApplication::translate("ControlWidget", "Connect", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("ControlWidget", "Orientation:", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("ControlWidget", "Options", 0, QApplication::UnicodeUTF8));
        checkBoxAddAvenuesOnBoundary->setText(QApplication::translate("ControlWidget", "Add Avenues on Boundary", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ControlWidget", "Iteration:", 0, QApplication::UnicodeUTF8));
        checkBoxLocalStreets->setText(QApplication::translate("ControlWidget", "Generate Local Streets", 0, QApplication::UnicodeUTF8));
        checkBoxRoadTypeHighway->setText(QApplication::translate("ControlWidget", "Highways", 0, QApplication::UnicodeUTF8));
        checkBoxRoadTypeBoulevard->setText(QApplication::translate("ControlWidget", "Boulevards", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(ControlWidget);
    } // retranslateUi

};

namespace Ui {
    class ControlWidget: public Ui_ControlWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTROLWIDGET_H
