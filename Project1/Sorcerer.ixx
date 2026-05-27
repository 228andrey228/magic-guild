module;
import Spell;
import Book;
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
export module Sorcerer;


export class Sorcerer
{
private:
    int mana_;
    int max_mana_;
    std::chrono::minutes current_time_;

    std::vector<SpellContainer<Spell>> books_;
    std::map<size_t, int> book_cast_counts_;
    std::vector<Book> library_;

    std::array<int, 4> elem_cast_counts_ = { 0 };

public:
    Sorcerer();

    // Управление книгами
    bool addBook(const std::string& name);
    bool removeBook(size_t index);

    // Управление заклинаниями
    bool addSpell(size_t book_idx, const Spell& spell);
    bool removeSpell(size_t book_idx, size_t spell_idx);
    bool modifySpell(size_t book_idx, size_t spell_idx, const Spell& new_spell);
    SpellContainer<Spell>& getBook(size_t index);
    size_t getBookCount() const;
    std::string getBookName(size_t index) const;

    // Магия и отдых
    bool castSpell(size_t book_idx, size_t spell_idx);
    void rest(int hours);

    // Статистика
    void printStats() const;

    // Сохранение/Загрузка
    bool saveState(const std::string& filename) const;
    bool loadState(const std::string& filename);

    // Геттеры
    int getMana() const { return mana_; }
    int getMaxMana() const { return max_mana_; }
    std::chrono::minutes getTime() const { return current_time_; }
};

// Конструктор с начальными параметрами
Sorcerer::Sorcerer() : mana_(100), max_mana_(100), current_time_(0) {}

// Добавление новой книги в коллекцию
bool Sorcerer::addBook(const std::string& name)
{
    for (const auto& b : library_) if (b.getName() == name) {
        std::cout << "Книга с таким именем уже есть!\n";
        return false;
    }
    library_.emplace_back(name);
    return true;
}

// Удаление книги по индексу
bool Sorcerer::removeBook(size_t index)
{
    if (index >= library_.size()) return false;
    library_.erase(library_.begin() + index);
    return true;
}

// Добавление заклинания в конкретную книгу
bool Sorcerer::addSpell(size_t book_idx, const Spell& spell)
{
    if (book_idx >= library_.size()) return false;
    library_[book_idx].addSpell(spell);
    return true;
}

// Удаление заклинания из книги по индексу
bool Sorcerer::removeSpell(size_t book_idx, size_t spell_idx)
{
    if (book_idx >= library_.size()) return false;
    return library_[book_idx].removeSpell(spell_idx);
}

// Изменение заклинания в книге
bool Sorcerer::modifySpell(size_t book_idx, size_t spell_idx, const Spell& new_spell)
{
    if (book_idx >= library_.size()) return false;
    return library_[book_idx].modifySpell(spell_idx, new_spell);
}

// Получение контейнера книги для чтения/фильтрации
SpellContainer<Spell>& Sorcerer::getBook(size_t index)
{
    return library_.at(index).getSpells();
}

size_t Sorcerer::getBookCount() const
{
    return library_.size();
}

std::string Sorcerer::getBookName(size_t index) const
{
    return library_.at(index).getName();
}

// Применение заклинания с проверкой маны и времени
bool Sorcerer::castSpell(size_t book_idx, size_t spell_idx)
{
    if (book_idx >= library_.size()) return false;
    const Spell* spell = library_[book_idx].getSpellAt(spell_idx);
    if (!spell) return false;

    if (mana_ < spell->getMana()) {
        std::cout << "Недостаточно маны! Требуется: " << spell->getMana() << ", есть: " << mana_ << '\n';
        return false;
    }
    if (!spell->isAvailableAtTime(current_time_)) {
        std::cout << "Заклинание недоступно в текущее время суток ("
            << Spell::minutesToString(current_time_) << ")\n";
        return false;
    }

    mana_ -= spell->getMana();
    current_time_ += std::chrono::minutes(15); // Каст занимает время
    ++library_[book_idx];
    elem_cast_counts_[static_cast<int>(spell->getElement()) - 1]++;
    std::cout << "Заклинание '" << spell->getName() << "' успешно применено!\n";
    return true;
}

export int manaMin(int a, int b)
{
    if (a <= b)
        return a;
    else
        return b;
}

// Отдых для восстановления маны
void Sorcerer::rest(int hours)
{
    if (hours <= 0) return;
    current_time_ += std::chrono::hours(hours);
    int restored = hours * 10; // 10 маны за час
    mana_ = manaMin(mana_ + restored, max_mana_);
    std::cout << "Вы отдохнули " << hours << " ч. Время: "
        << Spell::minutesToString(current_time_)
        << ". Мана: " << mana_ << "/" << max_mana_ << '\n';
}

// Вывод статистики использования
void Sorcerer::printStats() const
{
    std::cout << "\n=== Статистика чародея ===\n";
    std::cout << "Мана: " << mana_ << "/" << max_mana_ << '\n';
    std::cout << "Текущее время: " << Spell::minutesToString(current_time_) << "\n\n";

    std::cout << "[По книгам]\n";
    if (library_.empty()) std::cout << "  Заклинания не применялись\n";
    else {
        for (size_t i = 0; i < library_.size(); ++i) {
            std::cout << "  " << library_[i].getName() << ": " << library_[i].getCastCount() << " раз\n";
        }
    }

    std::cout << "\n[По стихиям]\n";
    bool used = false;
    for (int i = 0; i < 4; ++i) {
        if (elem_cast_counts_[i] > 0) {
            std::cout << "  " << ELEMENT_NAMES[i] << ": " << elem_cast_counts_[i] << " раз\n";
            used = true;
        }
    }
    if (!used) std::cout << "  Стихии не использовались\n";
    std::cout << "==========================\n";
}

// Сохранение состояния на диск
bool Sorcerer::saveState(const std::string& filename) const
{
    std::ofstream out(filename);
    if (!out.is_open()) return false;

    // Заголовок состояния
    out << mana_ << ' ' << max_mana_ << ' ' << current_time_.count() << '\n';
    out << library_.size() << '\n';

    // Сохранение книг и заклинаний
    for (const auto& book : library_) {
        out << book.getName() << '\n';
        out << book.size() << '\n';
        book.getSpells().print(out); // Вывод заклинаний в формате файла
        out << book.getCastCount() << '\n';
    }

    // Сохранение статистики стихий
    for (int count : elem_cast_counts_) out << count << ' ';
    out << '\n';

    return out.good();
}

// Загрузка состояния с диска
bool Sorcerer::loadState(const std::string& filename)
{
    std::ifstream in(filename);
    if (!in.is_open()) return false;

    int m, max_m, time_min;
    if (!(in >> m >> max_m >> time_min)) return false;
    mana_ = m;
    max_mana_ = max_m;
    current_time_ = std::chrono::minutes(time_min);

    size_t book_count;
    if (!(in >> book_count)) return false;

    library_.clear();
    for (size_t i = 0; i < book_count; ++i) {
        std::string name;
        size_t spell_count;
        if (!std::getline(in >> std::ws, name) || !(in >> spell_count)) return false;

        Book b(name);
        for (size_t j = 0; j < spell_count; ++j) {
            Spell s;
            if (!(in >> s)) return false;
            b.addSpell(s);
        }
        int cnt; in >> cnt;
        b.setCastCount(cnt);
        library_.push_back(std::move(b));
    }

    // Загрузка статистики стихий
    for (int i = 0; i < 4; ++i) {
        if (!(in >> elem_cast_counts_[i])) return false;
    }

    return in.eof() || in.good();
}