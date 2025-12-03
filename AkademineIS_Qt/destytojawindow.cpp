#include "destytojawindow.h"
#include "exceptions.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QInputDialog>
#include <QLabel>
#include <QHeaderView>
#include <QGroupBox>

DestytojaWindow::DestytojaWindow(DatabaseManager* db, Destytojas* destytojas, QWidget *parent)
    : QMainWindow(parent), database(db), currentDestytojas(destytojas)
{
    grupeRepo = new GrupeRepository(database);
    dalykasRepo = new DalykasRepository(database);
    pazymysRepo = new PazymysRepository(database);
    studentasRepo = new StudentasRepository(database);
    
    setupUI();
    setWindowTitle(QString("Destytojo Panel - %1 %2").arg(destytojas->getVardas()).arg(destytojas->getPavarde()));
    resize(900, 600);
}

DestytojaWindow::~DestytojaWindow() {
    delete grupeRepo;
    delete dalykasRepo;
    delete pazymysRepo;
    delete studentasRepo;
}

void DestytojaWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    

    QLabel* title = new QLabel("<h2>Destytojo Meniu</h2>");
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);
    
    btnLogout = new QPushButton("Atsijungti");
    btnLogout->setMaximumWidth(150);
    btnLogout->setStyleSheet("QPushButton { background-color: #ff6b6b; color: white; padding: 8px; }");
    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addStretch();
    topLayout->addWidget(btnLogout);
    mainLayout->addLayout(topLayout);
    
    QLabel* welcomeLabel = new QLabel(QString("Sveiki, %1 %2!")
                                      .arg(currentDestytojas->getVardas())
                                      .arg(currentDestytojas->getPavarde()));
    welcomeLabel->setStyleSheet("QLabel { font-size: 14pt; color: #0066cc; margin: 10px; }");
    mainLayout->addWidget(welcomeLabel);
    
    QLabel* dalykaiLabel = new QLabel("<h3>Mano dalykai:</h3>");
    mainLayout->addWidget(dalykaiLabel);
    
    dalykaiTable = new QTableWidget();
    dalykaiTable->setColumnCount(2);
    dalykaiTable->setHorizontalHeaderLabels({"ID", "Pavadinimas"});
    dalykaiTable->horizontalHeader()->setStretchLastSection(true);
    dalykaiTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    dalykaiTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(dalykaiTable);
    
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnAddPazymys = new QPushButton("Ivesti nauja pazymi");
    btnViewPazymiai = new QPushButton("Perziureti pazymius");
    btnEditPazymys = new QPushButton("Redaguoti pazymi");
    btnAddPazymys->setMinimumHeight(40);
    btnViewPazymiai->setMinimumHeight(40);
    btnEditPazymys->setMinimumHeight(40);
    
    btnLayout->addWidget(btnAddPazymys);
    btnLayout->addWidget(btnViewPazymiai);
    btnLayout->addWidget(btnEditPazymys);
    mainLayout->addLayout(btnLayout);
    
    setCentralWidget(centralWidget);
    

    connect(btnAddPazymys, &QPushButton::clicked, this, &DestytojaWindow::onAddPazymys);
    connect(btnViewPazymiai, &QPushButton::clicked, this, &DestytojaWindow::onViewPazymiai);
    connect(btnEditPazymys, &QPushButton::clicked, this, &DestytojaWindow::onEditPazymys);
    connect(btnLogout, &QPushButton::clicked, this, &DestytojaWindow::onLogout);
    
    refreshMyDalykaiList();
}

void DestytojaWindow::refreshMyDalykaiList() {
    try {
        auto dalykai = dalykasRepo->findByDestytojas(currentDestytojas->getDestytojoId());
        dalykaiTable->setRowCount(dalykai.size());
        
        for (int i = 0; i < dalykai.size(); i++) {
            dalykaiTable->setItem(i, 0, new QTableWidgetItem(QString::number(dalykai[i].getId())));
            dalykaiTable->setItem(i, 1, new QTableWidgetItem(dalykai[i].getPavadinimas()));
        }
    } catch (const Klaida& e) {
        QMessageBox::warning(this, "Klaida", e.getZinute());
    }
}

