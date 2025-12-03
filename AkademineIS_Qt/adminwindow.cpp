#include "adminwindow.h"
#include "exceptions.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QGroupBox>

AdminWindow::AdminWindow(DatabaseManager* db, Admin* admin, QWidget *parent)
    : QMainWindow(parent), database(db), currentAdmin(admin)
{
    userRepo = new NaudotojasRepository(database);
    grupeRepo = new GrupeRepository(database);
    dalykasRepo = new DalykasRepository(database);
    pazymysRepo = new PazymysRepository(database);
    destytojaRepo = new DestytojaRepository(database);
    studentasRepo = new StudentasRepository(database);
    
    setupUI();
    setWindowTitle(QString("Admin Panel - %1 %2").arg(admin->getVardas()).arg(admin->getPavarde()));
    resize(1000, 700);
}

AdminWindow::~AdminWindow() {
    delete userRepo;
    delete grupeRepo;
    delete dalykasRepo;
    delete pazymysRepo;
    delete destytojaRepo;
    delete studentasRepo;
}

void AdminWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    btnLogout = new QPushButton("Atsijungti");
    btnLogout->setMaximumWidth(150);
    btnLogout->setStyleSheet("QPushButton { background-color: #ff6b6b; color: white; padding: 8px; }");
    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addStretch();
    topLayout->addWidget(btnLogout);
    mainLayout->addLayout(topLayout);
    
    tabWidget = new QTabWidget();
    
    setupNaudotojaiTab();
    setupGrupesTab();
    setupDalykaiTab();
    setupPazymiaiTab();
    
    mainLayout->addWidget(tabWidget);
    
    connect(btnLogout, &QPushButton::clicked, this, &AdminWindow::onLogout);
    
    setCentralWidget(centralWidget);
}

void AdminWindow::setupNaudotojaiTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);
    
    QLabel* title = new QLabel("<h3>Naudotojai</h3>");
    layout->addWidget(title);
    
    naudotojaiTable = new QTableWidget();
    naudotojaiTable->setColumnCount(4);
    naudotojaiTable->setHorizontalHeaderLabels({"ID", "Vardas", "Pavarde", "Role"});
    naudotojaiTable->horizontalHeader()->setStretchLastSection(true);
    naudotojaiTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    naudotojaiTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(naudotojaiTable);
    
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnAddNaudotojas = new QPushButton("Prideti naudotoja");
    btnDeleteNaudotojas = new QPushButton("Istrinti naudotoja");
    
    btnLayout->addWidget(btnAddNaudotojas);
    btnLayout->addWidget(btnDeleteNaudotojas);
    btnLayout->addStretch();
    layout->addLayout(btnLayout);
    
    connect(btnAddNaudotojas, &QPushButton::clicked, this, &AdminWindow::onAddNaudotojas);
    connect(btnDeleteNaudotojas, &QPushButton::clicked, this, &AdminWindow::onDeleteNaudotojas);
    
    tabWidget->addTab(tab, "Naudotojai");
    refreshNaudotojaiTable();
}

void AdminWindow::setupGrupesTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);
    
    QLabel* title = new QLabel("<h3>Grupes</h3>");
    layout->addWidget(title);
    
    grupesTable = new QTableWidget();
    grupesTable->setColumnCount(2);
    grupesTable->setHorizontalHeaderLabels({"ID", "Pavadinimas"});
    grupesTable->horizontalHeader()->setStretchLastSection(true);
    grupesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    grupesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(grupesTable);
    
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnAddGrupe = new QPushButton("Prideti grupe");
    btnDeleteGrupe = new QPushButton("Istrinti grupe");
    btnViewGrupeStudentai = new QPushButton("Perziureti grupes studentus");
    btnViewGrupeDalykai = new QPushButton("Perziureti grupes dalykus");
    
    btnLayout->addWidget(btnAddGrupe);
    btnLayout->addWidget(btnDeleteGrupe);
    btnLayout->addWidget(btnViewGrupeStudentai);
    btnLayout->addWidget(btnViewGrupeDalykai);
    btnLayout->addStretch();
    layout->addLayout(btnLayout);
    
    connect(btnAddGrupe, &QPushButton::clicked, this, &AdminWindow::onAddGrupe);
    connect(btnDeleteGrupe, &QPushButton::clicked, this, &AdminWindow::onDeleteGrupe);
    connect(btnViewGrupeStudentai, &QPushButton::clicked, this, &AdminWindow::onViewGrupeStudentai);
    connect(btnViewGrupeDalykai, &QPushButton::clicked, this, &AdminWindow::onViewGrupeDalykai);
    
    tabWidget->addTab(tab, "Grupes");
    refreshGrupesTable();
}

