#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include "models.h"
#include "repositories.h"

class AdminWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit AdminWindow(DatabaseManager* db, Admin* admin, QWidget *parent = nullptr);
    ~AdminWindow();

signals:
    void logoutRequested();

private slots:

    void refreshNaudotojaiTable();
    void onAddNaudotojas();
    void onDeleteNaudotojas();
    
    void refreshGrupesTable();
    void onAddGrupe();
    void onDeleteGrupe();
    void onViewGrupeStudentai();
    void onViewGrupeDalykai();
    

    void refreshDalykaiTable();
    void onAddDalykas();
    void onDeleteDalykas();
    void onAssignDalykasToDestytojas();
    void onAssignDalykasToGrupe();
    void onUnassignDalykasFromDestytojas();
    void onUnassignDalykasFromGrupe();
    

    void refreshPazymiaiTable();
    void onEditPazymys();
    void onDeletePazymys();
    

    void onLogout();

private:
    void setupUI();
    void setupNaudotojaiTab();
    void setupGrupesTab();
    void setupDalykaiTab();
    void setupPazymiaiTab();
    
    DatabaseManager* database;
    Admin* currentAdmin;
    

    NaudotojasRepository* userRepo;
    GrupeRepository* grupeRepo;
    DalykasRepository* dalykasRepo;
    PazymysRepository* pazymysRepo;
    DestytojaRepository* destytojaRepo;
    StudentasRepository* studentasRepo;
    

    QTabWidget* tabWidget;
    QPushButton* btnLogout;
    

    QTableWidget* naudotojaiTable;
    QPushButton* btnAddNaudotojas;
    QPushButton* btnDeleteNaudotojas;
    

    QTableWidget* grupesTable;
    QPushButton* btnAddGrupe;
    QPushButton* btnDeleteGrupe;
    QPushButton* btnViewGrupeStudentai;
    QPushButton* btnViewGrupeDalykai;
    

    QTableWidget* dalykaiTable;
    QPushButton* btnAddDalykas;
    QPushButton* btnDeleteDalykas;
    QPushButton* btnAssignToDestytojas;
    QPushButton* btnAssignToGrupe;
    QPushButton* btnUnassignFromDestytojas;
    QPushButton* btnUnassignFromGrupe;
    

    QTableWidget* pazymiaiTable;
    QPushButton* btnEditPazymys;
    QPushButton* btnDeletePazymys;
};

#endif // ADMINWINDOW_H
