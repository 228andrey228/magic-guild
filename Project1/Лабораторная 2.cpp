// Шеин Андрей 2 курс 91 гр

// Лаба 2 (4) - Заклинание
//«Заклинание» представляет собой структуру с полями : название, краткое описание,
//стихия(перечисление), затрачиваемая мана, сила и продолжительность эффекта,
//минимальный уровень чародея для использования, время суток «с» и «по» которое
//оно может быть применено(использовать одну из предлагаемых языком структур).
//Выборка заклинаний производится путем указания пользователем подходящего
//интервала по одному из критериев : название(например, от «Mutilatio»
//до «Somnium» включительно) и расход маны.Также выборка всех заклинаний,
//доступных в указанное время, выборка всех заклинаний, относящихся к указанной
//стихии, выборка всех заклинаний, доступных на указанном уровне.Шаблонный
//класс должен иметь дополнительный метод сортировки.Добавить в меню
//возможность отсортировать заклинания по минимальному уровню по возрастанию
//и по названию по алфавиту.

// Лаба 3 (4) - Чародей
//Реализовать класс «Чародей», который имеет свой запас маны, а также хранит
//несколько магических книг, в которых записаны заклинания.Чародей должен иметь
//возможность просматривать любую имеющуюся книгу, выбирать из нее
//подходящие по фильтру заклинания(по условию лабы 2), удалять их, править
//и добавлять новые.Также чародей может завести новую книгу или избавиться
//от старой.Кроме этого, чародей может колдовать, если ему хватает маны на данное
//заклинание и время суток подходящее, а также восстанавливать запас маны
//отдохнув(при этом текущее время увеличивается на N часов).
//Еще чародей может припомнить сколько раз он использовал заклинания из каждой
//отдельной книги и сколько раз он применял заклинания каждой отдельной стихии.

import Spell__Lab2_;
import Sorcerer__Lab3__;
import Tools;
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

// Имя файла для автосохранения
const std::string SAVE_FILE = "sorcerer_save.txt";


// Пространство для Лабы 2 (4) - Заклинание - СДАНА
namespace spell {

    // Перечисление пунктов меню
    enum class MenuOption : int
    {
        LOAD_FROM_FILE = 1,
        ADD_FROM_CONSOLE,
        ADD_FROM_FILE,
        REMOVE_BY_INDEX,
        REMOVE_BY_VALUE,
        CHANGE_BY_INDEX,
        FILTER_BY_NAME_RANGE,
        FILTER_BY_MANA_RANGE,
        FILTER_BY_ELEMENT,
        FILTER_BY_LEVEL,
        FILTER_BY_TIME,
        SORT_BY_NAME,
        SORT_BY_LEVEL,
        PRINT_TO_SCREEN,
        PRINT_TO_FILE,
        EXIT
    };