void AdminWindow::setupDalykaiTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);
    
    QLabel* title = new QLabel("<h3>Dalykai</h3>");
    layout->addWidget(title);
    
    dalykaiTable = new QTableWidget();
    dalykaiTable->setColumnCount(3);
    dalykaiTable->setHorizontalHeaderLabels({"ID", "Pavadinimas", "Destytojas"});
    dalykaiTable->horizontalHeader()->setStretchLastSection(true);
    dalykaiTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    dalykaiTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(dalykaiTable);
    
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnAddDalykas = new QPushButton("Prideti dalyka");
    btnDeleteDalykas = new QPushButton("Istrinti dalyka");
    btnAssignToDestytojas = new QPushButton("Priskirti destytojui");
    btnAssignToGrupe = new QPushButton("Priskirti grupei");
    btnUnassignFromDestytojas = new QPushButton("Atskirti nuo destytojo");
    btnUnassignFromGrupe = new QPushButton("Atskirti nuo grupes");
    
    btnLayout->addWidget(btnAddDalykas);
    btnLayout->addWidget(btnDeleteDalykas);
    btnLayout->addWidget(btnAssignToDestytojas);
    btnLayout->addWidget(btnAssignToGrupe);
    btnLayout->addWidget(btnUnassignFromDestytojas);
    btnLayout->addWidget(btnUnassignFromGrupe);
    btnLayout->addStretch();
    layout->addLayout(btnLayout);
    
    connect(btnAddDalykas, &QPushButton::clicked, this, &AdminWindow::onAddDalykas);
    connect(btnDeleteDalykas, &QPushButton::clicked, this, &AdminWindow::onDeleteDalykas);
    connect(btnAssignToDestytojas, &QPushButton::clicked, this, &AdminWindow::onAssignDalykasToDestytojas);
    connect(btnAssignToGrupe, &QPushButton::clicked, this, &AdminWindow::onAssignDalykasToGrupe);
    connect(btnUnassignFromDestytojas, &QPushButton::clicked, this, &AdminWindow::onUnassignDalykasFromDestytojas);
    connect(btnUnassignFromGrupe, &QPushButton::clicked, this, &AdminWindow::onUnassignDalykasFromGrupe);
    tabWidget->addTab(tab, "Dalykai");
    refreshDalykaiTable();
}

void AdminWindow::setupPazymiaiTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);
    
    QLabel* title = new QLabel("<h3>Pazymiai</h3>");
    layout->addWidget(title);
    
    pazymiaiTable = new QTableWidget();
    pazymiaiTable->setColumnCount(6);
    pazymiaiTable->setHorizontalHeaderLabels({"ID", "Studentas", "Dalyko pav.", "Reiksme", "Data", "Destytojas"});
    pazymiaiTable->horizontalHeader()->setStretchLastSection(true);
    pazymiaiTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    pazymiaiTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(pazymiaiTable);
    
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnEditPazymys = new QPushButton("Redaguoti pazymi");
    btnDeletePazymys = new QPushButton("Istrinti pazymi");
    
    btnLayout->addWidget(btnEditPazymys);
    btnLayout->addWidget(btnDeletePazymys);
    btnLayout->addStretch();
    layout->addLayout(btnLayout);
    
    connect(btnEditPazymys, &QPushButton::clicked, this, &AdminWindow::onEditPazymys);
    connect(btnDeletePazymys, &QPushButton::clicked, this, &AdminWindow::onDeletePazymys);
    
    tabWidget->addTab(tab, "Pazymiai");
    refreshPazymiaiTable();
}


