#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <ctime>
#include <fcntl.h> // Для _setmode
#include <io.h>    // Для _setmode

using namespace std;

// ---------- СТРУКТУРА ----------
struct Violation {
    wstring description; // Використовуємо wstring для Unicode
    double fine;
    tm date;
    bool paid = false;
};

// ---------- ФУНКЦІЇ ----------

// Додавання порушення
void addViolation(map<wstring, vector<Violation>>& db, const wstring& carNumber, const Violation& v) {
    db[carNumber].push_back(v);
}

// Друк усієї бази
void printAll(const map<wstring, vector<Violation>>& db) {
    if (db.empty()) {
        wcout << L"База даних порожня." << endl;
        return;
    }
    for (const auto& pair : db) {
        wcout << L"Номер машини: " << pair.first << endl;
        for (const auto& v : pair.second) {
            wcout << L"  Порушення: " << v.description
                << L" | Штраф: " << v.fine << L" грн"
                << L" | Оплачено: " << (v.paid ? L"Так" : L"Ні")
                << L" | Дата: " << v.date.tm_mday << L"." << v.date.tm_mon + 1 << L"." << v.date.tm_year + 1900
                << endl;
        }
        wcout << L"--------------------------" << endl;
    }
}

// Сума несплачених штрафів
double totalFine(const map<wstring, vector<Violation>>& db, const wstring& carNumber) {
    double sum = 0;
    auto it = db.find(carNumber);
    if (it != db.end()) {
        for (const auto& v : it->second) {
            if (!v.paid) sum += v.fine;
        }
    }
    return sum;
}

// Оплата всіх штрафів
void payFines(map<wstring, vector<Violation>>& db, const wstring& carNumber) {
    auto it = db.find(carNumber);
    if (it == db.end()) return;

    for (auto& v : it->second) {
        v.paid = true;
    }
    wcout << L"Усі штрафи для " << carNumber << L" оплачено." << endl;
}

// ---------- MAIN ----------
int main() {
    // Налаштування консолі для виводу Unicode (UTF-16)
    // Це прибере "знаки питання" замість літер
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);

    map<wstring, vector<Violation>> database;

    // Створення дати: 10 травня 2024 року
    tm date1 = { 0 };
    date1.tm_year = 2024 - 1900;
    date1.tm_mon = 4;
    date1.tm_mday = 10;

    // Додавання даних (використовуємо префікс L для Unicode рядків)
    addViolation(database, L"AA1234BB", { L"Перевищення швидкості", 510, date1 });
    addViolation(database, L"AA1234BB", { L"Паркування в недозволеному місці", 340, date1 });
    addViolation(database, L"BC5566CI", { L"Проїзд на червоне світло", 680, date1 });

    // Вивід бази
    printAll(database);

    // Робота зі штрафами
    wstring searchNum = L"AA1234BB";
    wcout << L"Загальна сума несплачених штрафів для " << searchNum << L": "
        << totalFine(database, searchNum) << L" грн" << endl;

    wcout << L"\nПроводимо оплату..." << endl;
    payFines(database, searchNum);

    wcout << L"Нова сума до сплати: " << totalFine(database, searchNum) << L" грн" << endl;

    return 0;
}