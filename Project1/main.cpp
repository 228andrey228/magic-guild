import Spell;
import Book;
import Sorcerer;
import Guild;
import Tools;
#include <iostream>
#include <Windows.h>

// Имя файла для автосохранения
const std::string SAVE_FILE = "sorcerer_save.txt";


// ============================ Меню чародея ====================================


namespace sorcerer
{
    // Перечисление пунктов меню
    enum class MenuOption : int
    {
        VIEW_BOOK = 1,
        ADD_BOOK,
        REMOVE_BOOK,
        ADD_SPELL,
        REMOVE_SPELL,
        MODIFY_SPELL,
        FILTER_SPELLS,
        CAST_SPELL,
        REST,
        VIEW_STATS,
        SAVE_MANUALLY,
        EXIT
    };

    // Отображение главного меню
    MenuOption showMenu(Sorcerer& s)
    {
        std::cout << "\n--- Меню Чародея [" << s.getName() << "] ---\n";
        std::cout << "1.  Просмотреть книгу\n";
        std::cout << "2.  Добавить книгу\n";
        std::cout << "3.  Удалить книгу\n";
        std::cout << "4.  Добавить заклинание в книгу\n";
        std::cout << "5.  Удалить заклинание из книги\n";
        std::cout << "6.  Изменить заклинание в книге\n";
        std::cout << "7.  Фильтр заклинаний в книге\n";
        std::cout << "8.  Применить заклинание (колдовать)\n";
        std::cout << "9.  Отдохнуть (восстановить ману)\n";
        std::cout << "10. Статистика использования\n";
        std::cout << "11. Сохранить прогресс вручную\n";
        std::cout << "12. Выход (авто-сохранение)\n";

        while (true) {
            int choice = readInt("Выберите пункт: ");
            if (choice >= 1 && choice <= 12) {
                return static_cast<MenuOption>(choice);
            }
            std::cout << "Ошибка: выберите пункт от 1 до 12.\n";
        }
    }

    // Обработка выбора пользователя в главном меню
    void executeOption(Sorcerer& sorcererr, MenuOption option)
    {
        switch (option)
        {
        case MenuOption::VIEW_BOOK: // Просмотр книги
        {
            int idx{};
            if (getIndexes(sorcererr, idx)) printBookSpells(sorcererr, idx);
            break;
        }

        case MenuOption::ADD_BOOK: // Добавить книгу
        {
            std::string name = readNonEmptyString("Название новой книги: ");
            if (sorcererr.addBook(name))
                std::cout << "Книга '" << name << "' добавлена.\n";
            break;
        }

        case MenuOption::REMOVE_BOOK: // Удалить книгу
        {
            int idx{};
            if (getIndexes(sorcererr, idx) && sorcererr.removeBook(idx))
                std::cout << "Книга удалена.\n";
            break;
        }

        case MenuOption::ADD_SPELL: // Добавить заклинание
        {
            int b_idx{};
            if (!getIndexes(sorcererr, b_idx)) break;
            Spell s = readSpellFromConsole();
            if (sorcererr.addSpell(b_idx, s)) std::cout << "Заклинание добавлено.\n";
            break;
        }

        case MenuOption::REMOVE_SPELL: // Удалить заклинание
        {
            int b_idx{}, s_idx{};
            if (!getIndexes(sorcererr, b_idx, s_idx)) break;
            if (sorcererr.removeSpell(b_idx, s_idx)) std::cout << "Удалено.\n";
            break;
        }

        case MenuOption::MODIFY_SPELL: // Изменить заклинание
        {
            int b_idx{}, s_idx{};
            if (!getIndexes(sorcererr, b_idx, s_idx)) break;
            Spell new_s = readSpellFromConsole();
            if (sorcererr.modifySpell(b_idx, s_idx, new_s)) std::cout << "Изменено.\n";
            break;
        }

        case MenuOption::FILTER_SPELLS: // Фильтр
        {
            int b_idx{};
            if (!getIndexes(sorcererr, b_idx)) break;
            if (sorcererr.getBook(b_idx).size() == 0) { std::cout << "Книга пуста.\n"; break; }

            filterBook(sorcererr.getBook(b_idx));
            break;
        }

        case MenuOption::CAST_SPELL: // Колдовать
        {
            int b_idx{}, s_idx{};
            if (!getIndexes(sorcererr, b_idx, s_idx)) break;
            sorcererr.castSpell(b_idx, s_idx);
            break;
        }

        case MenuOption::REST: // Отдохнуть
        {
            int hours = readIntInRange("Часов отдыха: ", 1, 24);
            sorcererr.rest(hours);
            break;
        }

        case MenuOption::VIEW_STATS: // Статистика
        {
            sorcererr.printStats();
            break;
        }

        case MenuOption::SAVE_MANUALLY: // Ручное сохранение
        {
            auto filename = readFilename();
            if (sorcererr.saveState(filename)) std::cout << "Сохранено.\n";
            else std::cout << "Ошибка сохранения.\n";
            break;
        }

        case MenuOption::EXIT:
        {
            std::cout << "Возврат к меню гильдии.\n";
            break;
        }
        }
    }
}


