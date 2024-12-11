#include <pqxx/pqxx>  // Для работы с PostgreSQL
#include <iostream> 
#include "arrayInteraction.h"

using namespace std;

// Функция для подключения к базе данных и выполнения SQL-запросов
void connectAndSaveData(vector<double>& unsortedArray, vector<double>& sortedArray) {

    try {
        // Параметры подключения
        pqxx::connection C("dbname=DesktopInsertionSort user=postgres password=Hp78219038 host=localhost port=5432");

        if (C.is_open()) {
            std::cout << "Connected to database successfully: " << C.dbname() << std::endl;

            pqxx::work W(C);  // Создание транзакции

            // Преобразуем массивы в строку формата PostgreSQL для массивов
            auto arrayToPostgresString = [](const std::vector<double>& array) {
                std::ostringstream oss;
                oss << "{";
                for (size_t i = 0; i < array.size(); ++i) {
                    oss << array[i];
                    if (i != array.size() - 1) {
                        oss << ", ";
                    }
                }
                oss << "}";
                return oss.str();
                };

            std::string unsortedArrayStr = arrayToPostgresString(unsortedArray);
            std::string sortedArrayStr = arrayToPostgresString(sortedArray);

            // Вставка в таблицу
            std::string query = "INSERT INTO arrays_table (unsorted_array, sorted_array) VALUES ('" +
                W.esc(unsortedArrayStr) + "', '" + W.esc(sortedArrayStr) + "');";
            W.exec(query);
            W.commit();  // Завершаем транзакцию

            std::cout << "Arrays save succesfully!" << std::endl;
        }
        else {
            std::cerr << "Failed to connect to database!" << std::endl;
        }
    }
    catch (const std::exception& e) {
        cerr << "Error: " << e.what() << std::endl;
    }
}
// функция выдачи массивов находящихся в базе данных
vector<pair<int, pair<vector<double>, vector<double>>>> getAllArraysFromDatabase(pqxx::connection& C) {
    vector<pair<int, pair<vector<double>, vector<double>>>> data; // Вектор для хранения данных

    try {
        pqxx::work W(C); // Создание транзакции
        string query = "SELECT id, unsorted_array, sorted_array FROM arrays_table;";
        pqxx::result R = W.exec(query); // Выполнение запроса и получение результата  

        // Проходим по всем строкам результата
        for (const auto& row : R) {
            int id = row[0].as<int>(); // Извлекаем ID массива

            // Парсинг unsorted_array
            string unsortedStr = row[1].c_str();
            // Удаляем '{' и '}'
            unsortedStr.erase(remove(unsortedStr.begin(), unsortedStr.end(), '{'), unsortedStr.end());
            unsortedStr.erase(remove(unsortedStr.begin(), unsortedStr.end(), '}'), unsortedStr.end());

            stringstream unsortedStream(unsortedStr);
            vector<double> unsortedArray;
            string token;
            while (getline(unsortedStream, token, ',')) {
                unsortedArray.push_back(stod(token));  // Преобразуем строку в число и добавляем в вектор
            }

            // Парсинг sorted_array
            string sortedStr = row[2].c_str();
            // Удаляем '{' и '}'
            sortedStr.erase(remove(sortedStr.begin(), sortedStr.end(), '{'), sortedStr.end());
            sortedStr.erase(remove(sortedStr.begin(), sortedStr.end(), '}'), sortedStr.end());

            stringstream sortedStream(sortedStr);
            vector<double> sortedArray;
            while (getline(sortedStream, token, ',')) {
                sortedArray.push_back(stod(token)); // Преобразуем строку в число и добавляем в вектор
            }

            // Добавляем ID, несортированный и отсортированный массив в итоговый список
            data.emplace_back(id, make_pair(unsortedArray, sortedArray));
        }

        W.commit();// Фиксация транзакции
    }
    catch (exception& e) {
        throw runtime_error(string("Ошибка загрузки данных: ") + e.what());
    }

    return data;
}

// Функция для загрузки массива из базы данных
vector<double> getArrayFromDatabase(pqxx::connection& C, int id) {
    pqxx::work W(C); // Создание транзакции
    string query = "SELECT unsorted_array FROM arrays_table WHERE id = " + to_string(id);
    pqxx::result R = W.exec(query); // Выполнение запроса

    vector<double> array;
    if (!R.empty()) { // Проверяем, что результат не пустой
        string arrayStr = R[0][0].as<string>(); // Получаем строку с массивом
        arrayStr.erase(0, 1); // Убираем открывающую скобку
        arrayStr.erase(arrayStr.size() - 1); // Убираем закрывающую скобку

        stringstream ss(arrayStr);
        string token;
        while (getline(ss, token, ',')) {
            array.push_back(stod(token)); // Преобразуем строку в число и добавляем в вектор
        }
    }
    return array;
}

// Функция для загрузки массива по id
bool isIdCorrect(pqxx::connection& C, int arrayId, vector<double>& unsortedArray, vector<double>& sortedArray) {
    pqxx::work W(C);  // Создание транзакции
    stringstream query;
    query << "SELECT unsorted_array, sorted_array FROM arrays_table WHERE id = " << arrayId;

    pqxx::result R = W.exec(query.str()); // Выполнение запроса

    if (R.empty()) {
        return false;  // Массив с таким id не найден
    }
    return true;  // Массив найден
}