void AdminWindow::refreshNaudotojaiTable() {
    try {
        QVector<Naudotojas> users = userRepo->findAll();
        naudotojaiTable->setRowCount(users.size());
        
        for (int i = 0; i < users.size(); i++) {
            naudotojaiTable->setItem(i, 0, new QTableWidgetItem(QString::number(users[i].getId())));
            naudotojaiTable->setItem(i, 1, new QTableWidgetItem(users[i].getVardas()));
            naudotojaiTable->setItem(i, 2, new QTableWidgetItem(users[i].getPavarde()));
            naudotojaiTable->setItem(i, 3, new QTableWidgetItem(users[i].getRole()));
        }
    } catch (const Klaida& e) {
        QMessageBox::warning(this, "Klaida", e.getZinute());
    }
}

void AdminWindow::onAddNaudotojas() {
    bool ok;
    QString vardas = QInputDialog::getText(this, "Naujas naudotojas", 
                                          "Vardas:", QLineEdit::Normal, "", &ok);
    if (!ok || vardas.isEmpty()) return;
    
    QString pavarde = QInputDialog::getText(this, "Naujas naudotojas", 
                                           "Pavarde:", QLineEdit::Normal, "", &ok);
    if (!ok || pavarde.isEmpty()) return;
    
    QStringList roles = {"admin", "destytojas", "studentas"};
    QString role = QInputDialog::getItem(this, "Naujas naudotojas", 
                                         "Role:", roles, 0, false, &ok);
    if (!ok) return;
    
    try {

        Naudotojas naujas;
        

        naujas.setVardas(vardas);
        naujas.setPavarde(pavarde);
        naujas.setRole(role);
        
        int newUserId = userRepo->save(naujas);
        
        if (role == "destytojas") {
            destytojaRepo->createDestytojas(newUserId);
        } else if (role == "studentas") {
            auto grupes = grupeRepo->findAll();
            if (grupes.isEmpty()) {
                QMessageBox::warning(this, "Perspejimas", "Nera sukurtu grupiu!");
                return;
            }
            
            QStringList grupeNames;
            QVector<int> grupeIds;
            for (const auto& g : grupes) {
                grupeNames << g.getPavadinimas();
                grupeIds << g.getId();
            }
            
            QString grupePav = QInputDialog::getItem(this, "Pasirinkite grupe", 
                                                     "Grupe:", grupeNames, 0, false, &ok);
            if (ok) {
                int idx = grupeNames.indexOf(grupePav);
                studentasRepo->createStudentas(newUserId, grupeIds[idx]);
            }
        }
        
        QMessageBox::information(this, "Sekme", "Naudotojas pridetas sekmingai!");
        refreshNaudotojaiTable();
    } catch (const Klaida& e) {
        QMessageBox::critical(this, "Klaida", e.getZinute());
    }
}

void AdminWindow::onDeleteNaudotojas() {
    int row = naudotojaiTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Perspejimas", "Pasirinkite naudotoja!");
        return;
    }
    
    int id = naudotojaiTable->item(row, 0)->text().toInt();
    
    auto reply = QMessageBox::question(this, "Patvirtinimas", 
                                       "Ar tikrai norite istrinti si naudotoja?",
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if (userRepo->remove(id)) {
            QMessageBox::information(this, "Sekme", "Naudotojas istrintas!");
            refreshNaudotojaiTable();
        } else {
            QMessageBox::critical(this, "Klaida", "Nepavyko istrinti naudotojo!");
        }
    }
}

