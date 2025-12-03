#ifndef DESTYTOJAWINDOW_H
#define DESTYTOJAWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QComboBox>
#include <QTabWidget>
#include "models.h"
#include "repositories.h"

class DestytojaWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit DestytojaWindow(DatabaseManager* db, Destytojas* destytojas, QWidget *parent = nullptr);
    ~DestytojaWindow();

signals:
    void logoutRequested();

private slots:
    void onAddPazymys();
    void onViewPazymiai();
    void onEditPazymys();
    void refreshMyDalykaiList();
    void onLogout();

private:
    void setupUI();
    
    DatabaseManager* database;
    Destytojas* currentDestytojas;
    
    GrupeRepository* grupeRepo;
    DalykasRepository* dalykasRepo;
    PazymysRepository* pazymysRepo;
    StudentasRepository* studentasRepo;
    

    QTableWidget* dalykaiTable;
    QPushButton* btnAddPazymys;
    QPushButton* btnViewPazymiai;
    QPushButton* btnEditPazymys;
    QPushButton* btnLogout;
};

#endif // DESTYTOJAWINDOW_H
