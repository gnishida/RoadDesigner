/********************************************************************************
** Form generated from reading UI file 'RoadBoxList.ui'
**
** Created: Fri Feb 7 09:50:41 2014
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ROADBOXLIST_H
#define UI_ROADBOXLIST_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RoadBoxList
{
public:

    void setupUi(QWidget *RoadBoxList)
    {
        if (RoadBoxList->objectName().isEmpty())
            RoadBoxList->setObjectName(QString::fromUtf8("RoadBoxList"));
        RoadBoxList->resize(295, 775);

        retranslateUi(RoadBoxList);

        QMetaObject::connectSlotsByName(RoadBoxList);
    } // setupUi

    void retranslateUi(QWidget *RoadBoxList)
    {
        RoadBoxList->setWindowTitle(QApplication::translate("RoadBoxList", "RoadBoxList", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class RoadBoxList: public Ui_RoadBoxList {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ROADBOXLIST_H
