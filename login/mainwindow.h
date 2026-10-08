#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QPixmap>

class QStackedWidget;
class QLineEdit;
class QPushButton;
class QVBoxLayout;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    // Appelée automatiquement par Qt pour dessiner la fenêtre
    void paintEvent(QPaintEvent *event) override;

private slots:
    void showLoginPage();
    void showRegisterPage();
    void onLoginClicked();
    void onRegisterClicked();
    void onForgotPasswordClicked();
    void onFaceIdClicked();

private:
    // Fonctions qui construisent l'interface
    QWidget *createHeader();
    QWidget *createLoginPage();
    QWidget *createRegisterPage();
    QLineEdit *addField(QVBoxLayout *layout, const QString &labelText,
                        const QString &iconPath, bool isPassword);
    QPushButton *createFaceIdButton();
    QPushButton *createLinkButton(const QString &text);

    QPixmap m_background;
    QStackedWidget *stackedWidget;

    // Champs de la page Login
    QLineEdit *loginIdEdit;
    QLineEdit *loginPasswordEdit;

    // Champs de la page Inscription
    QLineEdit *registerIdEdit;
    QLineEdit *registerPasswordEdit;
    QLineEdit *registerConfirmEdit;
};

#endif // MAINWINDOW_H
