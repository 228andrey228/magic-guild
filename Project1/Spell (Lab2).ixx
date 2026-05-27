module;
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
export module Spell__Lab2_;

// Перечисление стихий заклинаний
export enum class Element : int
{
    WATER = 1,
    LAND = 2,
    AIR = 3,
    FIRE = 4
};

// ПРАВКА ВЫПОЛНЕНА
export std::array<std::string, 4> ELEMENT_NAMES = { "Вода", "Земля", "Воздух", "Огонь" };
// ---

// Структура заклинания
export struct Spell
{
private:
    std::string name_;
    std::string description_;
    Element elem_;
    int mana_;
    int power_;
    int duration_minutes_;
    int min_level_;
    std::chrono::minutes start_time_;
    std::chrono::minutes end_time_;

public:
    Spell() : mana_(0), power_(0), duration_minutes_(0), min_level_(1) {}

    // Геттеры для доступа к приватным полям
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    Element getElement() const { return elem_; }
    int getMana() const { return mana_; }
    int getPower() const { return power_; }
    int getDurationMinutes() const { return duration_minutes_; }
    int getMinLevel() const { return min_level_; }
    std::chrono::minutes getStartTime() const { return start_time_; }
    std::chrono::minutes getEndTime() const { return end_time_; }

    // Сеттеры для модификации полей
    void setName(const std::string& name) { name_ = name; }
    void setDescription(const std::string& desc) { description_ = desc; }
    void setElement(Element elem) { elem_ = elem; }
    void setMana(int mana) { mana_ = mana; }
    void setPower(int power) { power_ = power; }
    void setDurationMinutes(int duration) { duration_minutes_ = duration; }
    void setMinLevel(int level) { min_level_ = level; }
    void setTimeRange(std::chrono::minutes start, std::chrono::minutes end) {
        start_time_ = start;
        end_time_ = end;
    }

    // Проверка доступности заклинания в указанное время суток
    bool isAvailableAtTime(std::chrono::minutes time) const;

    // Преобразование количества минут в строку формата "ЧЧ:ММ"
    static std::string minutesToString(std::chrono::minutes time);

    // Оператор сравнения для удаления по значению
    bool operator==(const Spell& other) const { return name_ == other.name_; }

    // Вывод в читаемом формате (для консоли)
    void printConsole(std::ostream& os) const;
};

// Реализация преобразования стихии в строку
export std::string elementToString(Element elem) {
    int idx = static_cast<int>(elem) - 1;
    return (idx >= 0 && idx < 4) ? ELEMENT_NAMES[idx] : "Неизвестно";
}

// Оператор вывода в поток для структуры Spell (для файла)
export std::ostream& operator<<(std::ostream& os, const Spell& spell)
{
    os << spell.getName() << '\n'
        << spell.getDescription() << '\n'
        << static_cast<int>(spell.getElement()) << ' '
        << spell.getMana() << ' '
        << spell.getPower() << ' '
        << spell.getMinLevel() << ' '
        << spell.getDurationMinutes() << '\n'
        << Spell::minutesToString(spell.getStartTime()) << ' '
        << Spell::minutesToString(spell.getEndTime()) << '\n';
    return os;
}

// Вывод в читаемом формате (для консоли)
void Spell::printConsole(std::ostream& os) const
{
    os << "=== Заклинание ===\n"
        << "Название: " << getName() << '\n'
        << "Описание: " << getDescription() << '\n'
        << "Стихия: " << elementToString(elem_) << '\n'
        << "Мана: " << getMana() << '\n'
        << "Сила: " << getPower() << '\n'
        << "Мин. уровень: " << getMinLevel() << '\n'
        << "Длительность (мин): " << getDurationMinutes() << '\n'
        << "Время применения: " << minutesToString(getStartTime())
        << " - " << minutesToString(getEndTime()) << "\n\n";
}

// Валидация часов и минут для времени суток
export bool isValidTime(int hours, int minutes)
{
    return (hours >= 0 && hours < 24) && (minutes >= 0 && minutes < 60);
}

// Парсинг времени из потока в формате "ЧЧ:ММ"
export bool parseTime(std::istream& is, std::chrono::minutes& out_time)
{
    int h, m;
    char colon;
    if (is >> h >> colon >> m && colon == ':' && isValidTime(h, m)) {
        out_time = std::chrono::hours(h) + std::chrono::minutes(m);
        return true;
    }
    is.setstate(std::ios::failbit);
    return false;
}

