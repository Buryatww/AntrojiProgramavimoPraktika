#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <memory>
#include "models.h"
#include "repositories.h"

class LoginWindow : public QDialog {
    Q_OBJECT

public:
    explicit LoginWindow(DatabaseManager* db, QWidget *parent = nullptr);
    ~LoginWindow();
    
    std::shared_ptr<Naudotojas> getLoggedInUser() const { return loggedInUser; }

private slots:
    void onLoginClicked();

private:
    QLineEdit* usernameEdit;
    QLineEdit* passwordEdit;
    QPushButton* loginButton;
    QLabel* statusLabel;
    
    DatabaseManager* database;
    NaudotojasRepository* userRepo;
    std::shared_ptr<Naudotojas> loggedInUser;
};

#endif // LOGINWINDOW_H
