#ifndef STUDENTWINDOW_H
#define STUDENTWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include "models.h"
#include "repositories.h"

class StudentWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit StudentWindow(DatabaseManager* db, Studentas* studentas, QWidget *parent = nullptr);
    ~StudentWindow();

signals:
    void logoutRequested();

private slots:
    void onViewMyPazymiai();
    void refreshDalykaiList();
    void onLogout();

private:
    void setupUI();
    
    DatabaseManager* database;
    Studentas* currentStudentas;
    
    GrupeRepository* grupeRepo;
    DalykasRepository* dalykasRepo;
    PazymysRepository* pazymysRepo;
    
    QTableWidget* dalykaiTable;
    QPushButton* btnViewPazymiai;
    QPushButton* btnLogout;
};

#endif // STUDENTWINDOW_H
