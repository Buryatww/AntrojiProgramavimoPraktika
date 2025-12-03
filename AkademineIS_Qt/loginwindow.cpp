#include "loginwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

LoginWindow::LoginWindow(DatabaseManager* db, QWidget *parent)
    : QDialog(parent), database(db), loggedInUser(nullptr)
{
    userRepo = new NaudotojasRepository(database);
    
    setWindowTitle("Prisijungimas - Akademine Sistema");
    setFixedSize(400, 250);
    
    QLabel* titleLabel = new QLabel("<h2>Akademinė Sistema</h2>");
    titleLabel->setAlignment(Qt::AlignCenter);
    
    QLabel* userLabel = new QLabel("Prisijungimo vardas:");
    usernameEdit = new QLineEdit();
    usernameEdit->setPlaceholderText("Iveskite varda");
    
    QLabel* passLabel = new QLabel("Slaptazodis:");
    passwordEdit = new QLineEdit();
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText("Iveskite slaptazodi");
    
    loginButton = new QPushButton("Prisijungti");
    loginButton->setDefault(true);
    
    statusLabel = new QLabel("");
    statusLabel->setStyleSheet("QLabel { color: red; }");
    statusLabel->setAlignment(Qt::AlignCenter);
    
    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(20);
    mainLayout->addWidget(userLabel);
    mainLayout->addWidget(usernameEdit);
    mainLayout->addWidget(passLabel);
    mainLayout->addWidget(passwordEdit);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(loginButton);
    mainLayout->addWidget(statusLabel);
    mainLayout->addStretch();
    
    setLayout(mainLayout);
    
    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(passwordEdit, &QLineEdit::returnPressed, this, &LoginWindow::onLoginClicked);
}

LoginWindow::~LoginWindow() {
    delete userRepo;
}

void LoginWindow::onLoginClicked() {
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text().trimmed();
    
    qDebug() << "Login attempt - Username:" << username << "Password:" << password;
    
    if (username.isEmpty() || password.isEmpty()) {
        statusLabel->setText("Uzpildykite visus laukus!");
        return;
    }
    
    try {
        loggedInUser = userRepo->authenticate(username, password);
        
        qDebug() << "Authentication result:" << (loggedInUser ? "SUCCESS" : "FAILED");
        
        if (loggedInUser) {
            qDebug() << "User role:" << loggedInUser->getRole();
            accept();
        } else {
            statusLabel->setText("Neteisingi prisijungimo duomenys!");
            passwordEdit->clear();
            usernameEdit->setFocus();
        }
    } catch (const Klaida& e) {
        qDebug() << "Exception during login:" << e.getZinute();
        QMessageBox::critical(this, "Klaida", e.getZinute());
    }
}