void AdminWindow::refreshGrupesTable() {
    try {
        QVector<Grupe> grupes = grupeRepo->findAll();
        grupesTable->setRowCount(grupes.size());
        
        for (int i = 0; i < grupes.size(); i++) {
            grupesTable->setItem(i, 0, new QTableWidgetItem(QString::number(grupes[i].getId())));
            grupesTable->setItem(i, 1, new QTableWidgetItem(grupes[i].getPavadinimas()));
        }
    } catch (const Klaida& e) {
        QMessageBox::warning(this, "Klaida", e.getZinute());
    }
}

void AdminWindow::onAddGrupe() {
    bool ok;
    QString pavadinimas = QInputDialog::getText(this, "Nauja grupe", 
                                               "Pavadinimas:", QLineEdit::Normal, "", &ok);
    if (ok && !pavadinimas.isEmpty()) {
        try {

            Grupe nauja;
            nauja.setPavadinimas(pavadinimas);
            
            grupeRepo->save(nauja);
            QMessageBox::information(this, "Sekme", "Grupe prideta sekmingai!");
            refreshGrupesTable();
        } catch (const Klaida& e) {
            QMessageBox::critical(this, "Klaida", e.getZinute());
        }
    }
}

void AdminWindow::onDeleteGrupe() {
    int row = grupesTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Perspejimas", "Pasirinkite grupe!");
        return;
    }
    
    int id = grupesTable->item(row, 0)->text().toInt();
    
    auto reply = QMessageBox::question(this, "Patvirtinimas", 
                                       "Ar tikrai norite istrinti sia grupe?",
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if (grupeRepo->remove(id)) {
            QMessageBox::information(this, "Sekme", "Grupe istrinta!");
            refreshGrupesTable();
        } else {
            QMessageBox::critical(this, "Klaida", "Nepavyko istrinti grupes!");
        }
    }
}

void AdminWindow::refreshDalykaiTable() {
    try {
        QVector<Dalykas> dalykai = dalykasRepo->findAll();
        dalykaiTable->setRowCount(dalykai.size());
        
        for (int i = 0; i < dalykai.size(); i++) {
            dalykaiTable->setItem(i, 0, new QTableWidgetItem(QString::number(dalykai[i].getId())));
            dalykaiTable->setItem(i, 1, new QTableWidgetItem(dalykai[i].getPavadinimas()));

            auto destytojai = dalykasRepo->getDestytojai(dalykai[i].getId());
            QString destytojaiText = "";
            if (!destytojai.isEmpty()) {
                QStringList vardai;
                for (const auto& dest : destytojai) {
                    vardai << dest.second;
                }
                destytojaiText = vardai.join(", ");
            } else {
                destytojaiText = "-";
            }
            dalykaiTable->setItem(i, 2, new QTableWidgetItem(destytojaiText));
        }
    } catch (const Klaida& e) {
        QMessageBox::warning(this, "Klaida", e.getZinute());
    }
}

void AdminWindow::onAddDalykas() {
    bool ok;
    QString pavadinimas = QInputDialog::getText(this, "Naujas dalykas", 
                                               "Pavadinimas:", QLineEdit::Normal, "", &ok);
    if (ok && !pavadinimas.isEmpty()) {
        try {

            Dalykas naujas;
            naujas.setPavadinimas(pavadinimas);
            
            dalykasRepo->save(naujas);
            QMessageBox::information(this, "Sekme", "Dalykas pridetas sekmingai!");
            refreshDalykaiTable();
        } catch (const Klaida& e) {
            QMessageBox::critical(this, "Klaida", e.getZinute());
        }
    }
}

void AdminWindow::onDeleteDalykas() {
    int row = dalykaiTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Perspejimas", "Pasirinkite dalyka!");
        return;
    }
    
    int id = dalykaiTable->item(row, 0)->text().toInt();
    
    auto reply = QMessageBox::question(this, "Patvirtinimas", 
                                       "Ar tikrai norite istrinti si dalyka?",
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if (dalykasRepo->remove(id)) {
            QMessageBox::information(this, "Sekme", "Dalykas istrintas!");
            refreshDalykaiTable();
        } else {
            QMessageBox::critical(this, "Klaida", "Nepavyko istrinti dalyko!");
        }
    }
}

