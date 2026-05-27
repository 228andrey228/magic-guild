module;
import Spell;
import Sorcerer;
#include <iostream>
#include <string>
#include <map>
#include <array>
export module Guild;

// Гильдия магов — контейнер чародеев с уникальными именами
export class Guild
{
private:
    std::string name_;                          // название гильдии
    std::map<std::string, Sorcerer> members_;   // ключ = имя чародея

public:
    explicit Guild(const std::string& name) : name_(name) {}

    const std::string& getName() const { return name_; }
    size_t size()                const { return members_.size(); }

    // Геттер к участникам
    const std::map<std::string, Sorcerer>& getMembers() const { return members_; }
    std::map<std::string, Sorcerer>& getMembers() { return members_; }

    // Добавить чародея. Возвращает false если имя уже занято
    bool addMember(const std::string& name)
    {
        // map::emplace возвращает пару {iterator, bool}
        // bool == false означает, что ключ уже существует
        auto [it, inserted] = members_.emplace(name, Sorcerer(name));
        if (!inserted)
            std::cout << "Чародей '" << name << "' уже состоит в гильдии.\n";
        return inserted;
    }

    // Удалить чародея по имени
    bool removeMember(const std::string& name)
    {
        // map::erase возвращает количество удалённых элементов (0 или 1)
        return members_.erase(name) > 0;
    }

    // Переименовать чародея
    bool renameMember(const std::string& old_name, const std::string& new_name)
    {
        auto it = members_.find(old_name);
        if (it == members_.end()) {
            std::cout << "Чародей '" << old_name << "' не найден.\n";
            return false;
        }
        if (members_.count(new_name)) {
            std::cout << "Имя '" << new_name << "' уже занято.\n";
            return false;
        }

        // Извлекаем чародея, меняем имя, вставляем с новым ключом
        Sorcerer sr = std::move(it->second);
        sr.setName(new_name);
        members_.erase(it);
        members_.emplace(new_name, std::move(sr));
        return true;
    }

    // Получить указатель на чародея (nullptr если не найден)
    Sorcerer* getMember(const std::string& name)
    {
        auto it = members_.find(name);
        return (it != members_.end()) ? &it->second : nullptr;
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
        for (const auto& [name, sr] : members_) {
            std::cout << "  " << i++ << ". " << name
                << " | Мана: " << sr.getMana() << "/" << sr.getMaxMana()
                << " | Книг: " << sr.getBookCount() << "\n";
        }
        std::cout << "================================\n";
    }
};