    // Обработка выбора пользователя в главном меню
    void executeOption(SpellContainer<Spell>& container, MenuOption option)
    {
        switch (option)
        {
        case MenuOption::LOAD_FROM_FILE:
        {
            auto filename = readFilename();
            if (container.loadFromFile(filename)) {
                std::cout << "Загрузка успешна. Элементов: " << container.size() << '\n';
            }
            else {
                std::cout << "Ошибка: не удалось загрузить файл '" << filename << "'\n";
                std::cout << "Проверьте существование файла и его формат.\n";
            }
            break;
        }

        case MenuOption::ADD_FROM_CONSOLE:
        {
            container.add(readSpellFromConsole());
            std::cout << "Заклинание добавлено.\n";
            break;
        }

        case MenuOption::ADD_FROM_FILE:
        {
            auto filename = readFilename();
            std::ifstream file(filename);
            Spell temp;
            if (file >> temp) {
                container.add(temp);
                std::cout << "Заклинание из файла добавлено.\n";
            }
            else {
                std::cout << "Ошибка чтения файла.\n";
            }
            break;
        }

        case MenuOption::REMOVE_BY_INDEX:
        {
            if (container.size() == 0) {
                std::cout << "Контейнер пуст.\n";
                break;
            }
            int idx = readIntInRange("Индекс для удаления", 0, static_cast<int>(container.size()) - 1);
            if (container.removeByIndex(static_cast<size_t>(idx))) {
                std::cout << "Удалено.\n";
            }
            else {
                std::cout << "Ошибка удаления.\n";
            }
            break;
        }

        case MenuOption::REMOVE_BY_VALUE:
        {
            if (container.size() == 0) {
                std::cout << "Контейнер пуст.\n";
                break;
            }
            std::cout << "Введите данные заклинания для удаления (по имени):\n";
            Spell target;
            target.setName(readNonEmptyString("Название: "));
            if (container.removeByValue(target)) {
                std::cout << "Удалено.\n";
            }
            else {
                std::cout << "Заклинание не найдено.\n";
            }
            break;
        }

        case MenuOption::CHANGE_BY_INDEX:
        {
            if (container.size() == 0) {
                std::cout << "Контейнер пуст.\n";
                break;
            }
            int idx = readIntInRange("Индекс для изменения", 0, static_cast<int>(container.size()) - 1);
            std::cout << "Введите новые данные заклинания:\n";
            Spell updated = readSpellFromConsole();
            if (container.changeElement(static_cast<size_t>(idx), updated)) {
                std::cout << "Изменено.\n";
            }
            else {
                std::cout << "Ошибка изменения.\n";
            }
            break;
        }

        case MenuOption::FILTER_BY_NAME_RANGE:
        {
            std::cout << "Диапазон названий (лексикографический):\n";
            auto from = readNonEmptyString("От: ");
            auto to = readNonEmptyString("До: ");
            auto result = container.select(filters::nameRange(from, to));
            displayFiltered(result);
            break;
        }

        case MenuOption::FILTER_BY_MANA_RANGE:
        {
            int min = readIntInRange("Мана от", 0, 10000);
            int max = readIntInRange("Мана до", min, 10000);
            auto result = container.select(filters::manaRange(min, max));
            displayFiltered(result);
            break;
        }

        case MenuOption::FILTER_BY_ELEMENT:
        {
            Element elem = readElement();
            auto result = container.select(filters::byElement(elem));
            displayFiltered(result);
            break;
        }

        case MenuOption::FILTER_BY_LEVEL:
        {
            int level = readIntInRange("Уровень чародея", 1, 100);
            auto result = container.select(filters::availableAtLevel(level));
            displayFiltered(result);
            break;
        }

        case MenuOption::FILTER_BY_TIME:
        {
            auto time = readTime();
            auto result = container.select(filters::atTime(time));
            displayFiltered(result);
            break;
        }

        case MenuOption::SORT_BY_NAME:
        {
            if (container.size() > 0) {
                container.sort(filters::byName());
                std::cout << "Сортировка по названию выполнена.\n";
            }
            else {
                std::cout << "Контейнер пуст.\n";
            }
            break;
        }

        case MenuOption::SORT_BY_LEVEL:
        {
            if (container.size() > 0) {
                container.sort(filters::byMinLevel());
                std::cout << "Сортировка по уровню выполнена.\n";
            }
            else {
                std::cout << "Контейнер пуст.\n";
            }
            break;
        }

        case MenuOption::PRINT_TO_SCREEN:
        {
            if (container.size() > 0) {
                std::cout << "\n=== Все заклинания ===\n";
                for (size_t i = 0; i < container.size(); ++i) {
                    const Spell* spell = container.at(i);
                    if (spell) {
                        spell->printConsole(std::cout);
                    }
                }
                std::cout << "=== Конец ===\n";
            }
            else {
                std::cout << "Контейнер пуст.\n";
            }
            break;
        }

        case MenuOption::PRINT_TO_FILE:
        {
            auto filename = readFilename();
            std::ofstream file(filename);
            if (container.size() > 0) {
                container.print(file);
                std::cout << "Данные сохранены в файл.\n";
            }
            else {
                std::cout << "Нечего сохранять: контейнер пуст.\n";
            }
            break;
        }

        case MenuOption::EXIT:
        {
            std::cout << "Завершение работы.\n";
            break;
        }
        }
    }

