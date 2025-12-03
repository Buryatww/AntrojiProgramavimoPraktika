#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include "loginwindow.h"
#include "adminwindow.h"
#include "destytojawindow.h"
#include "studentwindow.h"
#include "models.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    QString dbPath = "akademine.db";
    
    if (!QFileInfo::exists(dbPath)) {
        qDebug() << "Database not found in current directory:" << QDir::currentPath();
        
        QMessageBox::warning(nullptr, "Database not found",
                           QString("Database file 'akademine.db' not found in:\n%1\n\n"
                                  "Please select the database file.")
                           .arg(QDir::currentPath()));
        
        dbPath = QFileDialog::getOpenFileName(nullptr,
                                             "Select Database File",
                                             QDir::currentPath(),
                                             "Database Files (*.db);;All Files (*)");
        
        if (dbPath.isEmpty()) {
            return 0;
        }
    }
    
    qDebug() << "Using database:" << dbPath;
    
    DatabaseManager dbManager(dbPath);
    if (!dbManager.connect()) {
        QMessageBox::critical(nullptr, "Klaida", 
                            QString("Nepavyko prisijungti prie duomenu bazes!\nPath: %1").arg(dbPath));
        return 1;
    }
    
    qDebug() << "Database connected successfully!";
    
    bool running = true;
    while (running) {
        LoginWindow loginWindow(&dbManager);
        
        if (loginWindow.exec() != QDialog::Accepted) {
            return 0;
        }
        
        auto user = loginWindow.getLoggedInUser();
        
        if (!user) {
            QMessageBox::critical(nullptr, "Klaida", "Nepavyko autentifikuoti!");
            continue;
        }
        
        QString role = user->getRole();
        bool logoutRequested = false;
        
        if (role == "admin") {
            Admin* admin = dynamic_cast<Admin*>(user.get());
            if (admin) {
                AdminWindow* adminWindow = new AdminWindow(&dbManager, admin);
                
                QObject::connect(adminWindow, &AdminWindow::logoutRequested, [&]() {
                    logoutRequested = true;
                });
                
                adminWindow->show();
                app.exec();
                
                delete adminWindow;
                
                if (!logoutRequested) {
                    running = false;
                }
            }
        }
        else if (role == "destytojas") {
            Destytojas* destytojas = dynamic_cast<Destytojas*>(user.get());
            if (destytojas) {
                DestytojaWindow* destytojaWindow = new DestytojaWindow(&dbManager, destytojas);
                
                QObject::connect(destytojaWindow, &DestytojaWindow::logoutRequested, [&]() {
                    logoutRequested = true;
                });
                
                destytojaWindow->show();
                app.exec();
                
                delete destytojaWindow;
                
                if (!logoutRequested) {
                    running = false;
                }
            }
        }
        else if (role == "studentas") {
            Studentas* studentas = dynamic_cast<Studentas*>(user.get());
            if (studentas) {
                StudentWindow* studentWindow = new StudentWindow(&dbManager, studentas);
                
                QObject::connect(studentWindow, &StudentWindow::logoutRequested, [&]() {
                    logoutRequested = true;
                });
                
                studentWindow->show();
                app.exec();
                
                delete studentWindow;
                
                if (!logoutRequested) {
                    running = false;
                }
            }
        }
        else {
            QMessageBox::critical(nullptr, "Klaida", "Neatpazinta role!");
            continue;
        }
    }
    
    return 0;
}
