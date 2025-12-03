#ifndef MODELS_H
#define MODELS_H

#include <QString>
#include <QDate>
#include "sqlite3.h"
#include "exceptions.h"


class DatabaseManager {
private:
    sqlite3* db;
    QString dbPath;
public:
    explicit DatabaseManager(const QString& path);
    ~DatabaseManager();
    bool connect();
    sqlite3* getDb() { return db; }
};


class Naudotojas {
protected:
    int id;
    QString vardas;
    QString pavarde;
    QString role;
    QString prisijungimo_vardas;
    QString slaptazodis;
public:
    Naudotojas();
    virtual ~Naudotojas() {}
    
    int getId() const { return id; }
    QString getVardas() const { return vardas; }
    QString getPavarde() const { return pavarde; }
    QString getRole() const { return role; }
    QString getPrisijungimoVardas() const { return prisijungimo_vardas; }
    QString getSlaptazodis() const { return slaptazodis; }
    
    void setId(int newId) { id = newId; }
    void setVardas(const QString& v) { vardas = v; prisijungimo_vardas = v; }
    void setPavarde(const QString& p) { pavarde = p; slaptazodis = p; }
    void setRole(const QString& r) { role = r; }
    
    bool validate() const;
};


class Admin : public Naudotojas {
public:
    Admin();
};


class Destytojas : public Naudotojas {
private:
    int destytojo_id;
public:
    Destytojas();
    
    int getDestytojoId() const { return destytojo_id; }
    void setDestytojoId(int id) { destytojo_id = id; }

};


class Studentas : public Naudotojas {
private:
    int studento_id;
    int grupes_id;
public:
    Studentas();
    
    int getStudentoId() const { return studento_id; }
    int getGrupesId() const { return grupes_id; }
    void setStudentoId(int id) { studento_id = id; }
    void setGrupesId(int id) { grupes_id = id; }
};


class Grupe {
private:
    int id;
    QString pavadinimas;
public:
    Grupe();
    
    int getId() const { return id; }
    QString getPavadinimas() const { return pavadinimas; }
    void setId(int newId) { id = newId; }
    void setPavadinimas(const QString& pav) { pavadinimas = pav; }
    bool validate() const;
};


class Dalykas {
private:
    int id;
    QString pavadinimas;
public:
    Dalykas();
    
    int getId() const { return id; }
    QString getPavadinimas() const { return pavadinimas; }
    void setId(int newId) { id = newId; }
    void setPavadinimas(const QString& pav) { pavadinimas = pav; }
    bool validate() const;
};


class Pazymys {
private:
    int id;
    int studento_id;
    int dalyko_id;
    int destytojo_id;
    int reiksme;
    QString data;
public:
    Pazymys();
    
    int getId() const { return id; }
    int getStudentoId() const { return studento_id; }
    int getDalykoId() const { return dalyko_id; }
    int getDestytojoId() const { return destytojo_id; }
    int getReiksme() const { return reiksme; }
    QString getData() const { return data; }
    
    void setId(int newId) { id = newId; }
    void setStudentoId(int sid) { studento_id = sid; }
    void setDalykoId(int did) { dalyko_id = did; }
    void setDestytojoId(int did) { destytojo_id = did; }
    void setReiksme(int r) { reiksme = r; }
    void setData(const QString& d) { data = d; }
    
    bool validate() const;
    bool validateDate(const QString& dateStr) const;
};

#endif // MODELS_H
