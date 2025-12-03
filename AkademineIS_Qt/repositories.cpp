#include "repositories.h"
#include "exceptions.h"
#include "sqlite3.h"
#include <QDebug>
#include <memory>

NaudotojasRepository::NaudotojasRepository(DatabaseManager* database) : db(database) {}

int NaudotojasRepository::save(const Naudotojas& user) {
    user.validate();
    std::string sql = "INSERT INTO naudotojas (vardas, pavarde, role, prisijungimo_vardas, slaptazodis) "
                 "VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw TechnineKlaida("Nepavyko paruosti SQL");
    }
    sqlite3_bind_text(stmt, 1, user.getVardas().toStdString().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, user.getPavarde().toStdString().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, user.getRole().toStdString().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, user.getPrisijungimoVardas().toStdString().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, user.getSlaptazodis().toStdString().c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw TechnineKlaida("Nepavyko issaugoti naudotojo");
    }

    int newId = sqlite3_last_insert_rowid(db->getDb());
    sqlite3_finalize(stmt);
    return newId;
}


bool NaudotojasRepository::remove(int id) {
    std::string sql = "DELETE FROM naudotojas WHERE id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    sqlite3_bind_int(stmt, 1, id);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

QVector<Naudotojas> NaudotojasRepository::findAll() {
    QVector<Naudotojas> naudotojai;
    std::string sql = "SELECT id, vardas, pavarde, role FROM naudotojas;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Naudotojas user;
            user.setId(sqlite3_column_int(stmt, 0));
            user.setVardas(QString::fromUtf8((const char*)sqlite3_column_text(stmt, 1)));
            user.setPavarde(QString::fromUtf8((const char*)sqlite3_column_text(stmt, 2)));
            user.setRole(QString::fromUtf8((const char*)sqlite3_column_text(stmt, 3)));
            naudotojai.push_back(user);
        }
        sqlite3_finalize(stmt);
    }
    return naudotojai;
}

std::shared_ptr<Naudotojas> NaudotojasRepository::authenticate(const QString& username, const QString& password) {
    std::string sql = "SELECT id, vardas, pavarde, role FROM naudotojas "
                 "WHERE prisijungimo_vardas = ? AND slaptazodis = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return nullptr;
    }
    sqlite3_bind_text(stmt, 1, username.toStdString().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, password.toStdString().c_str(), -1, SQLITE_TRANSIENT);
    std::shared_ptr<Naudotojas> user = nullptr;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int naud_id = sqlite3_column_int(stmt, 0);
        QString vardas = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 1));
        QString pavarde = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 2));
        QString role = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 3));
        sqlite3_finalize(stmt);
        if (role == "admin") {
            auto admin = std::make_shared<Admin>();
            
            admin->setId(naud_id);
            admin->setVardas(vardas);
            admin->setPavarde(pavarde);
            admin->setRole(role);
            
            return admin;
        }
        else if (role == "destytojas") {
            std::string sql2 = "SELECT id FROM destytojas WHERE naudotojo_id = ?;";
            sqlite3_stmt* stmt2;
            if (sqlite3_prepare_v2(db->getDb(), sql2.c_str(), -1, &stmt2, nullptr) == SQLITE_OK) {
                sqlite3_bind_int(stmt2, 1, naud_id);
                int dest_id = 0;
                if (sqlite3_step(stmt2) == SQLITE_ROW) {
                    dest_id = sqlite3_column_int(stmt2, 0);
                }
                sqlite3_finalize(stmt2);
                
                auto destytojas = std::make_shared<Destytojas>();
                
                destytojas->setId(naud_id);
                destytojas->setVardas(vardas);
                destytojas->setPavarde(pavarde);
                destytojas->setRole(role);
                destytojas->setDestytojoId(dest_id);
                
                return destytojas;
            }
        }
        else if (role == "studentas") {
            std::string sql3 = "SELECT id, grupes_id FROM studentas WHERE naudotojo_id = ?;";
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
                
                auto studentas = std::make_shared<Studentas>();
                
                studentas->setId(naud_id);
                studentas->setVardas(vardas);
                studentas->setPavarde(pavarde);
                studentas->setRole(role);
                studentas->setStudentoId(stud_id);
                studentas->setGrupesId(grupe_id);
                
                return studentas;
            }
        }
    } else {
        sqlite3_finalize(stmt);
    }
    return nullptr;
}

