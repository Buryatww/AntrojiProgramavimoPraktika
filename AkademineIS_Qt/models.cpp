#include "models.h"
#include "sqlite3.h"
#include <QDebug>
#include <QRegularExpression>

DatabaseManager::DatabaseManager(const QString& path) 
    : db(nullptr), dbPath(path) {}

DatabaseManager::~DatabaseManager() {
    if (db) {
        sqlite3_close(db);
    }
}

bool DatabaseManager::connect() {
    int rc = sqlite3_open(dbPath.toStdString().c_str(), &db);
    if (rc) {
        qDebug() << "Nepavyko atidaryti duomenu bazes:" << sqlite3_errmsg(db);
        return false;
    }
    qDebug() << "Duomenu baze prijungta sekmingai!";
    return true;
}

Naudotojas::Naudotojas() 
    : id(0), vardas(""), pavarde(""), role(""), 
      prisijungimo_vardas(""), slaptazodis("") {}


bool Naudotojas::validate() const {
    if (vardas.isEmpty()) throw LogineKlaida("Vardas negali buti tuscias");
    if (pavarde.isEmpty()) throw LogineKlaida("Pavarde negali buti tuscia");
    if (role.isEmpty()) throw LogineKlaida("Role turi buti nurodyta");
    return true;
}

Admin::Admin() { 
    role = "admin"; 
}


Destytojas::Destytojas() : destytojo_id(0) { 
    role = "destytojas"; 
}


Studentas::Studentas() : studento_id(0), grupes_id(0) { 
    role = "studentas"; 
}


Grupe::Grupe() : id(0) {}

bool Grupe::validate() const {
    if (pavadinimas.isEmpty()) throw LogineKlaida("Pavadinimas negali buti tuscias");
    return true;
}

Dalykas::Dalykas() : id(0) {}

bool Dalykas::validate() const {
    if (pavadinimas.isEmpty()) throw LogineKlaida("Pavadinimas negali buti tuscias");
    return true;
}

Pazymys::Pazymys() : id(0), studento_id(0), dalyko_id(0), destytojo_id(0), reiksme(0) {}

bool Pazymys::validate() const {
    if (studento_id <= 0 || dalyko_id <= 0 || destytojo_id <= 0) {
        throw LogineKlaida("Neteisingi ID");
    }
    if (reiksme < 1 || reiksme > 10) {
        throw LogineKlaida("Pazymys turi buti tarp 1 ir 10");
    }
    if (data.isEmpty()) {
        throw LogineKlaida("Data negali buti tuscia");
    }
    return true;
}

bool Pazymys::validateDate(const QString& dateStr) const {
    if (dateStr.length() != 10) return false;
    if (dateStr[2] != '-' || dateStr[5] != '-') return false;
    
    QRegularExpression re("^\\d{2}-\\d{2}-\\d{4}$");
    if (!re.match(dateStr).hasMatch()) return false;
    
    int month = dateStr.mid(0, 2).toInt();
    int day = dateStr.mid(3, 2).toInt();
    int year = dateStr.mid(6, 4).toInt();
    
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31) return false;
    if (year < 1900 || year > 2100) return false;
    
    return true;
}
