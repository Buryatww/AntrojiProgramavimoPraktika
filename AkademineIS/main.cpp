#include <iostream>
#include "sqlite3.h"
#include <string>
#include <vector>
#include <memory>

using namespace std;


class Klaida {
protected:
    string zinute;
public:
    Klaida(const string& z) : zinute(z) {}
    string getZinute() const { return zinute; }
};

class LogineKlaida : public Klaida {
public:
    LogineKlaida(const string& z) : Klaida("Neteisingi duomenys: " + z) {}
};

class TechnineKlaida : public Klaida {
public:
    TechnineKlaida(const string& z) : Klaida("Sistemos klaida: " + z) {}
};

class NerastasKlaida : public Klaida {
public:
    NerastasKlaida(const string& objektas) : Klaida(objektas + " nerastas") {}
};

class DatabaseManager {
private:
    sqlite3* db;
    string dbPath;

public:
    DatabaseManager(const string& path) : db(nullptr), dbPath(path) {}

    ~DatabaseManager() {
        if (db) {
            sqlite3_close(db);
        }
    }

    bool connect() {
        int rc = sqlite3_open(dbPath.c_str(), &db);
        if (rc) {
            cerr << "Nepavyko atidaryti duomenu bazes: " << sqlite3_errmsg(db) << endl;
            return false;
        }
        cout << "Duomenu baze prijungta sekmingai!" << endl;
        return true;
    }

    sqlite3* getDb() { return db; }
};


bool inputSuCancel(const string& prompt, string& value) {
    cout << prompt << " (arba '0' - atgal): ";
    cin >> value;
    if (value == "0") {
        cout << "Griztama i meniu..." << endl;
        return false;
    }
    return true;
}

bool inputSuCancel(const string& prompt, int& value) {
    cout << prompt << " (arba '0' - atgal): ";
    cin >> value;
    if (value == 0) {
        cout << "Griztama i meniu..." << endl;
        return false;
    }
    return true;
}

class Naudotojas {
protected:
    int id;
    string vardas;
    string pavarde;
    string role;
    string prisijungimo_vardas;
    string slaptazodis;

public:
    Naudotojas() : id(0) {}

    Naudotojas(int id, const string& vardas, const string& pavarde,
               const string& role)
        : id(id), vardas(vardas), pavarde(pavarde), role(role),
          prisijungimo_vardas(vardas), slaptazodis(pavarde) {}

    virtual ~Naudotojas() {}

    int getId() const { return id; }
    string getVardas() const { return vardas; }
    string getPavarde() const { return pavarde; }
    string getRole() const { return role; }
    string getPrisijungimoVardas() const { return prisijungimo_vardas; }
    string getSlaptazodis() const { return slaptazodis; }

    void setId(int newId) { id = newId; }
    void setVardas(const string& v) { vardas = v; prisijungimo_vardas = v; }
    void setPavarde(const string& p) { pavarde = p; slaptazodis = p; }
    void setRole(const string& r) { role = r; }

    virtual void rodytiInformacija() const {
        cout << "ID: " << id << " | " << vardas << " " << pavarde
                  << " | Role: " << role << endl;
    }

    bool validate() const {
        if (vardas.empty()) throw LogineKlaida("Vardas negali buti tuscias");
        if (pavarde.empty()) throw LogineKlaida("Pavarde negali buti tuscia");
        if (role.empty()) throw LogineKlaida("Role turi buti nurodyta");
        return true;
    }
};

class Admin : public Naudotojas {
public:
    Admin() { role = "admin"; }
    Admin(int id, const string& vardas, const string& pavarde)
        : Naudotojas(id, vardas, pavarde, "admin") {}

    void rodytiInformacija() const override {
        cout << "[ADMIN] " << vardas << " " << pavarde << endl;
    }
};

class Destytojas : public Naudotojas {
private:
    int destytojo_id;

public:
    Destytojas() : destytojo_id(0) { role = "destytojas"; }
    Destytojas(int naud_id, const string& vardas, const string& pavarde, int dest_id)
        : Naudotojas(naud_id, vardas, pavarde, "destytojas"), destytojo_id(dest_id) {}

    int getDestytojoId() const { return destytojo_id; }
    void setDestytojoId(int id) { destytojo_id = id; }

    void rodytiInformacija() const override {
        cout << "[DESTYTOJAS] " << vardas << " " << pavarde << endl;
    }
};

class Studentas : public Naudotojas {
private:
    int studento_id;
    int grupes_id;

public:
    Studentas() : studento_id(0), grupes_id(0) { role = "studentas"; }
    Studentas(int naud_id, const string& vardas, const string& pavarde,
              int stud_id, int grupe_id)
        : Naudotojas(naud_id, vardas, pavarde, "studentas"),
          studento_id(stud_id), grupes_id(grupe_id) {}

    int getStudentoId() const { return studento_id; }
    int getGrupesId() const { return grupes_id; }
    void setStudentoId(int id) { studento_id = id; }
    void setGrupesId(int id) { grupes_id = id; }

    void rodytiInformacija() const override {
        cout << "[STUDENTAS] " << vardas << " " << pavarde
             << " | Grupe ID: " << grupes_id << endl;
    }
};

class Grupe {
private:
    int id;
    string pavadinimas;

public:
    Grupe() : id(0) {}
    Grupe(int id, const string& pav) : id(id), pavadinimas(pav) {}

    int getId() const { return id; }
    string getPavadinimas() const { return pavadinimas; }
    void setId(int newId) { id = newId; }
    void setPavadinimas(const string& pav) { pavadinimas = pav; }

    bool validate() const {
        if (pavadinimas.empty()) throw LogineKlaida("Pavadinimas negali buti tuscias");
        return true;
    }
};

