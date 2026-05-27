module;
import Spell__Lab2_;
import Sorcerer__Lab3__;
#include <iostream>
#include <fstream>
#include <functional>
#include <random>
#include <string>
#include <algorithm>
#include <iterator>
#include <vector>
#include <map>
#include <chrono>
#include <array>
#include <Windows.h>
export module Tools;

// Очистка состояния потока ввода после ошибки
export void clearInputStream()
{
    std::cin.clear();
    std::cin.ignore(std::cin.rdbuf()->in_avail());
}

// Ввод целого числа с проверкой корректности
export int readInt(const std::string& prompt = "")
{
    int value{};
    std::cout << prompt;
    while (!(std::cin >> value)) {
        std::cout << "Ошибка: введите целое число: ";
        clearInputStream();
    }
    clearInputStream();
    return value;
}

// Ввод целого числа в заданном диапазоне [min, max]
export int readIntInRange(const std::string& prompt, int min_val, int max_val)
{
    int value{};
    do {
        std::cout << prompt << " [" << min_val << ".." << max_val << "]: ";
        if (std::cin >> value && value >= min_val && value <= max_val) {
            clearInputStream();
            return value;
        }
        std::cout << "Ошибка: значение вне диапазона.\n";
        clearInputStream();
    } while (true);
}

// Ввод непустой строки с консоли
export std::string readNonEmptyString(const std::string& prompt)
{
    std::string result;
    do {
        std::cout << prompt;
        std::getline(std::cin >> std::ws, result);
    } while (result.empty());
    return result;
}

// Ввод имени файла с проверкой расширения .txt
export std::string readFilename()
{
    std::string filename;
    const std::string ext = ".txt";

    do {
        std::cout << "Имя файла: ";
        std::getline(std::cin >> std::ws, filename);
        if (filename.length() >= ext.length() &&
            filename.compare(filename.length() - ext.length(), ext.length(), ext) == 0) {
            return filename;
        }
        std::cout << "Ошибка: имя файла должно заканчиваться на .txt\n";
    } while (true);
}

// Ввод стихии через меню выбора
export Element readElement()
{
    while (true) {
        std::cout << "\nВыберите стихию:\n";
        std::cout << "1 - Вода\n";
        std::cout << "2 - Земля\n";
        std::cout << "3 - Воздух\n";
        std::cout << "4 - Огонь\n";

        int choice = readInt("Ваш выбор: ");
        if (choice >= 1 && choice <= 4) {
            return static_cast<Element>(choice);
        }
        std::cout << "Ошибка: выберите число от 1 до 4.\n";
    }
}

// Ввод времени в формате ЧЧ:ММ
export std::chrono::minutes readTime()
{
    while (true) {
        std::cout << "Введите время (ЧЧ:ММ): ";
        int h, m;
        char colon;
        if (std::cin >> h >> colon >> m && colon == ':' && isValidTime(h, m)) {
            clearInputStream();
            return std::chrono::hours(h) + std::chrono::minutes(m);
        }
        std::cout << "Ошибка: неверный формат времени.\n";
        clearInputStream();
    }
}

// Ввод полного заклинания с консоли
export Spell readSpellFromConsole()
{
    Spell spell;

    std::cout << "\n--- Добавление заклинания ---\n";
    spell.setName(readNonEmptyString("Название: "));
    spell.setDescription(readNonEmptyString("Описание: "));
    spell.setElement(readElement());
    spell.setMana(readIntInRange("Мана", 0, 10000));
    spell.setPower(readIntInRange("Сила", 0, 1000));
    spell.setDurationMinutes(readIntInRange("Продолжительность (мин)", 0, 3600));
    spell.setMinLevel(readIntInRange("Мин. уровень чародея", 1, 100));

    while (true) {
        auto start = readTime();
        auto end = readTime();
        spell.setTimeRange(start, end);

        if (start > end) {
            std::cout << "Внимание: интервал пересекает полночь.\n";
            std::cout << "Подтвердить? (y/n): ";
            char confirm;
            std::cin >> confirm;
            clearInputStream();
            if (confirm == 'y' || confirm == 'Y') { break; }
        }
        else {
            break;
        }
    }

    return spell;
}