GrupeRepository::GrupeRepository(DatabaseManager* database) : db(database) {}

int GrupeRepository::save(const Grupe& grupe) {
    grupe.validate();
    std::string sql = "INSERT INTO grupe (pavadinimas) VALUES (?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw TechnineKlaida("Nepavyko paruosti SQL");
    }
    sqlite3_bind_text(stmt, 1, grupe.getPavadinimas().toStdString().c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw TechnineKlaida("Nepavyko issaugoti grupe");
    }
    int newId = sqlite3_last_insert_rowid(db->getDb());
    sqlite3_finalize(stmt);
    return newId;
}

bool GrupeRepository::remove(int id) {
    std::string sql = "DELETE FROM grupe WHERE id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    sqlite3_bind_int(stmt, 1, id);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

QVector<Grupe> GrupeRepository::findAll() {
    QVector<Grupe> grupes;
    std::string sql = "SELECT id, pavadinimas FROM grupe;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            QString pav = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 1));
            
            Grupe g;
            g.setId(id);
            g.setPavadinimas(pav);
            
            grupes.push_back(g);
        }
        sqlite3_finalize(stmt);
    }
    return grupes;
}

QVector<int> GrupeRepository::getStudentai(int grupes_id) {
    QVector<int> studentai;
    std::string sql = "SELECT id FROM studentas WHERE grupes_id = ?;";
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

QVector<Dalykas> GrupeRepository::getDalykai(int grupes_id) {
    QVector<Dalykas> dalykai;
    std::string sql = "SELECT d.id, d.pavadinimas FROM dalykas d "
                 "JOIN GrupesDalykai gd ON d.id = gd.dalyko_id "
                 "WHERE gd.grupes_id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, grupes_id);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            QString pav = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 1));
            
            Dalykas d;
            d.setId(id);
            d.setPavadinimas(pav);
            
            dalykai.push_back(d);
        }
        sqlite3_finalize(stmt);
    }
    return dalykai;
}

QVector<Grupe> GrupeRepository::getGrupesByDalykas(int dalyko_id) {
    QVector<Grupe> grupes;
    std::string sql = "SELECT g.id, g.pavadinimas FROM grupe g "
                 "JOIN GrupesDalykai gd ON g.id = gd.grupes_id "
                 "WHERE gd.dalyko_id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, dalyko_id);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Grupe g;
            g.setId(sqlite3_column_int(stmt, 0));
            g.setPavadinimas(QString::fromUtf8((const char*)sqlite3_column_text(stmt, 1)));
            grupes.push_back(g);
        }
        sqlite3_finalize(stmt);
    }
    return grupes;
}

DalykasRepository::DalykasRepository(DatabaseManager* database) : db(database) {}

int DalykasRepository::save(const Dalykas& dalykas) {
    dalykas.validate();
    std::string sql = "INSERT INTO dalykas (pavadinimas) VALUES (?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw TechnineKlaida("Nepavyko paruosti SQL");
    }
    sqlite3_bind_text(stmt, 1, dalykas.getPavadinimas().toStdString().c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw TechnineKlaida("Nepavyko issaugoti dalykas");
    }
    int newId = sqlite3_last_insert_rowid(db->getDb());
    sqlite3_finalize(stmt);
    return newId;
}

bool DalykasRepository::remove(int id) {
    std::string sql = "DELETE FROM dalykas WHERE id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, id);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

Dalykas DalykasRepository::findById(int id) {
    std::string sql = "SELECT id, pavadinimas FROM dalykas WHERE id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            Dalykas d;
            d.setId(sqlite3_column_int(stmt, 0));
            d.setPavadinimas(QString::fromUtf8((const char*)sqlite3_column_text(stmt, 1)));
            return d;
        }
        sqlite3_finalize(stmt);
    }
    throw NerastasKlaida("Dalykas");
}