class Dalykas {
private:
    int id;
    string pavadinimas;

public:
    Dalykas() : id(0) {}
    Dalykas(int id, const string& pav) : id(id), pavadinimas(pav) {}

    int getId() const { return id; }
    string getPavadinimas() const { return pavadinimas; }
    void setId(int newId) { id = newId; }
    void setPavadinimas(const string& pav) { pavadinimas = pav; }

    bool validate() const {
        if (pavadinimas.empty()) throw LogineKlaida("Pavadinimas negali buti tuscias");
        return true;
    }
};

class Pazymys {
private:
    int id;
    int studento_id;
    int dalyko_id;
    int destytojo_id;
    int reiksme;
    string data;

public:
    Pazymys() : id(0), studento_id(0), dalyko_id(0), destytojo_id(0), reiksme(0) {}

    Pazymys(int id, int stud_id, int dal_id, int dest_id, int reik, const string& dat)
        : id(id), studento_id(stud_id), dalyko_id(dal_id),
          destytojo_id(dest_id), reiksme(reik), data(dat) {}

    int getId() const { return id; }
    int getStudentoId() const { return studento_id; }
    int getDalykoId() const { return dalyko_id; }
    int getDestytojoId() const { return destytojo_id; }
    int getReiksme() const { return reiksme; }
    string getData() const { return data; }

    void setId(int i) { id = i; }
    void setStudentoId(int i) { studento_id = i; }
    void setDalykoId(int i) { dalyko_id = i; }
    void setDestytojoId(int i) { destytojo_id = i; }
    void setReiksme(int r) { reiksme = r; }
    void setData(const string& d) { data = d; }

    bool validate() const {
        if (studento_id <= 0 || dalyko_id <= 0 || destytojo_id <= 0) {
            throw LogineKlaida("Neteisingi ID");
        }
        if (reiksme < 1 || reiksme > 10) {
            throw LogineKlaida("Pazymys turi buti tarp 1 ir 10");
        }
        if (data.empty()) {
            throw LogineKlaida("Data negali buti tuscia");
        }
        return true;
    }
    bool validateDate(const std::string& data) {
        if (data.length() != 10) return false;
        if (data[2] != '-' || data[5] != '-') return false;

        for (int i = 0; i < 10; i++) {
            if (i == 2 || i == 5) continue;
            if (!isdigit(data[i])) return false;
        }

        int month = stoi(data.substr(0, 2));
        int day = stoi(data.substr(3, 2));
        int year = stoi(data.substr(6, 4));

        if (month < 1 || month > 12) return false;
        if (day < 1 || day > 31) return false;
        if (year < 1900 || year > 2100) return false;

        return true;
    }
};


class NaudotojasRepository {
private:
    DatabaseManager* db;

public:
    NaudotojasRepository(DatabaseManager* database) : db(database) {}

    int save(const Naudotojas& user) {
        user.validate();

        string sql = "INSERT INTO naudotojas (vardas, pavarde, role, prisijungimo_vardas, slaptazodis) "
                     "VALUES (?, ?, ?, ?, ?);";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            throw TechnineKlaida("Nepavyko paruosti SQL");
        }

        sqlite3_bind_text(stmt, 1, user.getVardas().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, user.getPavarde().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, user.getRole().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, user.getPrisijungimoVardas().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 5, user.getSlaptazodis().c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            throw TechnineKlaida("Nepavyko issaugoti naudotojo");
        }

        int newId = sqlite3_last_insert_rowid(db->getDb());
        sqlite3_finalize(stmt);
        return newId;
    }

    bool remove(int id) {
        string sql = "DELETE FROM naudotojas WHERE id = ?;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            return false;
        }

        sqlite3_bind_int(stmt, 1, id);
        bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);
        return success;
    }

    vector<Naudotojas> findAll() {
        vector<Naudotojas> naudotojai;
        string sql = "SELECT id, vardas, pavarde, role FROM naudotojas;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                int id = sqlite3_column_int(stmt, 0);
                string vardas = (const char*)sqlite3_column_text(stmt, 1);
                string pavarde = (const char*)sqlite3_column_text(stmt, 2);
                string role = (const char*)sqlite3_column_text(stmt, 3);

                naudotojai.push_back(Naudotojas(id, vardas, pavarde, role));
            }
            sqlite3_finalize(stmt);
        }
        return naudotojai;
    }

    shared_ptr<Naudotojas> authenticate(const string& username, const string& password) {
        string sql = "SELECT id, vardas, pavarde, role FROM naudotojas "
                     "WHERE prisijungimo_vardas = ? AND slaptazodis = ?;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            return nullptr;
        }

        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_TRANSIENT);

        shared_ptr<Naudotojas> user = nullptr;

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int naud_id = sqlite3_column_int(stmt, 0);
            string vardas = (const char*)sqlite3_column_text(stmt, 1);
            string pavarde = (const char*)sqlite3_column_text(stmt, 2);
            string role = (const char*)sqlite3_column_text(stmt, 3);

            sqlite3_finalize(stmt);

            if (role == "admin") {
                return make_shared<Admin>(naud_id, vardas, pavarde);
            }
            else if (role == "destytojas") {
                string sql2 = "SELECT id FROM destytojas WHERE naudotojo_id = ?;";
                sqlite3_stmt* stmt2;
                if (sqlite3_prepare_v2(db->getDb(), sql2.c_str(), -1, &stmt2, nullptr) == SQLITE_OK) {
                    sqlite3_bind_int(stmt2, 1, naud_id);
                    int dest_id = 0;
                    if (sqlite3_step(stmt2) == SQLITE_ROW) {
                        dest_id = sqlite3_column_int(stmt2, 0);
                    }
                    sqlite3_finalize(stmt2);
                    return make_shared<Destytojas>(naud_id, vardas, pavarde, dest_id);
                }
            }
            else if (role == "studentas") {
                string sql3 = "SELECT id, grupes_id FROM studentas WHERE naudotojo_id = ?;";
                sqlite3_stmt* stmt3;
                if (sqlite3_prepare_v2(db->getDb(), sql3.c_str(), -1, &stmt3, nullptr) == SQLITE_OK) {
                    sqlite3_bind_int(stmt3, 1, naud_id);
                    int stud_id = 0;
                    int grupe_id = 0;
                    if (sqlite3_step(stmt3) == SQLITE_ROW) {
                        stud_id = sqlite3_column_int(stmt3, 0);
                        grupe_id = sqlite3_column_int(stmt3, 1);
                    }
                    sqlite3_finalize(stmt3);
                    return make_shared<Studentas>(naud_id, vardas, pavarde, stud_id, grupe_id);
                }
            }
        } else {
            sqlite3_finalize(stmt);
        }

        return nullptr;
    }
};