void AdminWindow::onAssignDalykasToDestytojas() {
    int row = dalykaiTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Perspejimas", "Pasirinkite dalyka!");
        return;
    }
    
    int dalyko_id = dalykaiTable->item(row, 0)->text().toInt();
    
    auto destytojai = destytojaRepo->getAll();
    if (destytojai.isEmpty()) {
        QMessageBox::warning(this, "Perspejimas", "Nera destytoju!");
        return;
    }
    
    QStringList destytojaNames;
    QVector<int> destytojaIds;
    for (const auto& d : destytojai) {
        destytojaNames << d.second;
        destytojaIds << d.first;
    }
    
    bool ok;
    QString destName = QInputDialog::getItem(this, "Pasirinkite destytoja", 
                                             "Destytojas:", destytojaNames, 0, false, &ok);
    if (ok) {
        int idx = destytojaNames.indexOf(destName);
        if (dalykasRepo->assignToDestytojas(dalyko_id, destytojaIds[idx])) {
            QMessageBox::information(this, "Sekme", "Dalykas priskirtas destytojui!");
            refreshDalykaiTable();
        } else {
            QMessageBox::critical(this, "Klaida", "Nepavyko priskirti dalyko!");
        }
    }
}

void AdminWindow::onAssignDalykasToGrupe() {
    int row = dalykaiTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Perspejimas", "Pasirinkite dalyka!");
        return;
    }
    
    int dalyko_id = dalykaiTable->item(row, 0)->text().toInt();
    
    auto grupes = grupeRepo->findAll();
    if (grupes.isEmpty()) {
        QMessageBox::warning(this, "Perspejimas", "Nera grupiu!");
        return;
    }
    
    QStringList grupeNames;
    QVector<int> grupeIds;
    for (const auto& g : grupes) {
        grupeNames << g.getPavadinimas();
        grupeIds << g.getId();
    }
    
    bool ok;
    QString grupePav = QInputDialog::getItem(this, "Pasirinkite grupe", 
                                             "Grupe:", grupeNames, 0, false, &ok);
    if (ok) {
        int idx = grupeNames.indexOf(grupePav);
        if (dalykasRepo->assignToGrupe(dalyko_id, grupeIds[idx])) {
            QMessageBox::information(this, "Sekme", "Dalykas priskirtas grupei!");
            refreshDalykaiTable();
        } else {
            QMessageBox::critical(this, "Klaida", "Nepavyko priskirti dalyko!");
        }
    }
}

void AdminWindow::onUnassignDalykasFromDestytojas() {
    int row = dalykaiTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Perspejimas", "Pasirinkite dalyka is lenteles!");
        return;
    }
    
    int dalyko_id = dalykaiTable->item(row, 0)->text().toInt();
    QString dalyko_pav = dalykaiTable->item(row, 1)->text();
    
    auto destytojai = dalykasRepo->getDestytojai(dalyko_id);
    if (destytojai.isEmpty()) {
        QMessageBox::warning(this, "Perspejimas", 
            QString("Dalykas \"%1\" nepriskirtas jokiam destytojui!").arg(dalyko_pav));
        return;
    }
    int destytojo_id = destytojai[0].first;
    QString destytojoVardas = destytojai[0].second;
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Patvirtinti atskyrimą", 
        QString("Ar tikrai norite atskirti dalyka \"%1\" nuo destytojo %2?")
            .arg(dalyko_pav)
            .arg(destytojoVardas),
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        if (dalykasRepo->unassignFromDestytojas(dalyko_id, destytojo_id)) {
            QMessageBox::information(this, "Sekme", "Dalykas atskirtas nuo destytojo!");
            refreshDalykaiTable();
        } else {
            QMessageBox::critical(this, "Klaida", "Nepavyko atskirti dalyko!");
        }
    }
}