QVector<Dalykas> DalykasRepository::findAll() {
    QVector<Dalykas> dalykai;
    std::string sql = "SELECT id, pavadinimas FROM dalykas;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            QString pav = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 1));
            
            Dalykas d;
            d.setId(id);
            d.setPavadinimas(pav);
            
            dalykai.push_back(d);
        }
        sqlite3_finalize(stmt);
    }
    return dalykai;
}

QVector<Dalykas> DalykasRepository::findByDestytojas(int destytojo_id) {
    QVector<Dalykas> dalykai;
    std::string sql = "SELECT d.id, d.pavadinimas FROM dalykas d "
                 "JOIN DestytojoDalykai dd ON d.id = dd.dalyko_id "
                 "WHERE dd.destytojo_id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, destytojo_id);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            QString pav = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 1));
            
            Dalykas d;
            d.setId(id);
            d.setPavadinimas(pav);
            
            dalykai.push_back(d);
        }
        sqlite3_finalize(stmt);
    }
    return dalykai;
}

bool DalykasRepository::assignToDestytojas(int dalyko_id, int destytojo_id) {
    std::string sql = "INSERT INTO DestytojoDalykai (destytojo_id, dalyko_id) VALUES (?, ?);";
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

bool DalykasRepository::assignToGrupe(int dalyko_id, int grupes_id) {
    std::string sql = "INSERT INTO GrupesDalykai (grupes_id, dalyko_id) VALUES (?, ?);";
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

QVector<QPair<int, QString>> DalykasRepository::getDestytojai(int dalyko_id) {
    QVector<QPair<int, QString>> destytojai;
    std::string sql = "SELECT d.id, n.vardas, n.pavarde FROM destytojas d "
                 "JOIN naudotojas n ON d.naudotojo_id = n.id "
                 "JOIN DestytojoDalykai dd ON d.id = dd.destytojo_id "
                 "WHERE dd.dalyko_id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, dalyko_id);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            QString vardas = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 1));
            QString pavarde = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 2));
            QString pilnas_vardas = vardas + " " + pavarde;
            destytojai.push_back(qMakePair(id, pilnas_vardas));
        }
        sqlite3_finalize(stmt);
    }
    return destytojai;
}