class GrupeRepository {
private:
    DatabaseManager* db;

public:
    GrupeRepository(DatabaseManager* database) : db(database) {}

    int save(const Grupe& grupe) {
        grupe.validate();

        string sql = "INSERT INTO grupe (pavadinimas) VALUES (?);";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            throw TechnineKlaida("Nepavyko paruosti SQL");
        }

        sqlite3_bind_text(stmt, 1, grupe.getPavadinimas().c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            throw TechnineKlaida("Nepavyko issaugoti grupę");
        }

        int newId = sqlite3_last_insert_rowid(db->getDb());
        sqlite3_finalize(stmt);
        return newId;
    }

    bool remove(int id) {
        string sql = "DELETE FROM grupe WHERE id = ?;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            return false;
        }

        sqlite3_bind_int(stmt, 1, id);
        bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);
        return success;
    }

    vector<Grupe> findAll() {
        vector<Grupe> grupes;
        string sql = "SELECT id, pavadinimas FROM grupe;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                int id = sqlite3_column_int(stmt, 0);
                string pav = (const char*)sqlite3_column_text(stmt, 1);
                grupes.push_back(Grupe(id, pav));
            }
            sqlite3_finalize(stmt);
        }
        return grupes;
    }

    vector<int> getStudentai(int grupes_id) {
        vector<int> studentai;
        string sql = "SELECT id FROM studentas WHERE grupes_id = ?;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, grupes_id);

            while (sqlite3_step(stmt) == SQLITE_ROW) {
                studentai.push_back(sqlite3_column_int(stmt, 0));
            }
            sqlite3_finalize(stmt);
        }
        return studentai;
    }

    vector<Dalykas> getDalykai(int grupes_id) {
        vector<Dalykas> dalykai;
        string sql = "SELECT d.id, d.pavadinimas FROM dalykas d "
                     "JOIN GrupesDalykai gd ON d.id = gd.dalyko_id "
                     "WHERE gd.grupes_id = ?;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, grupes_id);

            while (sqlite3_step(stmt) == SQLITE_ROW) {
                int id = sqlite3_column_int(stmt, 0);
                string pav = (const char*)sqlite3_column_text(stmt, 1);
                dalykai.push_back(Dalykas(id, pav));
            }
            sqlite3_finalize(stmt);
        }
        return dalykai;
    }
};

class DalykasRepository {
private:
    DatabaseManager* db;

public:
    DalykasRepository(DatabaseManager* database) : db(database) {}

    int save(const Dalykas& dalykas) {
        dalykas.validate();

        string sql = "INSERT INTO dalykas (pavadinimas) VALUES (?);";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            throw TechnineKlaida("Nepavyko paruosti SQL");
        }

        sqlite3_bind_text(stmt, 1, dalykas.getPavadinimas().c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            throw TechnineKlaida("Nepavyko issaugoti dalyką");
        }

        int newId = sqlite3_last_insert_rowid(db->getDb());
        sqlite3_finalize(stmt);
        return newId;
    }

    bool remove(int id) {
        string sql = "DELETE FROM dalykas WHERE id = ?;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            return false;
        }

        sqlite3_bind_int(stmt, 1, id);
        bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);
        return success;
    }

    Dalykas findById(int id) {
        string sql = "SELECT id, pavadinimas FROM dalykas WHERE id = ?;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, id);

            if (sqlite3_step(stmt) == SQLITE_ROW) {
                int did = sqlite3_column_int(stmt, 0);
                string pav = (const char*)sqlite3_column_text(stmt, 1);
                sqlite3_finalize(stmt);
                return Dalykas(did, pav);
            }
            sqlite3_finalize(stmt);
        }

        throw NerastasKlaida("Dalykas");
    }

    vector<Dalykas> findAll() {
        vector<Dalykas> dalykai;
        string sql = "SELECT id, pavadinimas FROM dalykas;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                int id = sqlite3_column_int(stmt, 0);
                string pav = (const char*)sqlite3_column_text(stmt, 1);
                dalykai.push_back(Dalykas(id, pav));
            }
            sqlite3_finalize(stmt);
        }
        return dalykai;
    }

    vector<Dalykas> findByDestytojas(int destytojo_id) {
        vector<Dalykas> dalykai;
        string sql = "SELECT d.id, d.pavadinimas FROM dalykas d "
                     "JOIN DestytojoDalykai dd ON d.id = dd.dalyko_id "
                     "WHERE dd.destytojo_id = ?;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, destytojo_id);

            while (sqlite3_step(stmt) == SQLITE_ROW) {
                int id = sqlite3_column_int(stmt, 0);
                string pav = (const char*)sqlite3_column_text(stmt, 1);
                dalykai.push_back(Dalykas(id, pav));
            }
            sqlite3_finalize(stmt);
        }
        return dalykai;
    }

    bool assignToDestytojas(int dalyko_id, int destytojo_id) {
        string sql = "INSERT INTO DestytojoDalykai (destytojo_id, dalyko_id) VALUES (?, ?);";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            return false;
        }

        sqlite3_bind_int(stmt, 1, destytojo_id);
        sqlite3_bind_int(stmt, 2, dalyko_id);

        bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);
        return success;
    }

    bool assignToGrupe(int dalyko_id, int grupes_id) {
        string sql = "INSERT INTO GrupesDalykai (grupes_id, dalyko_id) VALUES (?, ?);";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            return false;
        }

        sqlite3_bind_int(stmt, 1, grupes_id);
        sqlite3_bind_int(stmt, 2, dalyko_id);

        bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);
        return success;
    }
};

