#ifndef REPOSITORIES_H
#define REPOSITORIES_H

#include <QVector>
#include <QPair>
#include <QString>
#include <memory>
#include "sqlite3.h"
#include "models.h"

template <typename T>
class IRepository {
public:
    virtual ~IRepository() = default;
    virtual int save(const T& entity) = 0;
    virtual bool remove(int id) = 0;
};


class NaudotojasRepository : public IRepository<Naudotojas> {
private:
    DatabaseManager* db;
public:
    explicit NaudotojasRepository(DatabaseManager* database);
    int save(const Naudotojas& user) override;
    bool remove(int id) override;
    QVector<Naudotojas> findAll();
    std::shared_ptr<Naudotojas> authenticate(const QString& username, const QString& password);
};


class GrupeRepository : public IRepository<Grupe> {
private:
    DatabaseManager* db;
public:
    explicit GrupeRepository(DatabaseManager* database);
    int save(const Grupe& grupe) override;
    bool remove(int id) override;
    QVector<Grupe> findAll();
    QVector<int> getStudentai(int grupes_id);
    QVector<Dalykas> getDalykai(int grupes_id);
    QVector<Grupe> getGrupesByDalykas(int dalyko_id);
};


class DalykasRepository : public IRepository<Dalykas> {
private:
    DatabaseManager* db;
public:
    explicit DalykasRepository(DatabaseManager* database);
    int save(const Dalykas& dalykas) override;
    bool remove(int id) override;
    Dalykas findById(int id);
    QVector<Dalykas> findAll();
    QVector<Dalykas> findByDestytojas(int destytojo_id);
    bool assignToDestytojas(int dalyko_id, int destytojo_id);
    bool assignToGrupe(int dalyko_id, int grupes_id);
    bool unassignFromDestytojas(int dalyko_id, int destytojo_id);
    bool unassignFromGrupe(int dalyko_id, int grupes_id);
    QVector<QPair<int, QString>> getDestytojai(int dalyko_id);
};

class PazymysRepository : public IRepository<Pazymys> {
private:
    DatabaseManager* db;
public:
    explicit PazymysRepository(DatabaseManager* database);
    int save(const Pazymys& pazymys) override;
    bool update(const Pazymys& pazymys);
    bool remove(int id) override;
    Pazymys findById(int id);
    QVector<Pazymys> findAll();
    QVector<Pazymys> findByStudentAndDalykas(int studento_id, int dalyko_id);
    QVector<Pazymys> findByDalykas(int dalyko_id);
    QVector<Pazymys> findByDalykasAndGrupe(int dalyko_id, int grupes_id);
    QString getDestytojoVardas(int destytojo_id);
};


class DestytojaRepository {
private:
    DatabaseManager* db;
public:
    explicit DestytojaRepository(DatabaseManager* database);
    QVector<QPair<int, QString>> getAll();
    int createDestytojas(int naudotojo_id);
};


class StudentasRepository {
private:
    DatabaseManager* db;
public:
    explicit StudentasRepository(DatabaseManager* database);
    QVector<QPair<int, QString>> getAll();
    QString getStudentoVardasPavarde(int studento_id);
    int createStudentas(int naudotojo_id, int grupes_id);
    QVector<QPair<int, QString>> getStudentsByGrupe(int grupes_id);
};

#endif // REPOSITORIES_H