bool DalykasRepository::unassignFromDestytojas(int dalyko_id, int destytojo_id) {
    std::string sql = "DELETE FROM DestytojoDalykai WHERE destytojo_id = ? AND dalyko_id = ?;";
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

bool DalykasRepository::unassignFromGrupe(int dalyko_id, int grupes_id) {
    std::string sql = "DELETE FROM GrupesDalykai WHERE grupes_id = ? AND dalyko_id = ?;";
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

PazymysRepository::PazymysRepository(DatabaseManager* database) : db(database) {}

int PazymysRepository::save(const Pazymys& pazymys) {
    pazymys.validate();
    std::string sql = "INSERT INTO pazymys (studento_id, dalyko_id, destytojo_id, reiksme, data) "
                 "VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw TechnineKlaida("Nepavyko paruosti SQL");
    }
    sqlite3_bind_int(stmt, 1, pazymys.getStudentoId());
    sqlite3_bind_int(stmt, 2, pazymys.getDalykoId());
    sqlite3_bind_int(stmt, 3, pazymys.getDestytojoId());
    sqlite3_bind_int(stmt, 4, pazymys.getReiksme());
    sqlite3_bind_text(stmt, 5, pazymys.getData().toStdString().c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw TechnineKlaida("Nepavyko issaugoti pazymys");
    }

    int newId = sqlite3_last_insert_rowid(db->getDb());
    sqlite3_finalize(stmt);
    return newId;
}

bool PazymysRepository::update(const Pazymys& pazymys) {
    pazymys.validate();
    std::string sql = "UPDATE pazymys SET reiksme = ?, data = ? WHERE id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    sqlite3_bind_int(stmt, 1, pazymys.getReiksme());
    sqlite3_bind_text(stmt, 2, pazymys.getData().toStdString().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, pazymys.getId());

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

bool PazymysRepository::remove(int id) {
    std::string sql = "DELETE FROM pazymys WHERE id = ?;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    sqlite3_bind_int(stmt, 1, id);
    
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

Pazymys PazymysRepository::findById(int id) {
    std::string sql = "SELECT id, studento_id, dalyko_id, destytojo_id, reiksme, data "
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
            p.setData(QString::fromUtf8((const char*)sqlite3_column_text(stmt, 5)));
            sqlite3_finalize(stmt);
            return p;
        }
        sqlite3_finalize(stmt);
    }
    Pazymys empty;
    return empty;
}

QVector<Pazymys> PazymysRepository::findAll() {
    QVector<Pazymys> pazymiai;
    std::string sql = "SELECT id, studento_id, dalyko_id, destytojo_id, reiksme, data FROM pazymys;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            int sid = sqlite3_column_int(stmt, 1);
            int did = sqlite3_column_int(stmt, 2);
            int destId = sqlite3_column_int(stmt, 3);
            int reiksme = sqlite3_column_int(stmt, 4);
            QString data = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 5));
            
            Pazymys paz;
            paz.setId(id);
            paz.setStudentoId(sid);
            paz.setDalykoId(did);
            paz.setDestytojoId(destId);
            paz.setReiksme(reiksme);
            paz.setData(data);
            
            pazymiai.push_back(paz);
        }
        sqlite3_finalize(stmt);
    }
    return pazymiai;
}

QVector<Pazymys> PazymysRepository::findByStudentAndDalykas(int studento_id, int dalyko_id) {
    QVector<Pazymys> pazymiai;
    std::string sql = "SELECT id, studento_id, dalyko_id, destytojo_id, reiksme, data "
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
            QString data = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 5));
            
            Pazymys paz;
            paz.setId(id);
            paz.setStudentoId(sid);
            paz.setDalykoId(did);
            paz.setDestytojoId(destId);
            paz.setReiksme(reiksme);
            paz.setData(data);
            
            pazymiai.push_back(paz);
        }
        sqlite3_finalize(stmt);
    }
    return pazymiai;
}

QVector<Pazymys> PazymysRepository::findByDalykas(int dalyko_id) {
    QVector<Pazymys> pazymiai;
    std::string sql = "SELECT id, studento_id, dalyko_id, destytojo_id, reiksme, data "
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
            QString data = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 5));

            Pazymys paz;
            paz.setId(id);
            paz.setStudentoId(sid);
            paz.setDalykoId(did);
            paz.setDestytojoId(destId);
            paz.setReiksme(reiksme);
            paz.setData(data);
            
            pazymiai.push_back(paz);
        }
        sqlite3_finalize(stmt);
    }
    return pazymiai;
}

QVector<Pazymys> PazymysRepository::findByDalykasAndGrupe(int dalyko_id, int grupes_id) {
    QVector<Pazymys> pazymiai;
    std::string sql = "SELECT p.id, p.studento_id, p.dalyko_id, p.destytojo_id, p.reiksme, p.data "
                 "FROM pazymys p "
                 "JOIN studentas s ON p.studento_id = s.id "
                 "WHERE p.dalyko_id = ? AND s.grupes_id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, dalyko_id);
        sqlite3_bind_int(stmt, 2, grupes_id);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Pazymys paz;
            paz.setId(sqlite3_column_int(stmt, 0));
            paz.setStudentoId(sqlite3_column_int(stmt, 1));
            paz.setDalykoId(sqlite3_column_int(stmt, 2));
            paz.setDestytojoId(sqlite3_column_int(stmt, 3));
            paz.setReiksme(sqlite3_column_int(stmt, 4));
            paz.setData(QString::fromUtf8((const char*)sqlite3_column_text(stmt, 5)));
            
            pazymiai.push_back(paz);
        }
        sqlite3_finalize(stmt);
    }
    return pazymiai;
}

