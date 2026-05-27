module;
import Spell;
import Sorcerer;
#include <iostream>
#include <string>
#include <set>
#include <array>
export module Guild;


// Прозрачный компаратор — позволяет искать в set по std::string
export struct ByName {
    using is_transparent = void;

    // Сравнение двух чародеев
    bool operator()(const Sorcerer& a, const Sorcerer& b) const {
        return a.getName() < b.getName();
    }
    // Поиск по строке слева: find("Имя")
    bool operator()(const std::string& s, const Sorcerer& b) const {
        return s < b.getName();
    }
    // Поиск по строке справа
    bool operator()(const Sorcerer& a, const std::string& s) const {
        return a.getName() < s;
    }
};


// Гильдия магов — контейнер чародеев с уникальными именами
export class Guild
{
private:
    std::string name_;                          // название гильдии
    std::set<Sorcerer, ByName> members_;    

public:
    Guild();
    explicit Guild(const std::string& name) : name_(name) {}

    const std::string& getName() const { return name_; }
    size_t size()                const { return members_.size(); }

    // Геттер к участникам
    const std::set<Sorcerer, ByName>& getMembers() const { return members_; }

    // Добавить чародея. Возвращает false если имя уже занято
    bool addMember(const std::string& name)
    {
        // map::emplace возвращает пару {iterator, bool}
        // bool == false означает, что ключ уже существует
        auto [it, inserted] = members_.emplace(name); // Sorcerer(name)
        if (!inserted)
            std::cout << "Чародей '" << name << "' уже состоит в гильдии.\n";
        return inserted;
    }

    // Удалить чародея по имени
    bool removeMember(const std::string& name)
    {
        auto it = members_.find(name); // find принимает string благодаря ByName::is_transparent
        if (it == members_.end()) return false;
        members_.erase(it);
        return true;
    }

    // Переименовать чародея
    bool renameMember(const std::string& old_name, const std::string& new_name)
    {
        auto it = members_.find(old_name);
        if (it == members_.end()) {
            std::cout << "Чародей '" << old_name << "' не найден.\n";
            return false;
        }
        if (members_.find(new_name) != members_.end()) {
            std::cout << "Имя '" << new_name << "' уже занято.\n";
            return false;
        }

        // extract вынимает узел из set без удаления объекта
        auto node = members_.extract(it);
        node.value().setName(new_name); // меняем имя внутри вынутого узла
        members_.insert(std::move(node)); // вставляем обратно — set переупорядочивается
        return true;
    }

    // Получить указатель на чародея (nullptr если не найден)
    Sorcerer* getMember(const std::string& name)
    {
        auto it = members_.find(name);
        // через этот указатель имя (ключ сортировки) не меняется
        return (it != members_.end()) ? const_cast<Sorcerer*>(&*it) : nullptr;
    }

    // Вывести список членов гильдии
    void printMembers() const
    {
        if (members_.empty()) {
            std::cout << "Гильдия пуста.\n";
            return;
        }
        std::cout << "\n=== Члены гильдии [" << name_ << "] ===\n";
        int i = 1;
        for (const auto& sr : members_) {
            std::cout << "  " << i++ << ". " << sr.getName()
                << " | Мана: " << sr.getMana() << "/" << sr.getMaxMana()
                << " | Книг: " << sr.getBookCount() << "\n";
        }
        std::cout << "================================\n";
    }

    // Сохранение и загрузка гильдии
    bool saveState(const std::string& filename) const;
    bool loadState(const std::string& filename);

    // Статистика по гильдии
    void printStats() const;
};

bool Guild::saveState(const std::string& filename) const
{
    std::ofstream out(filename);
    if (!out.is_open()) return false;

    // название гильдии
    out << name_ << '\n';
    // количество чародеев
    out << members_.size() << '\n';

    // Для каждого чародея
    for (const auto& sr : members_)
        sr.saveState(out);

    return out.good();
}

bool Guild::loadState(const std::string& filename)
{
    std::ifstream in(filename);
    if (!in.is_open()) return false;

    std::string gname;
    if (!std::getline(in >> std::ws, gname)) return false;
    name_ = gname;

    size_t member_count;
    if (!(in >> member_count)) return false;

    members_.clear();
    for (size_t i = 0; i < member_count; ++i) {
        Sorcerer sr;
        if (!sr.loadState(in)) return false;
        members_.insert(std::move(sr));
    }

    return in.eof() || in.good();
}

void Guild::printStats() const
{
    std::cout << "\n=== Статистика гильдии [" << name_ << "] ===\n";
    std::cout << "Членов: " << members_.size() << "\n\n";

    // Суммарные счётчики по всем чародеям
    int total_casts = 0;                // всего применений заклинаний
    std::array<int, 4> total_elem{ 0 };    // по стихиям

    // Итерируемся по всем членам гильдии
    for (const auto& sr : members_) {
        std::cout << "   --- " << sr.getName() << " (мана: " << sr.getMana() << "/" << sr.getMaxMana() << ")\n";

        // Считаем применения по книгам этого чародея
        int member_casts = 0;
        for (size_t i = 0; i < sr.getBookCount(); ++i) 
        {
            const Book& book = sr.getLib().at(i);
            int cnt = book.getCastCount();
            if (cnt > 0) member_casts += cnt;
        }
        std::cout << "   Использовано заклинаний: " << member_casts << "\n";
        total_casts += member_casts;

        // Накапливаем статистику стихий
        const auto& ec = sr.getElemCounts();
        for (int i = 0; i < 4; ++i)
            total_elem[i] += ec[i];
    }

    // Итоговая сводка
    std::cout << "\n[Итого по гильдии]\n";
    if (total_casts == 0) std::cout << "  Заклинания не применялись.\n";
    else
    {
        std::cout << "Всего заклинаний применено: " << total_casts << "\n";
        std::cout << "[По стихиям]\n";

        bool any = false;
        for (int i = 0; i < 4; ++i) {
            if (total_elem[i] > 0) {
                std::cout << "  " << ELEMENT_NAMES[i] << ": " << total_elem[i] << " раз\n";
                any = true;
            }
        }

        std::cout << "==============================\n";
    }
}