#include <iostream>
#include <vector>
#include <string>

class WarhammerMiniature {
public:
    int pointsCost;
    std::string modelName;
    int releaseYear;

    WarhammerMiniature(int cost = 0, std::string name = "", int year = 0)
        : pointsCost(cost), modelName(name), releaseYear(year) {}

    bool operator==(const WarhammerMiniature& other) const {
        return pointsCost == other.pointsCost &&
               modelName == other.modelName &&
               releaseYear == other.releaseYear;
    }

    bool operator!=(const WarhammerMiniature& other) const {
        return !(*this == other);
    }
};

class WarhammerHashTable {
private:
    std::vector<WarhammerMiniature> table; // Вектор для хранения данных (открытая адресация)
    std::vector<bool> occupied; // Флаг, указывающий, занята ли ячейка
    std::vector<bool> deleted; // Флаг, указывающий, была ли ячейка удалена
    size_t capacity; // Размер таблицы
    size_t count; // Количество элементов

    // Хэш-функция: комбинирование полей с полиномиальным хэшированием строки и битовыми операциями
    size_t hashFunction(const WarhammerMiniature& mini) const {
        size_t hash = 0;
        // Полиномиальное хэширование для строки modelName
        const size_t base = 31;
        for (char c : mini.modelName) {
            hash = hash * base + static_cast<size_t>(c);
        }
        // Комбинируем с числовыми полями с использованием битовых операций
        hash ^= static_cast<size_t>(mini.pointsCost) << 3;
        hash ^= static_cast<size_t>(mini.releaseYear) << 7;
        return hash % capacity;
    }

    // Квадратичное пробирование для разрешения коллизий
    size_t quadraticProbe(size_t index, size_t attempt) const {
        return (index + attempt * attempt) % capacity;
    }

    // Рехэширование при заполнении таблицы
    void rehash() {
        std::vector<WarhammerMiniature> oldTable = table;
        std::vector<bool> oldOccupied = occupied;
        capacity *= 2;
        count = 0;
        table.assign(capacity, WarhammerMiniature());
        occupied.assign(capacity, false);
        deleted.assign(capacity, false);

        // Перевставляем все существующие элементы
        for (size_t i = 0; i < oldTable.size(); ++i) {
            if (oldOccupied[i] && !deleted[i]) {
                insert(oldTable[i]);
            }
        }
    }

public:
    WarhammerHashTable(size_t initialCapacity = 5)
        : capacity(initialCapacity), count(0) {
        table.assign(capacity, WarhammerMiniature());
        occupied.assign(capacity, false);
        deleted.assign(capacity, false);
    }

    // Вставка элемента
    void insert(const WarhammerMiniature& mini) {
        // Проверяем уровень заполнения (50%)
        if (static_cast<double>(count) / capacity >= 0.5) {
            rehash();
        }

        size_t index = hashFunction(mini);
        size_t attempt = 0;

        while (occupied[index] && !deleted[index] && table[index] != mini) {
            attempt++;
            index = quadraticProbe(hashFunction(mini), attempt);
            if (attempt >= capacity) {
                rehash();
                insert(mini);
                return;
            }
        }

        table[index] = mini;
        occupied[index] = true;
        deleted[index] = false;
        count++;
    }

    // Поиск элемента
    bool find(const WarhammerMiniature& mini, WarhammerMiniature& result) const {
        size_t index = hashFunction(mini);
        size_t attempt = 0;

        while (occupied[index] || deleted[index]) {
            if (occupied[index] && !deleted[index] && table[index] == mini) {
                result = table[index];
                return true;
            }
            attempt++;
            index = quadraticProbe(hashFunction(mini), attempt);
            if (attempt >= capacity) {
                break;
            }
        }
        return false;
    }

    // Удаление элемента
    bool remove(const WarhammerMiniature& mini) {
        size_t index = hashFunction(mini);
        size_t attempt = 0;

        while (occupied[index] || deleted[index]) {
            if (occupied[index] && !deleted[index] && table[index] == mini) {
                deleted[index] = true;
                count--;
                return true;
            }
            attempt++;
            index = quadraticProbe(hashFunction(mini), attempt);
            if (attempt >= capacity) {
                break;
            }
        }
        return false;
    }

    // Вывод таблицы
    void print() const {
        std::cout << "Hash Table (capacity: " << capacity << ", elements: " << count << "):\n";
        for (size_t i = 0; i < capacity; ++i) {
            std::cout << "Index " << i << ": ";
            if (occupied[i]) {
                if (deleted[i]) {
                    std::cout << "[Deleted]";
                } else {
                    std::cout << table[i].modelName << " (Cost: " << table[i].pointsCost
                              << ", Year: " << table[i].releaseYear << ")";
                }
            } else {
                std::cout << "[Empty]";
            }
            std::cout << std::endl;
        }
    }
};

// Пример использования
int main() {
    WarhammerHashTable ht(5);

    // Вставка элементов
    ht.insert(WarhammerMiniature(100, "Space Marine Captain", 2020));
    ht.insert(WarhammerMiniature(50, "Imperial Guard", 2019));
    ht.insert(WarhammerMiniature(75, "Ork Warboss", 2021));

    // Вывод таблицы
    ht.print();

    // Поиск элемента
    WarhammerMiniature result;
    if (ht.find(WarhammerMiniature(100, "Space Marine Captain", 2020), result)) {
        std::cout << "\nFound: " << result.modelName << " (Cost: " << result.pointsCost << ")\n";
    } else {
        std::cout << "\nNot found\n";
    }

    // Удаление элемента
    if (ht.remove(WarhammerMiniature(50, "Imperial Guard", 2019))) {
        std::cout << "Removed Imperial Guard\n";
    } else {
        std::cout << "Failed to remove Imperial Guard\n";
    }

    // Вывод таблицы после удаления
    ht.print();

    return 0;
}