// Оператор ввода из потока для структуры Spell
export std::istream& operator>>(std::istream& in, Spell& spell)
{
    std::string name, desc;
    if (!std::getline(in >> std::ws, name) || name.empty()) {
        in.setstate(std::ios::failbit);
        return in;
    }
    if (!std::getline(in, desc)) {
        in.setstate(std::ios::failbit);
        return in;
    }

    int elem_raw, mana, power, min_level, duration_min;
    if (!(in >> elem_raw >> mana >> power >> min_level >> duration_min)) { return in; }

    if (elem_raw < 1 || elem_raw > 4 || mana < 0 || min_level < 1 || duration_min < 0) {
        in.setstate(std::ios::failbit);
        return in;
    }

    std::chrono::minutes start, end;
    if (!parseTime(in, start) || !parseTime(in, end)) { return in; }

    spell.setName(name);
    spell.setDescription(desc);
    spell.setElement(static_cast<Element>(elem_raw));
    spell.setMana(mana);
    spell.setPower(power);
    spell.setMinLevel(min_level);
    spell.setDurationMinutes(duration_min);
    spell.setTimeRange(start, end);

    return in;
}

// Реализация метода проверки доступности заклинания по времени
bool Spell::isAvailableAtTime(std::chrono::minutes time) const
{
    if (start_time_ <= end_time_) {
        return time >= start_time_ && time <= end_time_;
    }
    return time >= start_time_ || time <= end_time_;
}

// Реализация преобразования минут в строку "ЧЧ:ММ"
std::string Spell::minutesToString(std::chrono::minutes time)
{
    auto total = time.count();
    auto h = total / 60;
    auto m = total % 60;
    return std::to_string(h) + ":" + (m < 10 ? "0" : "") + std::to_string(m);
}


//============================================================================================


// Шаблонный класс-контейнер для управления коллекцией объектов
// В качестве хранилища используется std::vector для доступа по индексу и эффективной сортировки
export template<typename T>
class SpellContainer
{
private:
    std::vector<T> items_;

public:
    // Возвращает текущее количество элементов в контейнере
    size_t size() const { return items_.size(); }

    // Добавляет новый элемент в конец контейнера
    void add(const T& item);

    // Удаляет элемент по его индексу
    bool removeByIndex(size_t index);

    // Удаляет первый элемент, равный заданному значению
    bool removeByValue(const T& item);

    // Заменяет элемент по индексу на новый
    bool changeElement(size_t index, const T& new_item);

    // Выводит все элементы контейнера в указанный поток
    void print(std::ostream& os) const;

    // Загружает элементы из файла, заменяя текущее содержимое контейнера
    bool loadFromFile(const std::string& filename);

    // Сортирует элементы контейнера по заданному компаратору
    void sort(std::function<bool(const T&, const T&)> comparator);

    // Возвращает новый контейнер с элементами, удовлетворяющими предикату
    template<typename Predicate>
    SpellContainer<T> select(Predicate pred) const;

    // Безопасный доступ к элементу по индексу
    const T* at(size_t index) const;
};

// Реализация метода добавления элемента
template<typename T>
void SpellContainer<T>::add(const T& item)
{
    items_.push_back(item);
}

// Реализация удаления по индексу
template<typename T>
bool SpellContainer<T>::removeByIndex(size_t index)
{
    if (index < items_.size()) {
        items_.erase(items_.begin() + static_cast<std::ptrdiff_t>(index));
        return true;
    }
    return false;
}

// Реализация удаления по значению
template<typename T>
bool SpellContainer<T>::removeByValue(const T& item)
{
    auto it = std::find(items_.begin(), items_.end(), item);
    if (it != items_.end()) {
        items_.erase(it);
        return true;
    }
    return false;
}

// Реализация изменения элемента по индексу
template<typename T>
bool SpellContainer<T>::changeElement(size_t index, const T& new_item)
{
    if (index < items_.size()) {
        items_[index] = new_item;
        return true;
    }
    return false;
}

// Реализация вывода в поток
template<typename T>
void SpellContainer<T>::print(std::ostream& os) const
{
    std::copy(
        items_.cbegin(),
        items_.cend(),
        std::ostream_iterator<T>(os, "\n")
    );
}

// Реализация загрузки из файла
template<typename T>
bool SpellContainer<T>::loadFromFile(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    // Сначала читаем во временный контейнер, чтобы не потерять данные при ошибке парсинга
    std::vector<T> temp;
    std::copy(
        std::istream_iterator<T>(file),
        std::istream_iterator<T>(),
        std::back_inserter(temp)
    );

    if (temp.empty()) {
        return false;
    }

    items_ = std::move(temp);
    return true;
}

// Реализация сортировки
template<typename T>
void SpellContainer<T>::sort(std::function<bool(const T&, const T&)> comparator)
{
    std::sort(items_.begin(), items_.end(), comparator);
}

// Реализация фильтрации
template<typename T>
template<typename Predicate>
SpellContainer<T> SpellContainer<T>::select(Predicate pred) const
{
    SpellContainer<T> result;
    std::copy_if(
        items_.cbegin(),
        items_.cend(),
        std::back_inserter(result.items_),
        pred
    );
    return result;
}

// Реализация безопасного доступа к элементу
template<typename T>
const T* SpellContainer<T>::at(size_t index) const
{
    if (index < items_.size()) {
        return &items_[index];
    }
    return nullptr;
}