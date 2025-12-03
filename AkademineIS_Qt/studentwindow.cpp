#include "studentwindow.h"
#include "exceptions.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QHeaderView>

StudentWindow::StudentWindow(DatabaseManager* db, Studentas* studentas, QWidget *parent)
    : QMainWindow(parent), database(db), currentStudentas(studentas)
{
    grupeRepo = new GrupeRepository(database);
    dalykasRepo = new DalykasRepository(database);
    pazymysRepo = new PazymysRepository(database);
    
    setupUI();
    setWindowTitle(QString("Studento Panel - %1 %2").arg(studentas->getVardas()).arg(studentas->getPavarde()));
    resize(800, 500);
}

StudentWindow::~StudentWindow() {
    delete grupeRepo;
    delete dalykasRepo;
    delete pazymysRepo;
}

void StudentWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    QLabel* title = new QLabel("<h2>Studento Meniu</h2>");
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
                                      .arg(currentStudentas->getVardas())
                                      .arg(currentStudentas->getPavarde()));
    welcomeLabel->setStyleSheet("QLabel { font-size: 14pt; color: #0066cc; margin: 10px; }");
    mainLayout->addWidget(welcomeLabel);
    
    QLabel* dalykaiLabel = new QLabel("<h3>Jusu grupes dalykai:</h3>");
    mainLayout->addWidget(dalykaiLabel);
    
    dalykaiTable = new QTableWidget();
    dalykaiTable->setColumnCount(2);
    dalykaiTable->setHorizontalHeaderLabels({"Dalyko ID", "Pavadinimas"});
    dalykaiTable->horizontalHeader()->setStretchLastSection(true);
    dalykaiTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    dalykaiTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(dalykaiTable);
    
    btnViewPazymiai = new QPushButton("Perziureti mano pazymius pasirinkto dalyko");
    btnViewPazymiai->setMinimumHeight(40);
    mainLayout->addWidget(btnViewPazymiai);
    
    setCentralWidget(centralWidget);
    
    connect(btnViewPazymiai, &QPushButton::clicked, this, &StudentWindow::onViewMyPazymiai);
    connect(btnLogout, &QPushButton::clicked, this, &StudentWindow::onLogout);
    
    refreshDalykaiList();
}

void StudentWindow::refreshDalykaiList() {
    try {
        auto dalykai = grupeRepo->getDalykai(currentStudentas->getGrupesId());
        dalykaiTable->setRowCount(dalykai.size());
        
        if (dalykai.isEmpty()) {
            QMessageBox::information(this, "Info", "Jusu grupei nepriskirta jokiu dalyku.");
            return;
        }
        
        for (int i = 0; i < dalykai.size(); i++) {
            dalykaiTable->setItem(i, 0, new QTableWidgetItem(QString::number(dalykai[i].getId())));
            dalykaiTable->setItem(i, 1, new QTableWidgetItem(dalykai[i].getPavadinimas()));
        }
    } catch (const Klaida& e) {
        QMessageBox::warning(this, "Klaida", e.getZinute());
    }
}

void StudentWindow::onViewMyPazymiai() {
    int row = dalykaiTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Perspejimas", "Pasirinkite dalyka is lenteles!");
        return;
    }
    
    try {
        int dalyko_id = dalykaiTable->item(row, 0)->text().toInt();
        QString dalyko_pav = dalykaiTable->item(row, 1)->text();
        
        auto pazymiai = pazymysRepo->findByStudentAndDalykas(
            currentStudentas->getStudentoId(), dalyko_id
        );
        
        if (pazymiai.isEmpty()) {
            QMessageBox::information(this, "Info", 
                QString("Dalyko \"%1\" pazymiu dar nera.").arg(dalyko_pav));
            return;
        }
        
        QString message = QString("Dalykas: %1\n\nJusu pazymiai:\n\n").arg(dalyko_pav);
        
        double suma = 0;
        for (const auto& p : pazymiai) {
            QString dest_vardas = pazymysRepo->getDestytojoVardas(p.getDestytojoId());
            message += QString("Pazymys: %1 | Data: %2 | Destytojas: %3\n")
                           .arg(p.getReiksme())
                           .arg(p.getData())
                           .arg(dest_vardas);
            suma += p.getReiksme();
        }
        
        double vidurkis = suma / pazymiai.size();
        message += QString("\n---\nVidurkis: %1").arg(vidurkis, 0, 'f', 2);
        
        QMessageBox::information(this, "Mano pazymiai", message);
        
    } catch (const Klaida& e) {
        QMessageBox::critical(this, "Klaida", e.getZinute());
    }
}

void StudentWindow::onLogout() {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Atsijungti", "Ar tikrai norite atsijungti?",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        emit logoutRequested();
        close();
    }
}