class PazymysRepository {
private:
    DatabaseManager* db;

public:
    PazymysRepository(DatabaseManager* database) : db(database) {}

    int save(const Pazymys& pazymys) {
        pazymys.validate();

        string sql = "INSERT INTO pazymys (studento_id, dalyko_id, destytojo_id, reiksme, data) "
                     "VALUES (?, ?, ?, ?, ?);";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            throw TechnineKlaida("Nepavyko paruosti SQL");
        }

        sqlite3_bind_int(stmt, 1, pazymys.getStudentoId());
        sqlite3_bind_int(stmt, 2, pazymys.getDalykoId());
        sqlite3_bind_int(stmt, 3, pazymys.getDestytojoId());
        sqlite3_bind_int(stmt, 4, pazymys.getReiksme());
        sqlite3_bind_text(stmt, 5, pazymys.getData().c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            throw TechnineKlaida("Nepavyko issaugoti pazymi");
        }

        int newId = sqlite3_last_insert_rowid(db->getDb());
        sqlite3_finalize(stmt);
        return newId;
    }

    bool update(const Pazymys& pazymys) {
        pazymys.validate();

        string sql = "UPDATE pazymys SET reiksme = ?, data = ? WHERE id = ?;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            return false;
        }

        sqlite3_bind_int(stmt, 1, pazymys.getReiksme());
        sqlite3_bind_text(stmt, 2, pazymys.getData().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 3, pazymys.getId());

        bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);
        return success;
    }

    Pazymys findById(int id) {
        string sql = "SELECT id, studento_id, dalyko_id, destytojo_id, reiksme, data "
                     "FROM pazymys WHERE id = ?;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, id);

            if (sqlite3_step(stmt) == SQLITE_ROW) {
                Pazymys p;
                p.setId(sqlite3_column_int(stmt, 0));
                p.setStudentoId(sqlite3_column_int(stmt, 1));
                p.setDalykoId(sqlite3_column_int(stmt, 2));
                p.setDestytojoId(sqlite3_column_int(stmt, 3));
                p.setReiksme(sqlite3_column_int(stmt, 4));
                p.setData((const char*)sqlite3_column_text(stmt, 5));
                sqlite3_finalize(stmt);
                return p;
            }
            sqlite3_finalize(stmt);
        }

        Pazymys empty;
        return empty;
    }

    vector<Pazymys> findAll() {
        vector<Pazymys> pazymiai;
        string sql = "SELECT id, studento_id, dalyko_id, destytojo_id, reiksme, data FROM pazymys;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                int id = sqlite3_column_int(stmt, 0);
                int sid = sqlite3_column_int(stmt, 1);
                int did = sqlite3_column_int(stmt, 2);
                int destId = sqlite3_column_int(stmt, 3);
                int reiksme = sqlite3_column_int(stmt, 4);
                string data = (const char*)sqlite3_column_text(stmt, 5);

                pazymiai.push_back(Pazymys(id, sid, did, destId, reiksme, data));
            }
            sqlite3_finalize(stmt);
        }
        return pazymiai;
    }

    vector<Pazymys> findByStudentAndDalykas(int studento_id, int dalyko_id) {
        vector<Pazymys> pazymiai;
        string sql = "SELECT id, studento_id, dalyko_id, destytojo_id, reiksme, data "
                     "FROM pazymys WHERE studento_id = ? AND dalyko_id = ?;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, studento_id);
            sqlite3_bind_int(stmt, 2, dalyko_id);

            while (sqlite3_step(stmt) == SQLITE_ROW) {
                int id = sqlite3_column_int(stmt, 0);
                int sid = sqlite3_column_int(stmt, 1);
                int did = sqlite3_column_int(stmt, 2);
                int destId = sqlite3_column_int(stmt, 3);
                int reiksme = sqlite3_column_int(stmt, 4);
                string data = (const char*)sqlite3_column_text(stmt, 5);

                pazymiai.push_back(Pazymys(id, sid, did, destId, reiksme, data));
            }
            sqlite3_finalize(stmt);
        }
        return pazymiai;
    }

    vector<Pazymys> findByDalykas(int dalyko_id) {
        vector<Pazymys> pazymiai;
        string sql = "SELECT id, studento_id, dalyko_id, destytojo_id, reiksme, data "
                     "FROM pazymys WHERE dalyko_id = ?;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, dalyko_id);

            while (sqlite3_step(stmt) == SQLITE_ROW) {
                int id = sqlite3_column_int(stmt, 0);
                int sid = sqlite3_column_int(stmt, 1);
                int did = sqlite3_column_int(stmt, 2);
                int destId = sqlite3_column_int(stmt, 3);
                int reiksme = sqlite3_column_int(stmt, 4);
                string data = (const char*)sqlite3_column_text(stmt, 5);

                pazymiai.push_back(Pazymys(id, sid, did, destId, reiksme, data));
            }
            sqlite3_finalize(stmt);
        }
        return pazymiai;
    }

    string getDestytojoVardas(int destytojo_id) {
        string sql = "SELECT n.vardas, n.pavarde FROM naudotojas n "
                     "JOIN destytojas d ON n.id = d.naudotojo_id "
                     "WHERE d.id = ?;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, destytojo_id);

            if (sqlite3_step(stmt) == SQLITE_ROW) {
                string vardas = (const char*)sqlite3_column_text(stmt, 0);
                string pavarde = (const char*)sqlite3_column_text(stmt, 1);
                sqlite3_finalize(stmt);
                return vardas + " " + pavarde;
            }
            sqlite3_finalize(stmt);
        }
        return "Nezinomas";
    }
};