    // Отображение главного меню
    MenuOption showMenu()
    {
        std::cout << "\n--- Меню управления заклинаниями ---\n";
        std::cout << "1.  Загрузить из файла\n";
        std::cout << "2.  Добавить (консоль)\n";
        std::cout << "3.  Добавить (файл)\n";
        std::cout << "4.  Удалить (по индексу)\n";
        std::cout << "5.  Удалить (по значению)\n";
        std::cout << "6.  Изменить (по индексу)\n";
        std::cout << "7.  Выборка (диапазон названий)\n";
        std::cout << "8.  Выборка (диапазон маны)\n";
        std::cout << "9.  Выборка (по стихии)\n";
        std::cout << "10. Выборка (по уровню)\n";
        std::cout << "11. Выборка (по времени)\n";
        std::cout << "12. Сортировка (по названию)\n";
        std::cout << "13. Сортировка (по уровню)\n";
        std::cout << "14. Вывести на экран\n";
        std::cout << "15. Сохранить в файл\n";
        std::cout << "16. Выход\n";

        while (true) {
            int choice = readInt("Выберите пункт: ");
            if (choice >= static_cast<int>(MenuOption::LOAD_FROM_FILE) &&
                choice <= static_cast<int>(MenuOption::EXIT)) {
                return static_cast<MenuOption>(choice);
            }
            std::cout << "Ошибка: выберите пункт от 1 до 16.\n";
        }
    }
}


// =================================================================================================


// Пространство для Лабы 3 (4) - Чародей
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
            if (!getIndexes(sorcererr, idx)) break;
            printBookSpells(sorcererr, idx);
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
            if (!getIndexes(sorcererr, idx)) break;
            if (sorcererr.removeBook(idx)) std::cout << "Книга удалена.\n";
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

            std::cout << "Критерий выборки:\n1. Название\n2. Мана\n3. Стихия\n4. Уровень\n5. Время\n";
            int f_choice = readIntInRange("Выбор: ", 1, 5);

            switch (f_choice) {
            case 1: spell::executeOption(sorcererr.getBook(b_idx), spell::MenuOption::FILTER_BY_NAME_RANGE); break;
            case 2: spell::executeOption(sorcererr.getBook(b_idx), spell::MenuOption::FILTER_BY_MANA_RANGE); break;
            case 3: spell::executeOption(sorcererr.getBook(b_idx), spell::MenuOption::FILTER_BY_ELEMENT); break;
            case 4: spell::executeOption(sorcererr.getBook(b_idx), spell::MenuOption::FILTER_BY_LEVEL); break;
            case 5: spell::executeOption(sorcererr.getBook(b_idx), spell::MenuOption::FILTER_BY_TIME); break;
            }
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

// =================================================================================================


// Пространство для выбора задания
namespace task
{
    enum class MenuOption : int
    {
        EX_2_SPELL = 1,
        EX_3_SORCERER,
        EXIT
    };

    MenuOption showMenu()
    {
        std::cout << "\n!!! Выбор задания !!!\n";
        std::cout << "1.  Лаба 2 (4) - Заклинание - [СДАНА]\n";
        std::cout << "2.  Лаба 3 (4) - Чародей\n";
        std::cout << "3.  Выход\n";

        while (true) {
            int choice = readInt("Выберите пункт: ");
            if (choice >= static_cast<int>(MenuOption::EX_2_SPELL) &&
                choice <= static_cast<int>(MenuOption::EXIT)) {
                return static_cast<MenuOption>(choice);
            }
            std::cout << "Ошибка: выберите пункт от 1 до 3.\n";
        }
    }

    void executeTask(MenuOption option)
    {
        switch (option)
        {
        case MenuOption::EX_2_SPELL:
        {
            SpellContainer<Spell> container;
            spell::MenuOption currentOption{};

            do {
                currentOption = spell::showMenu();
                spell::executeOption(container, currentOption);
            } while (currentOption != spell::MenuOption::EXIT);
            break;
        }

        case MenuOption::EX_3_SORCERER:
        {
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

    task::MenuOption currentTask{};
    do {
        currentTask = task::showMenu();
        executeTask(currentTask);
    } while (currentTask != task::MenuOption::EXIT);

    return 0;
}