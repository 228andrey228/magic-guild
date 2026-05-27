module;
import Spell;
#include <string>
#include <iostream>
export module Book;


// Отдельный класс книги заклинаний со встроенным счётчиком
export class Book {
private:
    std::string name_;
    SpellContainer<Spell> spells_;
    int cast_count_ = 0;
public:
    explicit Book(const std::string& name) : name_(name) {}

    const std::string& getName() const { return name_; }
    SpellContainer<Spell>& getSpells() { return spells_; }
    const SpellContainer<Spell>& getSpells() const { return spells_; }
    int getCastCount() const { return cast_count_; }
    void setCastCount(int count) { cast_count_ = count; }

    Book& operator++() {
        ++cast_count_;
        return *this;
    }
    Book operator++(int) {
        Book temp = *this;
        ++(*this);
        return temp;
    }
    size_t size() const { return spells_.size(); }

    // Методы для удобства работы извне
    void addSpell(const Spell& s) { spells_.add(s); }
    bool removeSpell(size_t idx) { return spells_.removeByIndex(idx); }
    bool modifySpell(size_t idx, const Spell& new_s) { return spells_.changeElement(idx, new_s); }
    const Spell* getSpellAt(size_t idx) const { return spells_.at(idx); }
};