void DestytojaWindow::onAddPazymys() {

    int row = dalykaiTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Perspejimas", "Pirmiausia pasirinkite dalyka is lenteles!");
        return;
    }
    
    try {
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
            QString("Dalykas: %1\n\nPasirinkite grupe:").arg(dalyko_pav),
            grupeNames, 0, false, &ok);
        if (!ok) return;
        
        int grupes_id = grupeIds[grupeNames.indexOf(grupePav)];
        
        auto studentai_ids = grupeRepo->getStudentai(grupes_id);
        if (studentai_ids.isEmpty()) {
            QMessageBox::warning(this, "Perspejimas", "Grupeje studentu nera!");
            return;
        }
        
        QStringList studentNames;
        QVector<int> studentIds;
        for (int stud_id : studentai_ids) {
            QString vardas_pavarde = studentasRepo->getStudentoVardasPavarde(stud_id);
            studentNames << QString("ID: %1 | %2").arg(stud_id).arg(vardas_pavarde);
            studentIds << stud_id;
        }
        
        QString studentChoice = QInputDialog::getItem(this, "Pasirinkite studenta", 
            QString("Dalykas: %1\nGrupe: %2\n\nPasirinkite studenta:").arg(dalyko_pav).arg(grupePav),
            studentNames, 0, false, &ok);
        if (!ok) return;
        
        int studento_id = studentIds[studentNames.indexOf(studentChoice)];
        
        int reiksme = QInputDialog::getInt(this, "Iveskite pazymi", 
            QString("Studentas: %1\nDalykas: %2\n\nPazymio reiksme (1-10):").arg(studentChoice).arg(dalyko_pav),
            5, 1, 10, 1, &ok);
        if (!ok) return;
        
        QString data = QInputDialog::getText(this, "Iveskite data", 
            "Data (MM-DD-YYYY):", QLineEdit::Normal, "", &ok);
        if (!ok || data.isEmpty()) return;
        
        Pazymys naujas;
        naujas.setStudentoId(studento_id);
        naujas.setDalykoId(dalyko_id);
        naujas.setDestytojoId(currentDestytojas->getDestytojoId());
        naujas.setReiksme(reiksme);
        naujas.setData(data);
        
        if (!naujas.validateDate(data)) {
            QMessageBox::critical(this, "Klaida", "Netinkama data! Naudokite formata MM-DD-YYYY.");
            return;
        }
        
        pazymysRepo->save(naujas);
        QMessageBox::information(this, "Sekme", "Pazymys ivestas sekmingai!");
        
    } catch (const Klaida& e) {
        QMessageBox::critical(this, "Klaida", e.getZinute());
    }
}

void DestytojaWindow::onViewPazymiai() {

    int row = dalykaiTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Perspejimas", "Pirmiausia pasirinkite dalyka is lenteles!");
        return;
    }
    
    try {
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
            QString("Dalykas: %1\n\nPasirinkite grupe, kurios pazymius norite perziureti:").arg(dalyko_pav),
            grupeNames, 0, false, &ok);
        if (!ok) return;
        
        int grupes_id = grupeIds[grupeNames.indexOf(grupePav)];
        
        auto pazymiai = pazymysRepo->findByDalykasAndGrupe(dalyko_id, grupes_id);
        
        if (pazymiai.isEmpty()) {
            QMessageBox::information(this, "Info", 
                QString("Grupeje \"%1\" dalyko \"%2\" pazymiu dar nera.").arg(grupePav).arg(dalyko_pav));
            return;
        }
        
        QString message = QString("Dalykas: %1\nGrupe: %2\n\nStudentu pazymiai:\n\n").arg(dalyko_pav).arg(grupePav);
        for (const auto& paz : pazymiai) {
            QString stud_vardas = studentasRepo->getStudentoVardasPavarde(paz.getStudentoId());
            message += QString("Studento ID: %1 (%2) - Pazymys: %3 - Data: %4\n")
                           .arg(paz.getStudentoId())
                           .arg(stud_vardas)
                           .arg(paz.getReiksme())
                           .arg(paz.getData());
        }
        
        QMessageBox::information(this, "Studentu pazymiai", message);
        
    } catch (const Klaida& e) {
        QMessageBox::critical(this, "Klaida", e.getZinute());
    }
}

