#include <QApplication>
#include "mainwindow.h"

// Le style est écrit directement dans le code
static const char *kStyleSheet = R"(
/* ===== Carte glassmorphism ===== */
#card {
    background-color: rgba(255, 255, 255, 150);
    border: 1px solid rgba(255, 255, 255, 200);
    border-radius: 28px;
}
#stackedWidget, #loginPage, #registerPage {
    background: transparent;
}

/* ===== Textes ===== */
QLabel {
    background: transparent;
    color: #1B2A6B;
    font-family: "Inter", "Segoe UI", sans-serif;
    font-size: 13px;
}
#titleLabel   { color: #0F6CB6; font-size: 24px; font-weight: bold; }
#welcomeLabel { font-size: 18px; font-weight: 600; margin-bottom: 4px; }
#fieldLabel   { font-weight: bold; margin-top: 4px; }

/* ===== Champs de saisie ===== */
QLineEdit {
    background-color: rgba(255, 255, 255, 235);
    border: 1px solid rgba(15, 108, 182, 60);
    border-radius: 10px;
    padding: 4px 10px;
    color: #1B2A6B;
    font-size: 14px;
}
QLineEdit:hover { border: 1px solid rgba(15, 108, 182, 150); }
QLineEdit:focus { border: 2px solid #0F6CB6; }

/* ===== Bouton principal ===== */
QPushButton#primaryButton {
    background-color: #0F6CB6;
    color: white;
    border: none;
    border-radius: 10px;
    font-size: 15px;
    font-weight: bold;
}
QPushButton#primaryButton:hover   { background-color: #1380D6; }
QPushButton#primaryButton:pressed { background-color: #0B5590; }

/* ===== Bouton Face ID ===== */
QPushButton#faceIdButton {
    background-color: rgba(255, 255, 255, 235);
    color: #1B2A6B;
    border: 1px solid rgba(15, 108, 182, 60);
    border-radius: 10px;
    font-size: 14px;
}
QPushButton#faceIdButton:hover { background-color: white; }

/* ===== Liens ===== */
QPushButton#linkButton {
    background: transparent;
    border: none;
    color: #1B2A6B;
    font-size: 13px;
    padding: 4px;
}
QPushButton#linkButton:hover { color: #0F6CB6; text-decoration: underline; }

QPushButton#forgotButton { color: #D23B3B; font-size: 11px; }
QPushButton#forgotButton:hover { color: #A82020; }
)";

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setStyleSheet(QString::fromUtf8(kStyleSheet));

    MainWindow window;
    window.show();

    return app.exec();
}
