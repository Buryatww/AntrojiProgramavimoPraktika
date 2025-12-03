#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <QString>
#include <exception>

class Klaida : public std::exception {
protected:
    QString zinute;
public:
    explicit Klaida(const QString& z) : zinute(z) {}
    QString getZinute() const { return zinute; }
    const char* what() const noexcept override {
        return zinute.toStdString().c_str();
    }
};

class LogineKlaida : public Klaida {
public:
    explicit LogineKlaida(const QString& z) 
        : Klaida("Neteisingi duomenys: " + z) {}
};

class TechnineKlaida : public Klaida {
public:
    explicit TechnineKlaida(const QString& z) 
        : Klaida("Sistemos klaida: " + z) {}
};

class NerastasKlaida : public Klaida {
public:
    explicit NerastasKlaida(const QString& objektas) 
        : Klaida(objektas + " nerastas") {}
};

#endif // EXCEPTIONS_H