void DestytojaWindow::onEditPazymys() {

    int row = dalykaiTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Perspejimas", "Pirmiausia pasirinkite dalyka is lenteles!");
        return;
    }
    
    try {
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
            QString("Dalykas: %1\n\nPasirinkite grupe:").arg(dalyko_pav),
            grupeNames, 0, false, &ok);
        if (!ok) return;
        
        int grupes_id = grupeIds[grupeNames.indexOf(grupePav)];
        
        auto pazymiai = pazymysRepo->findByDalykasAndGrupe(dalyko_id, grupes_id);
        
        if (pazymiai.isEmpty()) {
            QMessageBox::information(this, "Info", 
                QString("Grupeje \"%1\" dalyko \"%2\" pazymiu dar nera.").arg(grupePav).arg(dalyko_pav));
            return;
        }
        
        QStringList pazymysChoices;
        QVector<int> pazymysIds;
        for (const auto& paz : pazymiai) {
            QString stud_vardas = studentasRepo->getStudentoVardasPavarde(paz.getStudentoId());
            pazymysChoices << QString("ID: %1 | Studentas: %2 | Pazymys: %3 | Data: %4")
                               .arg(paz.getId())
                               .arg(stud_vardas)
                               .arg(paz.getReiksme())
                               .arg(paz.getData());
            pazymysIds << paz.getId();
        }
        
        QString pazymysChoice = QInputDialog::getItem(this, "Pasirinkite pazymi", 
            QString("Dalykas: %1\nGrupe: %2\n\nPasirinkite pazymi redagavimui:").arg(dalyko_pav).arg(grupePav),
            pazymysChoices, 0, false, &ok);
        if (!ok) return;
        
        int pazymio_id = pazymysIds[pazymysChoices.indexOf(pazymysChoice)];
        
        Pazymys pazymys = pazymysRepo->findById(pazymio_id);
        
        if (pazymys.getId() == 0) {
            QMessageBox::warning(this, "Klaida", "Pazymys nerastas!");
            return;
        }
        
        int nauja_reiksme = QInputDialog::getInt(this, "Redaguoti pazymi", 
            QString("Pazymio ID: %1\n\nNauja reiksme (1-10):").arg(pazymio_id),
            pazymys.getReiksme(), 1, 10, 1, &ok);
        if (!ok) return;
        
        QString nauja_data = QInputDialog::getText(this, "Redaguoti pazymi", 
            "Nauja data (MM-DD-YYYY):", 
            QLineEdit::Normal, pazymys.getData(), &ok);
        if (!ok) return;
        
        if (!pazymys.validateDate(nauja_data)) {
            QMessageBox::critical(this, "Klaida", "Netinkama data! Naudokite formata MM-DD-YYYY.");
            return;
        }
        
        pazymys.setReiksme(nauja_reiksme);
        pazymys.setData(nauja_data);
        
        if (pazymysRepo->update(pazymys)) {
            QMessageBox::information(this, "Sekme", "Pazymys pakeistas sekmingai!");
        } else {
            QMessageBox::critical(this, "Klaida", "Nepavyko pakeisti pazymio!");
        }
        
    } catch (const Klaida& e) {
        QMessageBox::critical(this, "Klaida", e.getZinute());
    }
}

void DestytojaWindow::onLogout() {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Atsijungti", "Ar tikrai norite atsijungti?",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        emit logoutRequested();
        close();
    }
}
