#pragma once
#include <QMainWindow>

class EmployeePage;
class QButtonGroup;
class QLineEdit;
class QStackedWidget;

// Fenêtre principale : menu latéral + barre supérieure (créés une seule fois) + QStackedWidget des pages.
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum Page { PageHome, PageEmployee, PagePlanning, PagePerformance, PageSettings };

    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void goToPage(int index);
    void confirmLogout();
    void onTopSearchChanged(const QString &text);

private:
    QWidget *buildSidebar();
    QWidget *buildTopBar();

    QStackedWidget *m_stack = nullptr;
    QButtonGroup *m_navGroup = nullptr;
    QLineEdit *m_search = nullptr;
    EmployeePage *m_employeePage = nullptr;
};