// Пространство имён с предикатами и компараторами для фильтрации и сортировки
export namespace filters
{
    // Компаратор для сортировки по названию в алфавитном порядке
    /*auto byName = [](const Spell& a, const Spell& b) {
        return a.getName() < b.getName();
        };*/

    auto byName() {
        return [](const Spell& a, const Spell& b) {
            return a.getName() < b.getName();
            };
    }

    // Компаратор для сортировки по минимальному уровню по возрастанию
    /*auto byMinLevel = [](const Spell& a, const Spell& b) {
        return a.getMinLevel() < b.getMinLevel();
        };*/

    auto byMinLevel() {
        return [](const Spell& a, const Spell& b) {
            return a.getMinLevel() < b.getMinLevel();
            };
    }

    // Предикат для выбора заклинаний по диапазону названий (включительно)
    auto nameRange(const std::string& from, const std::string& to) {
        return [=](const Spell& s) {
            return s.getName() >= from && s.getName() <= to;
            };
    }

    // Предикат для выбора по диапазону затрат маны
    auto manaRange(int min, int max) {
        return [=](const Spell& s) {
            return s.getMana() >= min && s.getMana() <= max;
            };
    }

    // Предикат для выбора заклинаний определённой стихии
    auto byElement(Element elem) {
        return [=](const Spell& s) {
            return s.getElement() == elem;
            };
    }

    // Предикат для выбора заклинаний, доступных на заданном уровне
    auto availableAtLevel(int level) {
        return [=](const Spell& s) {
            return s.getMinLevel() <= level;
            };
    }

    // Предикат для выбора заклинаний, доступных в указанное время
    auto atTime(std::chrono::minutes time) {
        return [=](const Spell& s) {
            return s.isAvailableAtTime(time);
            };
    }
}

// Вывод результатов фильтрации на экран с информативными сообщениями
export void displayFiltered(const SpellContainer<Spell>& filtered)
{
    if (filtered.size() == 0) {
        std::cout << "\n[Нет заклинаний, удовлетворяющих критериям]\n";
        return;
    }

    std::cout << "\n=== Найдено заклинаний: " << filtered.size() << " ===\n";
    for (size_t i = 0; i < filtered.size(); ++i) {
        const Spell* spell = filtered.at(i);
        if (spell) {
            spell->printConsole(std::cout);
        }
    }
    std::cout << "=== Конец списка ===\n";
}

//================================================================================

// Печать доступных книг
export void printBooks(Sorcerer& sorcererr)
{
    std::cout << "Доступные книги:\n";
    for (size_t i = 0; i < sorcererr.getBookCount(); ++i) {
        std::cout << i << ". " << sorcererr.getBookName(i)
            << " (" << sorcererr.getBook(i).size() << " заклинаний)\n";
    }
}

// Печать заклинаний конкретной книги
export void printBookSpells(Sorcerer& sorcererr, int& b_idx)
{
    std::cout << "\n--- " << sorcererr.getBookName(b_idx) << " ---\n";
    for (size_t i = 0; i < sorcererr.getBook(b_idx).size(); ++i) {
        const Spell* s = sorcererr.getBook(b_idx).at(i);
        if (s) s->printConsole(std::cout);
    }
}

// Валидация индексов книг и заклинаний
export bool getIndexes(Sorcerer& sorcererr, int& b_idx)
{
    if (sorcererr.getBookCount() == 0) { std::cout << "Нет книг.\n"; return false; }
    printBooks(sorcererr);
    b_idx = readIntInRange("Индекс книги: ", 0, sorcererr.getBookCount() - 1);
    return true;
}

export bool getIndexes(Sorcerer& sorcererr, int& b_idx, int& s_idx)
{
    if (!getIndexes(sorcererr, b_idx)) return false;
    if (sorcererr.getBook(b_idx).size() == 0) { std::cout << "Книга пуста.\n"; return false; }
    printBookSpells(sorcererr, b_idx);
    s_idx = readIntInRange("Индекс заклинания: ", 0, sorcererr.getBook(b_idx).size() - 1);
    return true;
}