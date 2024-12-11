#include <iostream>
#include <random>
#include <pqxx/pqxx>
#include <chrono>
#include <vector>
#include "InsertionSort.h"

using namespace std;
using namespace chrono;

// Генерация случайного массива случайного размера
vector<double> generateRandomArray() {
    random_device rd; // Источник случайных чисел
    mt19937 gen(rd()); // Генератор случайных чисел
    uniform_int_distribution<> sizeDist(10, 50);  // Размер массива от 10 до 50
    uniform_real_distribution<> valueDist(-1000.0, 1000.0); // Значения от -1000 до 1000

    size_t size = sizeDist(gen); // Случайный размер массива
    vector<double> arr(size);

    for (size_t i = 0; i < size; ++i) {
        arr[i] = valueDist(gen); // Заполнение массива случайными числами
    }
    return arr;
}

// Добавление массива в базу данных
void addArrayToDatabase(pqxx::connection& C, const vector<double>& unsortedArray) {
    pqxx::work W(C); // Начало транзакции
    // Преобразование массива в строку PostgreSQL
    auto arrayToPostgresString = [](const std::vector<double>& array) {
        std::ostringstream oss; // Поток для формирования строки
        oss << "{";
        for (size_t i = 0; i < array.size(); ++i) {
            oss << array[i];
            if (i != array.size() - 1) { // Если не последний элемент, добавляем запятую
                oss << ", ";
            }
        }
        oss << "}";
        return oss.str(); // Возвращаем строку
        };

    // Преобразуем несортированный массив
    std::string unsortedArrayStr = arrayToPostgresString(unsortedArray);

    // SQL-запрос
    string query = "INSERT INTO test (unsorted_array) VALUES ('" +
        W.esc(unsortedArrayStr) + "');";
    W.exec(query); // Выполняем запрос
    W.commit(); // Фиксируем изменения
}

// Загрузка массива из базы данных по случайному ID
vector<double> getRandomArrayFromDatabase(pqxx::connection& C) {
    pqxx::work W(C); // Начало транзакции

    // Получение случайного ID
    pqxx::result R = W.exec("SELECT id FROM test;");

    random_device rd; // Источник случайных чисел
    mt19937 gen(rd()); // Генератор случайных чисел
    uniform_int_distribution<> dist(0, R.size() - 1); // Случайный индекс
    int randomID = R[dist(gen)][0].as<int>(); // Случайный ID

    // Получение массива по ID
    string query = "SELECT unsorted_array FROM test WHERE id = " + to_string(randomID) + ";";
    pqxx::result arrayResult = W.exec(query); // Выполняем запрос


    // Разбор строки в массив
    string unsortedStr = arrayResult[0][0].as<string>(); // Получаем строку массива
    vector<double> array;
    // Удаляем фигурные скобки, если они есть
    if (unsortedStr.front() == '{' && unsortedStr.back() == '}') {
        unsortedStr = unsortedStr.substr(1, unsortedStr.size() - 2);
    }

    // Парсим строку по запятым
    stringstream ss(unsortedStr);
    string token;
    while (getline(ss, token, ',')) {
        try {
            array.push_back(stod(token)); // Преобразуем строку в число
        }
        catch (const invalid_argument& e) { // Обработка ошибок преобразования
            throw runtime_error("Ошибка преобразования элемента в число: " + token);
        }
    }

    return array;
}

// Очистка базы данных
void clearDatabase(pqxx::connection& C) {
    pqxx::work W(C); // Начало транзакции
    W.exec("DELETE FROM test;");  // Удаление всех записей из таблицы
    W.commit(); // Фиксируем изменения
}

// Тест добавления массивов
void testAddArrays(pqxx::connection& C, size_t count) {
    cout << "Тест добавления " << count << " массивов...\n";

    auto start = high_resolution_clock::now(); // Запоминаем время начала

    for (size_t i = 0; i < count; ++i) {
        vector<double> randomArray = generateRandomArray(); // Генерация случайного массива
        addArrayToDatabase(C, randomArray);                 // Добавление в базу данных
    }

    auto end = high_resolution_clock::now(); // Время завершения
    auto duration = duration_cast<milliseconds>(end - start); // Вычисляем продолжительность

    cout << "Добавление завершено. Время: " << duration.count() << " мс\n";
}

// Тест сортировки массивов из базы данных
void testSortRandomArrays(pqxx::connection& C, size_t count) {
    cout << "Тест сортировки " << count << " случайных массивов из базы данных...\n";

    InsertionSort sorter;
    size_t processedCount = 0; // Количество успешно обработанных массивов
    microseconds totalDuration(0); // Суммарное время сортировки

    auto totalStart = high_resolution_clock::now(); // Время начала теста

    for (size_t i = 0; i < count; ++i) {
        try {
            // Получение массива и измерение времени сортировки
            auto start = high_resolution_clock::now(); // Время начала сортировки
            vector<double> array = getRandomArrayFromDatabase(C); // Получение массива
            sorter.sortIncrease(array);
            auto end = high_resolution_clock::now(); // Время окончания сортировки

            totalDuration += duration_cast<microseconds>(end - start); // Суммируем время
            processedCount++; // Увеличиваем счетчик
        }
        catch (const runtime_error& e) {
            cerr << "Ошибка при сортировке массива: " << e.what() << "\n";
        }
    }

    auto totalEnd = high_resolution_clock::now(); // Время окончания теста
    auto totalTime = duration_cast<milliseconds>(totalEnd - totalStart); // Общее время теста

    // Вычисление среднего времени
    double averageTime = (processedCount > 0) ? static_cast<double>(totalDuration.count()) / processedCount : 0.0;

    cout << "Сортировка завершена.\n";
    cout << "Общее время: " << totalTime.count() << " мс\n";
    cout << "Среднее время на 1 массив: " << averageTime / 1000.0 << " мс\n"; // Перевод из мкс в мс
}

// Тест очистки базы данных
void testClearDatabase(pqxx::connection& C) {
    cout << "Тест очистки базы данных...\n";
    auto start = high_resolution_clock::now(); // Время начала
    clearDatabase(C); // Очистка базы данных
    auto end = high_resolution_clock::now(); // Время окончания
    auto duration = duration_cast<milliseconds>(end - start); // Вычисляем продолжительность

    cout << "Очистка завершена. Время: " << duration.count() << " мс\n";
}


int main()
{
    setlocale(LC_ALL, "Russian");
    try {
        // Подключение к базе данных
        pqxx::connection C("dbname=test_insertion_sort user=postgres password=Hp78219038 host=localhost port=5432");

        // Тесты
        testAddArrays(C, 10000); // Добавление 100, 1000, 10000 массивов
        testSortRandomArrays(C, 10000); // Сортировка 100, 1000, 10000 случайных массивов
        testClearDatabase(C); // Очистка базы данных
    }
    catch (const std::exception& e) { // Обработка исключений
        cerr << "Ошибка: " << e.what() << "\n";
    }

    return 0;
}