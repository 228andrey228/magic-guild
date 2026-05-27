import Spell;
import Book;
import Sorcerer;
import Tools;
#include <iostream>
#include <Windows.h>

// Имя файла для автосохранения
const std::string SAVE_FILE = "sorcerer_save.txt";


// Пространство Чародей
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
    MenuOption showMenu()
    {
        std::cout << "\n--- Меню Чародея ---\n";
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
            std::cout << "Завершение работы.\n";
            break;
        }
        }
    }
}


int main()
{
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    //std::cout << "AAA\n";               // проверка сборки на наличие мнимых ошибок от vs

    Sorcerer sorcerer;

    // Автоматическая загрузка состояния при старте
    if (sorcerer.loadState(SAVE_FILE)) {
        std::cout << "Прогресс восстановлен. Мана: " << sorcerer.getMana()
            << ", Время: " << Spell::minutesToString(sorcerer.getTime()) << '\n';
    }
    else {
        std::cout << "Сохранение не найдено. Начинаем новую игру.\n";
        sorcerer.addBook("Гримуар новичка"); // Стартовая книга
    }

    sorcerer::MenuOption currentOption{};
    do {
        currentOption = sorcerer::showMenu();
        sorcerer::executeOption(sorcerer, currentOption);

    } while (currentOption != sorcerer::MenuOption::EXIT);

    // Автоматическое сохранение при выходе
    if (sorcerer.saveState(SAVE_FILE)) {
        std::cout << "Прогресс сохранён.\n";
    }
    else {
        std::cout << "Ошибка сохранения!\n";
    }

    return 0;
}