QString PazymysRepository::getDestytojoVardas(int destytojo_id) {
    std::string sql = "SELECT n.vardas, n.pavarde FROM naudotojas n "
                 "JOIN destytojas d ON n.id = d.naudotojo_id "
                 "WHERE d.id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, destytojo_id);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            QString vardas = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 0));
            QString pavarde = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 1));
            sqlite3_finalize(stmt);
            return vardas + " " + pavarde;
        }
        sqlite3_finalize(stmt);
    }
    return "Nezinomas";
}

DestytojaRepository::DestytojaRepository(DatabaseManager* database) : db(database) {}

QVector<QPair<int, QString>> DestytojaRepository::getAll() {
    QVector<QPair<int, QString>> destytojai;
    std::string sql = "SELECT d.id, n.vardas, n.pavarde FROM destytojas d "
                 "JOIN naudotojas n ON d.naudotojo_id = n.id;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            QString vardas = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 1));
            QString pavarde = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 2));
            destytojai.push_back(qMakePair(id, vardas + " " + pavarde));
        }
        sqlite3_finalize(stmt);
    }
    return destytojai;
}

int DestytojaRepository::createDestytojas(int naudotojo_id) {
    std::string sql = "INSERT INTO destytojas (naudotojo_id) VALUES (?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw TechnineKlaida("Nepavyko paruosti SQL");
    }
    sqlite3_bind_int(stmt, 1, naudotojo_id);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw TechnineKlaida("Nepavyko sukurti destytoja");
    }
    int newId = sqlite3_last_insert_rowid(db->getDb());
    sqlite3_finalize(stmt);
    return newId;
}

StudentasRepository::StudentasRepository(DatabaseManager* database) : db(database) {}

QVector<QPair<int, QString>> StudentasRepository::getAll() {
    QVector<QPair<int, QString>> studentai;
    std::string sql = "SELECT s.id, n.vardas, n.pavarde FROM studentas s "
                 "JOIN naudotojas n ON s.naudotojo_id = n.id;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            QString vardas = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 1));
            QString pavarde = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 2));
            studentai.push_back(qMakePair(id, vardas + " " + pavarde));
        }
        sqlite3_finalize(stmt);
    }
    return studentai;
}

QString StudentasRepository::getStudentoVardasPavarde(int studento_id) {
    std::string sql = "SELECT n.vardas, n.pavarde FROM naudotojas n "
                 "JOIN studentas s ON n.id = s.naudotojo_id "
                 "WHERE s.id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, studento_id);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            QString vardas = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 0));
            QString pavarde = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 1));
            sqlite3_finalize(stmt);
            return vardas + " " + pavarde;
        }
        sqlite3_finalize(stmt);
    }
    return "Nezinomas";
}

int StudentasRepository::createStudentas(int naudotojo_id, int grupes_id) {
    std::string sql = "INSERT INTO studentas (naudotojo_id, grupes_id) VALUES (?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw TechnineKlaida("Nepavyko paruosti SQL");
    }
    sqlite3_bind_int(stmt, 1, naudotojo_id);
    sqlite3_bind_int(stmt, 2, grupes_id);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw TechnineKlaida("Nepavyko sukurti studenta");
    }
    int newId = sqlite3_last_insert_rowid(db->getDb());
    sqlite3_finalize(stmt);
    return newId;
}

QVector<QPair<int, QString>> StudentasRepository::getStudentsByGrupe(int grupes_id) {
    QVector<QPair<int, QString>> studentai;
    std::string sql = "SELECT s.id, n.vardas, n.pavarde FROM studentas s "
                 "JOIN naudotojas n ON s.naudotojo_id = n.id "
                 "WHERE s.grupes_id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db->getDb(), sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, grupes_id);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            QString vardas = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 1));
            QString pavarde = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 2));
            studentai.push_back(qMakePair(id, vardas + " " + pavarde));
        }
        sqlite3_finalize(stmt);
    }
    return studentai;
}