void AdminWindow::onUnassignDalykasFromGrupe() {
    int row = dalykaiTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Perspejimas", "Pasirinkite dalyka is lenteles!");
        return;
    }
    
    int dalyko_id = dalykaiTable->item(row, 0)->text().toInt();
    QString dalyko_pav = dalykaiTable->item(row, 1)->text();
    
    auto grupes = grupeRepo->getGrupesByDalykas(dalyko_id);
    if (grupes.isEmpty()) {
        QMessageBox::warning(this, "Perspejimas", 
            QString("Dalykas \"%1\" nepriskirtas jokiai grupei!").arg(dalyko_pav));
        return;
    }
    
    QStringList grupeNames;
    QVector<int> grupeIds;
    for (const auto& g : grupes) {
        grupeNames << g.getPavadinimas();
        grupeIds << g.getId();
    }
    
    bool ok;
    QString grupePav = QInputDialog::getItem(this, "Pasirinkite grupe", 
        QString("Dalykas: %1\n\nPasirinkite grupe atskirimui:").arg(dalyko_pav),
        grupeNames, 0, false, &ok);
    
    if (ok) {
        int idx = grupeNames.indexOf(grupePav);
        int grupes_id = grupeIds[idx];
        
        if (dalykasRepo->unassignFromGrupe(dalyko_id, grupes_id)) {
            QMessageBox::information(this, "Sekme", "Dalykas atskirtas nuo grupes!");
            refreshDalykaiTable();
        } else {
            QMessageBox::critical(this, "Klaida", "Nepavyko atskirti dalyko!");
        }
    }
}

void AdminWindow::refreshPazymiaiTable() {
    try {
        QVector<Pazymys> pazymiai = pazymysRepo->findAll();
        pazymiaiTable->setRowCount(pazymiai.size());
        
        for (int i = 0; i < pazymiai.size(); i++) {
            pazymiaiTable->setItem(i, 0, new QTableWidgetItem(QString::number(pazymiai[i].getId())));
            
            QString studentas = studentasRepo->getStudentoVardasPavarde(pazymiai[i].getStudentoId());
            pazymiaiTable->setItem(i, 1, new QTableWidgetItem(studentas));
            
            try {
                Dalykas dalykas = dalykasRepo->findById(pazymiai[i].getDalykoId());
                pazymiaiTable->setItem(i, 2, new QTableWidgetItem(dalykas.getPavadinimas()));
            } catch(...) {
                pazymiaiTable->setItem(i, 2, new QTableWidgetItem("Nezinomas"));
            }
            
            pazymiaiTable->setItem(i, 3, new QTableWidgetItem(QString::number(pazymiai[i].getReiksme())));
            pazymiaiTable->setItem(i, 4, new QTableWidgetItem(pazymiai[i].getData()));
            
            QString destVardas = pazymysRepo->getDestytojoVardas(pazymiai[i].getDestytojoId());
            pazymiaiTable->setItem(i, 5, new QTableWidgetItem(destVardas));
        }
    } catch (const Klaida& e) {
        QMessageBox::warning(this, "Klaida", e.getZinute());
    }
}

void AdminWindow::onEditPazymys() {
    int row = pazymiaiTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Perspejimas", "Pasirinkite pazymi!");
        return;
    }
    
    int pazymio_id = pazymiaiTable->item(row, 0)->text().toInt();
    
    try {
        Pazymys pazymys = pazymysRepo->findById(pazymio_id);
        
        if (pazymys.getId() == 0) {
            QMessageBox::warning(this, "Klaida", "Pazymys nerastas!");
            return;
        }
        
        bool ok;
        int nauja_reiksme = QInputDialog::getInt(this, "Redaguoti pazymi", 
                                                 "Nauja reiksme (1-10):", 
                                                 pazymys.getReiksme(), 1, 10, 1, &ok);
        if (!ok) return;
        
        QString nauja_data = QInputDialog::getText(this, "Redaguoti pazymi", 
                                                   "Nauja data (MM-DD-YYYY):", 
                                                   QLineEdit::Normal, pazymys.getData(), &ok);
        if (!ok) return;
        
        pazymys.setReiksme(nauja_reiksme);
        pazymys.setData(nauja_data);
        
        if (pazymysRepo->update(pazymys)) {
            QMessageBox::information(this, "Sekme", "Pazymys pakeistas sekmingai!");
            refreshPazymiaiTable();
        } else {
            QMessageBox::critical(this, "Klaida", "Nepavyko pakeisti pazymio!");
        }
    } catch (const Klaida& e) {
        QMessageBox::critical(this, "Klaida", e.getZinute());
    }
}