// ============================ Меню гильдии ====================================


namespace guild_ui
{
    enum class MenuOption : int {
        VIEW_MEMBERS = 1,   // посмотреть всех чародеев
        ADD_MEMBER,         // добавить чародея
        REMOVE_MEMBER,      // удалить чародея
        RENAME_MEMBER,      // переименовать чародея
        MANAGE_MEMBER,      // управлять конкретным чародеем (его меню)
        VIEW_STATS,         // статистика по гильдии
        EXIT                // выход из гильдии
    };

    MenuOption showMenu(const Guild& g)
    {
        std::cout << "\n=== Гильдия [" << g.getName() << "] | Членов: " << g.size() << " ===\n"
            << "1. Список чародеев\n"
            << "2. Добавить чародея\n"
            << "3. Удалить чародея\n"
            << "4. Переименовать чародея\n"
            << "5. Управлять чародеем\n"
            << "6. Статистика гильдии\n"
            << "7. Выход\n";

        while (true) {
            int choice = readInt("Выберите пункт: ");
            if (choice >= 1 && choice <= 7) {
                return static_cast<MenuOption>(choice);
            }
            std::cout << "Ошибка: выберите пункт от 1 до 7.\n";
        }
    }

    void executeOption(Guild& g, MenuOption opt)
    {
        switch (opt)
        {
        case MenuOption::VIEW_MEMBERS:
        {
            g.printMembers();
            break;
        }

        case MenuOption::ADD_MEMBER: 
        {
            std::string name = readNonEmptyString("Имя нового чародея: ");
            if (g.addMember(name)) {
                // Стартовая книга для нового чародея
                g.getMember(name)->addBook("Гримуар новичка");
                std::cout << "Чародей '" << name << "' принят в гильдию.\n";
            }
            break;
        }

        case MenuOption::REMOVE_MEMBER: 
        {
            Sorcerer* sr = selectMember(g);
            if (!sr) break;
            std::string name = sr->getName();
            if (g.removeMember(name))
                std::cout << "Чародей '" << name << "' исключён из гильдии.\n";
            break;
        }

        case MenuOption::RENAME_MEMBER: {
            Sorcerer* sr = selectMember(g);
            if (!sr) break;
            std::string old_name = sr->getName();
            std::string new_name = readNonEmptyString("Новое имя: ");
            if (g.renameMember(old_name, new_name))
                std::cout << "Переименован: " << old_name << " -> " << new_name << '\n';
            break;
        }

        case MenuOption::MANAGE_MEMBER: {
            // Выбрать чародея и войти в его личное меню
            Sorcerer* sr = selectMember(g);
            if (!sr) break;
            //std::cout << "\n[Управление: " << sr->getName() << "]\n";
            sorcerer::MenuOption sopt{};
            do {
                sopt = sorcerer::showMenu(*sr);
                sorcerer::executeOption(*sr, sopt);
            } while (sopt != sorcerer::MenuOption::EXIT);
            break;
        }

        case MenuOption::VIEW_STATS:
            // Пока заглушка — полная статистика будет на шаге 6
            break;

        case MenuOption::EXIT:
            std::cout << "Выход из гильдии.\n";
            break;
        }
    }
}


int main()
{
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    //std::cout << "AAA\n";               // проверка сборки на наличие мнимых ошибок от vs

    Guild guild("Орден Пепла");

    guild_ui::MenuOption opt{};
    do {
        opt = guild_ui::showMenu(guild);
        guild_ui::executeOption(guild, opt);
    } while (opt != guild_ui::MenuOption::EXIT);

    return 0;
}