class DestytojaRepository {
private:
    DatabaseManager* db;

public:
    DestytojaRepository(DatabaseManager* database) : db(database) {}

    vector<pair<int, string>> getAll() {
        vector<pair<int, string>> destytojai;
        string sql = "SELECT d.id, n.vardas, n.pavarde FROM destytojas d "
                     "JOIN naudotojas n ON d.naudotojo_id = n.id;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                int id = sqlite3_column_int(stmt, 0);
                string vardas = (const char*)sqlite3_column_text(stmt, 1);
                string pavarde = (const char*)sqlite3_column_text(stmt, 2);
                destytojai.push_back({id, vardas + " " + pavarde});
            }
            sqlite3_finalize(stmt);
        }
        return destytojai;
    }
};

class StudentasRepository {
private:
    DatabaseManager* db;

public:
    StudentasRepository(DatabaseManager* database) : db(database) {}

    string getStudentoVardasPavarde(int studento_id) {
        string sql = "SELECT n.vardas, n.pavarde FROM naudotojas n "
                     "JOIN studentas s ON n.id = s.naudotojo_id "
                     "WHERE s.id = ?;";
        sqlite3_stmt* stmt;
        string result = "";

        if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, studento_id);
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                string vardas = (const char*)sqlite3_column_text(stmt, 0);
                string pavarde = (const char*)sqlite3_column_text(stmt, 1);
                result = vardas + " " + pavarde;
            }
            sqlite3_finalize(stmt);
        }
        return result;
    }
};

void administratoriausMeniu(Admin& admin, DatabaseManager& db);
void destytojoMeniu(Destytojas& destytojas, DatabaseManager& db);
void studentoMeniu(Studentas& studentas, DatabaseManager& db);

int main() {
    DatabaseManager db("AkademineIS.db");
    if (!db.connect()) {
        cout << "Patikrinkite, ar AkademineIS.db yra teisingu keliu!" << endl;
        return 1;
    }

    while (true) {
        cout << "\n========================================" << endl;
        cout << "   AKADEMINE INFORMACINE SISTEMA" << endl;
        cout << "========================================" << endl;
        cout << "1. Prisijungti" << endl;
        cout << "0. Iseiti" << endl;
        cout << "Pasirinkimas: ";
        int pasirinkimas;
        cin >> pasirinkimas;

        if (pasirinkimas == 0) {
            cout << "Viso gero!" << endl;
            break;
        }
        else if (pasirinkimas == 1) {
            string username, password;
            cout << "\nPrisijungimo vardas: ";
            cin >> username;
            cout << "Slaptazodis: ";
            cin >> password;

            try {
                NaudotojasRepository naudRepo(&db);
                auto user = naudRepo.authenticate(username, password);

                if (user) {
                    cout << "\nSveiki, " << user->getVardas() << "!" << endl;
                    user->rodytiInformacija();

                    if (user->getRole() == "admin") {
                        auto admin = dynamic_pointer_cast<Admin>(user);
                        administratoriausMeniu(*admin, db);
                    }
                    else if (user->getRole() == "destytojas") {
                        auto destytojas = dynamic_pointer_cast<Destytojas>(user);
                        destytojoMeniu(*destytojas, db);
                    }
                    else if (user->getRole() == "studentas") {
                        auto studentas = dynamic_pointer_cast<Studentas>(user);
                        studentoMeniu(*studentas, db);
                    }
                } else {
                    cout << "\nNeteisingas prisijungimo vardas arba slaptazodis!" << endl;
                }
            } catch (const Klaida& e) {
                cout << "Klaida: " << e.getZinute() << endl;
            }
        }
    }

    return 0;
}