void AdminWindow::onDeletePazymys() {
    int row = pazymiaiTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Perspejimas", "Pasirinkite pazymi is lenteles!");
        return;
    }
    
    int pazymio_id = pazymiaiTable->item(row, 0)->text().toInt();
    QString studentas = pazymiaiTable->item(row, 1)->text();
    QString dalyko_pav = pazymiaiTable->item(row, 2)->text();
    QString reiksme = pazymiaiTable->item(row, 3)->text();
    QString data = pazymiaiTable->item(row, 4)->text();
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Patvirtinti istrinima", 
        QString("Ar tikrai norite istrinti si pazymi?\n\n"
                "Pazymio ID: %1\n"
                "Studentas: %2\n"
                "Dalykas: %3\n"
                "Reiksme: %4\n"
                "Data: %5")
            .arg(pazymio_id)
            .arg(studentas)
            .arg(dalyko_pav)
            .arg(reiksme)
            .arg(data),
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        try {
            if (pazymysRepo->remove(pazymio_id)) {
                QMessageBox::information(this, "Sekme", "Pazymys istrintas sekmingai!");
                refreshPazymiaiTable();
            } else {
                QMessageBox::critical(this, "Klaida", "Nepavyko istrinti pazymio!");
            }
        } catch (const Klaida& e) {
            QMessageBox::critical(this, "Klaida", e.getZinute());
        }
    }
}

void AdminWindow::onViewGrupeStudentai() {
    int row = grupesTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Perspejimas", "Pasirinkite grupe is lenteles!");
        return;
    }
    
    try {
        int grupes_id = grupesTable->item(row, 0)->text().toInt();
        QString grupes_pav = grupesTable->item(row, 1)->text();
        
        auto studentai = studentasRepo->getStudentsByGrupe(grupes_id);
        
        if (studentai.isEmpty()) {
            QMessageBox::information(this, "Info", 
                QString("Grupeje \"%1\" studentu nera.").arg(grupes_pav));
            return;
        }
        
        QString message = QString("Grupe: %1\n\nStudentai:\n\n").arg(grupes_pav);
        for (const auto& pair : studentai) {
            message += QString("ID: %1 | %2\n").arg(pair.first).arg(pair.second);
        }
        
        QMessageBox::information(this, "Grupes studentai", message);
        
    } catch (const Klaida& e) {
        QMessageBox::critical(this, "Klaida", e.getZinute());
    }
}

void AdminWindow::onViewGrupeDalykai() {
    int row = grupesTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Perspejimas", "Pasirinkite grupe is lenteles!");
        return;
    }
    
    try {
        int grupes_id = grupesTable->item(row, 0)->text().toInt();
        QString grupes_pav = grupesTable->item(row, 1)->text();
        
        auto dalykai = grupeRepo->getDalykai(grupes_id);
        
        if (dalykai.isEmpty()) {
            QMessageBox::information(this, "Info", 
                QString("Grupeje \"%1\" dalyku nera.").arg(grupes_pav));
            return;
        }
        
        QString message = QString("Grupe: %1\n\nDalykai:\n\n").arg(grupes_pav);
        for (const auto& dalykas : dalykai) {
            message += QString("ID: %1 | %2\n").arg(dalykas.getId()).arg(dalykas.getPavadinimas());
        }
        
        QMessageBox::information(this, "Grupes dalykai", message);
        
    } catch (const Klaida& e) {
        QMessageBox::critical(this, "Klaida", e.getZinute());
    }
}

void AdminWindow::onLogout() {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Atsijungti", "Ar tikrai norite atsijungti?",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        emit logoutRequested();
        close();
    }
}