void administratoriausMeniu(Admin& admin, DatabaseManager& db) {
    NaudotojasRepository naudRepo(&db);
    GrupeRepository grupeRepo(&db);
    DalykasRepository dalykasRepo(&db);
    PazymysRepository pazymysRepo(&db);
    DestytojaRepository destytojaRepo(&db);

    int pasirinkimas;
    do {
        cout << "\n=== ADMINISTRATORIAUS MENIU ===" << endl;
        cout << "1. Prideti nauja naudotoja" << endl;
        cout << "2. Perziureti visus naudotojus" << endl;
        cout << "3. Istrinti naudotoja" << endl;
        cout << "4. Prideti grupe" << endl;
        cout << "5. Perziureti grupes" << endl;
        cout << "6. Istrinti grupe" << endl;
        cout << "7. Prideti dalyka" << endl;
        cout << "8. Perziureti dalykus" << endl;
        cout << "9. Istrinti dalyka" << endl;
        cout << "10. Priskirti dalyka destytojui" << endl;
        cout << "11. Priskirti dalyka grupei" << endl;
        cout << "12. Parodyti grupes dalykus" << endl;
        cout << "13. Parodyti destytoju dalykus" << endl;
        cout << "14. Pakeisti pazymi" << endl;
        cout << "0. Atsijungti" << endl;
        cout << "Pasirinkimas: ";
        cin >> pasirinkimas;

        try {
            if (pasirinkimas == 1) {
                string vardas, pavarde, role;
                if (!inputSuCancel("Iveskite varda", vardas)) continue;
                if (!inputSuCancel("Iveskite pavarde", pavarde)) continue;
                if (!inputSuCancel("Role (admin/destytojas/studentas)", role)) continue;

                int grupes_id = 0;
                if (role == "studentas") {
                    auto grupes = grupeRepo.findAll();
                    cout << "\n--- VISOS GRUPES ---" << endl;
                    for (const auto& g : grupes) {
                        cout << "ID: " << g.getId() << " | " << g.getPavadinimas() << endl;
                    }

                    if (!inputSuCancel("\nIveskite grupes ID", grupes_id)) continue;
                }

                Naudotojas naujas;
                naujas.setVardas(vardas);
                naujas.setPavarde(pavarde);
                naujas.setRole(role);

                int newId = naudRepo.save(naujas);
                cout << "Naudotojas sukurtas sekmingai! ID: " << newId << endl;

                if (role == "studentas") {
                    string sql = "INSERT INTO studentas (naudotojo_id, grupes_id) VALUES (?, ?);";
                    sqlite3_stmt* stmt;
                    if (sqlite3_prepare_v2(db.getDb(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                        sqlite3_bind_int(stmt, 1, newId);
                        sqlite3_bind_int(stmt, 2, grupes_id);
                        sqlite3_step(stmt);
                        sqlite3_finalize(stmt);
                    }
                } else if (role == "destytojas") {
                    string sql = "INSERT INTO destytojas (naudotojo_id) VALUES (?);";
                    sqlite3_stmt* stmt;
                    if (sqlite3_prepare_v2(db.getDb(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                        sqlite3_bind_int(stmt, 1, newId);
                        sqlite3_step(stmt);
                        sqlite3_finalize(stmt);
                    }
                }
            }
            else if (pasirinkimas == 2) {
                auto visi = naudRepo.findAll();
                cout << "\n--- VISI NAUDOTOJAI ---" << endl;
                for (const auto& n : visi) {
                    n.rodytiInformacija();
                }
            }
            else if (pasirinkimas == 3) {
                auto visi = naudRepo.findAll();
                cout << "\n--- VISI NAUDOTOJAI ---" << endl;
                for (const auto& n : visi) {
                    n.rodytiInformacija();
                }

                int id;
                if (!inputSuCancel("\nPasirinkite trinamo naudotojo ID", id)) continue;

                if (naudRepo.remove(id)) {
                    cout << "Naudotojas istrintas!" << endl;
                } else {
                    cout << "Klaida trinant naudotoja!" << endl;
                }
            }
            else if (pasirinkimas == 4) {
                string pavadinimas;
                cin.ignore();
                if (!inputSuCancel("Iveskite grupes pavadinima", pavadinimas)) continue;

                Grupe nauja;
                nauja.setPavadinimas(pavadinimas);
                int newId = grupeRepo.save(nauja);
                cout << "Grupe sukurta sekmingai! ID: " << newId << endl;
            }
            else if (pasirinkimas == 5) {
                auto visos = grupeRepo.findAll();
                cout << "\n--- VISOS GRUPES ---" << endl;
                for (const auto& g : visos) {
                    cout << "Grupes ID: " << g.getId() << " | " << g.getPavadinimas() << endl;
                }
            }
            else if (pasirinkimas == 6) {
                auto visos = grupeRepo.findAll();
                cout << "\n--- VISOS GRUPES ---" << endl;
                for (const auto& g : visos) {
                    cout << "Grupes ID: " << g.getId() << " | " << g.getPavadinimas() << endl;
                }

                int id;
                if (!inputSuCancel("\nPasirinkite trinamos grupes ID", id)) continue;

                if (grupeRepo.remove(id)) {
                    cout << "Grupe istrinta!" << endl;
                } else {
                    cout << "Klaida trinant grupe!" << endl;
                }
            }
            else if (pasirinkimas == 7) {
                string pavadinimas;
                cin.ignore();
                cout << "Iveskite dalyko pavadinima (arba '0' - atgal): ";
                getline(cin, pavadinimas);
                if (pavadinimas == "0") {
                    cout << "Griztama i meniu..." << endl;
                    continue;
                }

                Dalykas naujas;
                naujas.setPavadinimas(pavadinimas);
                int newId = dalykasRepo.save(naujas);
                cout << "Dalykas sukurtas sekmingai! ID: " << newId << endl;
            }
            else if (pasirinkimas == 8) {
                auto visi = dalykasRepo.findAll();
                cout << "\n--- VISI DALYKAI ---" << endl;
                for (const auto& d : visi) {
                    cout << "ID: " << d.getId() << " | Pavadinimas: " << d.getPavadinimas() << endl;
                }
            }
            else if (pasirinkimas == 9) {
                auto visi = dalykasRepo.findAll();
                cout << "\n--- VISI DALYKAI ---" << endl;
                for (const auto& d : visi) {
                    cout << "Dalyko ID: " << d.getId() << " | " << d.getPavadinimas() << endl;
                }

                int id;
                if (!inputSuCancel("\nPasirinkite trinamo dalyko ID", id)) continue;

                if (dalykasRepo.remove(id)) {
                    cout << "Dalykas istrintas!" << endl;
                } else {
                    cout << "Klaida trinant dalyka!" << endl;
                }
            }
            else if (pasirinkimas == 10) {
                auto dalykai = dalykasRepo.findAll();
                cout << "\n--- VISI DALYKAI ---" << endl;
                for (const auto& d : dalykai) {
                    cout << "ID: " << d.getId() << " | " << d.getPavadinimas() << endl;
                }

                int dalyko_id;
                if (!inputSuCancel("\nIveskite dalyko ID", dalyko_id)) continue;

                auto destytojai = destytojaRepo.getAll();
                cout << "\n--- VISI DESTYTOJAI ---" << endl;
                for (const auto& dest : destytojai) {
                    cout << "ID: " << dest.first << " | " << dest.second << endl;
                }

                int destytojo_id;
                if (!inputSuCancel("\nIveskite destytojo ID", destytojo_id)) continue;

                if (dalykasRepo.assignToDestytojas(dalyko_id, destytojo_id)) {
                    cout << "Dalykas priskirtas destytojui sekmingai!" << endl;
                } else {
                    cout << "Klaida priskiriant dalyka!" << endl;
                }
            }
            else if (pasirinkimas == 11) {
                auto dalykai = dalykasRepo.findAll();
                cout << "\n--- VISI DALYKAI ---" << endl;
                for (const auto& d : dalykai) {
                    cout << "ID: " << d.getId() << " | " << d.getPavadinimas() << endl;
                }

                int dalyko_id;
                if (!inputSuCancel("\nIveskite dalyko ID", dalyko_id)) continue;

                auto grupes = grupeRepo.findAll();
                cout << "\n--- VISOS GRUPES ---" << endl;
                for (const auto& g : grupes) {
                    cout << "ID: " << g.getId() << " | " << g.getPavadinimas() << endl;
                }

                int grupes_id;
                if (!inputSuCancel("\nIveskite grupes ID", grupes_id)) continue;

                if (dalykasRepo.assignToGrupe(dalyko_id, grupes_id)) {
                    cout << "Dalykas priskirtas grupei sekmingai!" << endl;
                } else {
                    cout << "Klaida priskiriant dalyka!" << endl;
                }
            }
            else if (pasirinkimas == 12) {
                // Parodyti grupes dalykus
                auto grupes = grupeRepo.findAll();
                cout << "\n--- VISOS GRUPES ---" << endl;
                for (const auto& g : grupes) {
                    cout << "ID: " << g.getId() << " | " << g.getPavadinimas() << endl;
                }

                int grupes_id;
                if (!inputSuCancel("\nIveskite grupes ID", grupes_id)) continue;

                auto dalykai = grupeRepo.getDalykai(grupes_id);
                cout << "\n--- GRUPES DALYKAI ---" << endl;
                if (dalykai.empty()) {
                    cout << "Grupei nepriskirta jokiu dalyku." << endl;
                } else {
                    for (const auto& d : dalykai) {
                        cout << "ID: " << d.getId() << " | " << d.getPavadinimas() << endl;
                    }
                }
            }
            else if (pasirinkimas == 13) {
                auto destytojai = destytojaRepo.getAll();
                cout << "\n--- VISI DESTYTOJAI ---" << endl;
                for (const auto& dest : destytojai) {
                    cout << "ID: " << dest.first << " | " << dest.second << endl;
                }

                int destytojo_id;
                if (!inputSuCancel("\nIveskite destytojo ID", destytojo_id)) continue;

                auto dalykai = dalykasRepo.findByDestytojas(destytojo_id);
                cout << "\n--- DESTYTOJO DALYKAI ---" << endl;
                if (dalykai.empty()) {
                    cout << "Destytojui nepriskirta jokiu dalyku." << endl;
                } else {
                    for (const auto& d : dalykai) {
                        cout << "ID: " << d.getId() << " | " << d.getPavadinimas() << endl;
                    }
                }
            }
            else if (pasirinkimas == 14) {
                auto pazymiai = pazymysRepo.findAll();
                cout << "\n--- VISI PAZYMIAI ---" << endl;
                for (const auto& p : pazymiai) {
                    try {
                        Dalykas dalykas = dalykasRepo.findById(p.getDalykoId());
                        cout << "Pazymio ID: " << p.getId()
                                 << " | Studento ID: " << p.getStudentoId()
                                 << " | Dalykas: " << dalykas.getPavadinimas()
                                 << " | Reiksme: " << p.getReiksme()
                                 << " | Data: " << p.getData() << endl;
                    } catch(...) {}
                }

                int pazymio_id;
                if (!inputSuCancel("\nIveskite pazymio ID", pazymio_id)) continue;

                Pazymys pazymys = pazymysRepo.findById(pazymio_id);

                if (pazymys.getId() == 0) {
                    cout << "Pazymys nerastas!" << endl;
                    continue;
                }

                int nauja_reiksme;
                string nauja_data;
                if (!inputSuCancel("Iveskite nauja reiksme (1-10)", nauja_reiksme)) continue;
                cout << "Iveskite nauja data (YYYY-MM-DD) (arba '0' - atgal): ";
                cin >> nauja_data;
                if (nauja_data == "0") {
                    cout << "Griztama i meniu..." << endl;
                    continue;
                }

                pazymys.setReiksme(nauja_reiksme);
                pazymys.setData(nauja_data);

                if (pazymysRepo.update(pazymys)) {
                    cout << "Pazymys pakeistas sekmingai!" << endl;
                } else {
                    cout << "Klaida keiciant pazymi!" << endl;
                }
            }

        } catch (const LogineKlaida& e) {
            cout << "Klaida: " << e.getZinute() << endl;
        } catch (const NerastasKlaida& e) {
            cout << "Klaida: " << e.getZinute() << endl;
        } catch (const TechnineKlaida& e) {
            cout << "Sistemos klaida: " << e.getZinute() << endl;
        }

    } while (pasirinkimas != 0);
}

void destytojoMeniu(Destytojas& destytojas, DatabaseManager& db) {
    GrupeRepository grupeRepo(&db);
    DalykasRepository dalykasRepo(&db);
    PazymysRepository pazymysRepo(&db);
    StudentasRepository studentasRepo(&db);

    int pasirinkimas;
    do {
        cout << "\n=== DESTYTOJO MENIU ===" << endl;
        cout << "1. Ivesti pazymi" << endl;
        cout << "2. Perziureti studentu pazymius" << endl;
        cout << "0. Atsijungti" << endl;
        cout << "Pasirinkimas: ";
        cin >> pasirinkimas;

        try {
            if (pasirinkimas == 1) {
                auto grupes = grupeRepo.findAll();
                cout << "\n--- GRUPES ---" << endl;
                for (const auto& g : grupes) {
                    cout << "ID: " << g.getId() << " | " << g.getPavadinimas() << endl;
                }

                int grupes_id;
                if (!inputSuCancel("\nIveskite Grupes ID", grupes_id)) continue;

                auto studentai_ids = grupeRepo.getStudentai(grupes_id);

                if (studentai_ids.empty()) {
                    cout << "Grupeje studentu nera." << endl;
                    continue;
                }

                cout << "\n--- Grupes studentai ---" << endl;
                for (int stud_id : studentai_ids) {
                    string vardas_pavarde = studentasRepo.getStudentoVardasPavarde(stud_id);
                    cout << "Studento ID: " << stud_id << " | " << vardas_pavarde << endl;
                }

                int studento_id, dalyko_id, reiksme;
                string data;

                if (!inputSuCancel("\nIveskite studento ID", studento_id)) continue;

                auto dalykai = dalykasRepo.findByDestytojas(destytojas.getDestytojoId());
                cout << "\n--- Paziurekite reikiamo dalyko id ---" << endl;
                for (const auto& d : dalykai) {
                    cout << "ID: " << d.getId() << " | " << d.getPavadinimas() << endl;
                }

                if (!inputSuCancel("Iveskite dalyko ID", dalyko_id)) continue;
                if (!inputSuCancel("Pazymio reiksme (1-10)", reiksme)) continue;
                cout << "Data (MM-DD-YYYY) (arba '0' - atgal): ";
                cin >> data;
                if (data == "0") {
                    cout << "Griztama i meniu..." << endl;
                    continue;
                }

                Pazymys naujas;
                naujas.setStudentoId(studento_id);
                naujas.setDalykoId(dalyko_id);
                naujas.setDestytojoId(destytojas.getDestytojoId());
                naujas.setReiksme(reiksme);
                naujas.setData(data);

                if (!naujas.validateDate(data)) {
                    cout << "Netinkama data! Naudokite formata MM-DD-YYYY." << endl;
                    continue;
                }

                pazymysRepo.save(naujas);
                cout << "Pazymys ivestas sekmingai!" << endl;
            }
            else if (pasirinkimas == 2) {
                auto dalykai = dalykasRepo.findByDestytojas(destytojas.getDestytojoId());

                if (dalykai.empty()) {
                    cout << "Jums nepriskirta jokiu dalyku." << endl;
                    continue;
                }

                cout << "\n--- Jusu dalykai ---" << endl;
                for (size_t i = 0; i < dalykai.size(); i++) {
                    cout << i + 1 << ". " << dalykai[i].getPavadinimas() << endl;
                }

                int pasirink;
                if (!inputSuCancel("Pasirinkite dalyka", pasirink)) continue;

                if (pasirink < 1 || pasirink > (int)dalykai.size()) {
                    cout << "Neteisingas pasirinkimas!" << endl;
                    continue;
                }

                int dalyko_id = dalykai[pasirink - 1].getId();
                auto pazymiai = pazymysRepo.findByDalykas(dalyko_id);

                cout << "\n--- Studentu pazymiai ---" << endl;
                for (const auto& paz : pazymiai) {
                    cout << "Studento ID: " << paz.getStudentoId()
                             << " - Pazymys: " << paz.getReiksme()
                             << " - Data: " << paz.getData() << endl;
                }
            }

        } catch (const Klaida& e) {
            cout << "Klaida: " << e.getZinute() << endl;
        }

    } while (pasirinkimas != 0);
}

void studentoMeniu(Studentas& studentas, DatabaseManager& db) {
    GrupeRepository grupeRepo(&db);
    DalykasRepository dalykasRepo(&db);
    PazymysRepository pazymysRepo(&db);

    int pasirinkimas;
    do {
        cout << "\n=== STUDENTO MENIU ===" << endl;
        cout << "1. Perziureti mano pazymius" << endl;
        cout << "0. Atsijungti" << endl;
        cout << "Pasirinkimas: ";
        cin >> pasirinkimas;

        try {
            if (pasirinkimas == 1) {
                auto dalykai = grupeRepo.getDalykai(studentas.getGrupesId());

                if (dalykai.empty()) {
                    cout << "Jusu grupei nepriskirta jokiu dalyku." << endl;
                    continue;
                }

                cout << "\n--- Jusu grupes dalykai ---" << endl;
                for (const auto& d : dalykai) {
                    cout << "Dalyko ID: " << d.getId() << " | " << d.getPavadinimas() << endl;
                }

                int dalyko_id;
                if (!inputSuCancel("\nIveskite dalyko ID", dalyko_id)) continue;

                auto pazymiai = pazymysRepo.findByStudentAndDalykas(
                    studentas.getStudentoId(), dalyko_id
                );

                Dalykas dalykas = dalykasRepo.findById(dalyko_id);

                cout << "\n--- Jusu pazymiai is dalyko: " << dalykas.getPavadinimas() << " ---" << endl;
                if (pazymiai.empty()) {
                    cout << "Pazymiu nera." << endl;
                } else {
                    for (const auto& p : pazymiai) {
                        string dest_vardas = pazymysRepo.getDestytojoVardas(p.getDestytojoId());
                        cout << "Pazymys: " << p.getReiksme()
                                 << " | Data: " << p.getData()
                                 << " | Destytojas: " << dest_vardas << endl;
                    }
                }
            }

        } catch (const Klaida& e) {
            cout << "Klaida: " << e.getZinute() << endl;
        }

    } while (pasirinkimas != 0);
}