#include <pqxx/pqxx>  // ��� ������ � PostgreSQL
#include <iostream> 
#include "arrayInteraction.h"

using namespace std;

// ������� ��� ����������� � ���� ������ � ���������� SQL-��������
void connectAndSaveData(vector<double>& unsortedArray, vector<double>& sortedArray) {

    try {
        // ��������� �����������
        pqxx::connection C("dbname=DesktopInsertionSort user=postgres password=Hp78219038 host=localhost port=5432");

        if (C.is_open()) {
            std::cout << "Connected to database successfully: " << C.dbname() << std::endl;

            pqxx::work W(C);  // �������� ����������

            // ����������� ������� � ������ ������� PostgreSQL ��� ��������
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

            // ������� � �������
            std::string query = "INSERT INTO arrays_table (unsorted_array, sorted_array) VALUES ('" +
                W.esc(unsortedArrayStr) + "', '" + W.esc(sortedArrayStr) + "');";
            W.exec(query);
            W.commit();  // ��������� ����������

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
// ������� ������ �������� ����������� � ���� ������
vector<pair<int, pair<vector<double>, vector<double>>>> getAllArraysFromDatabase(pqxx::connection& C) {
    vector<pair<int, pair<vector<double>, vector<double>>>> data;

    try {
        pqxx::work W(C);
        string query = "SELECT id, unsorted_array, sorted_array FROM arrays_table;";
        pqxx::result R = W.exec(query);

        for (const auto& row : R) {
            int id = row[0].as<int>();

            // ������� unsorted_array
            string unsortedStr = row[1].c_str();
            unsortedStr.erase(remove(unsortedStr.begin(), unsortedStr.end(), '{'), unsortedStr.end());
            unsortedStr.erase(remove(unsortedStr.begin(), unsortedStr.end(), '}'), unsortedStr.end());

            stringstream unsortedStream(unsortedStr);
            vector<double> unsortedArray;
            string token;
            while (getline(unsortedStream, token, ',')) {
                unsortedArray.push_back(stod(token));
            }

            // ������� sorted_array
            string sortedStr = row[2].c_str();
            sortedStr.erase(remove(sortedStr.begin(), sortedStr.end(), '{'), sortedStr.end());
            sortedStr.erase(remove(sortedStr.begin(), sortedStr.end(), '}'), sortedStr.end());

            stringstream sortedStream(sortedStr);
            vector<double> sortedArray;
            while (getline(sortedStream, token, ',')) {
                sortedArray.push_back(stod(token));
            }

            // ��������� ������ � ������
            data.emplace_back(id, make_pair(unsortedArray, sortedArray));
        }

        W.commit();
    }
    catch (exception& e) {
        throw runtime_error(string("������ �������� ������: ") + e.what());
    }

    return data;
}

// ������� ��� �������� ������� �� ���� ������
vector<double> getArrayFromDatabase(pqxx::connection& C, int id) {
    pqxx::work W(C);
    string query = "SELECT unsorted_array FROM arrays_table WHERE id = " + to_string(id);
    pqxx::result R = W.exec(query);

    vector<double> array;
    if (!R.empty()) {
        string arrayStr = R[0][0].as<string>(); // �������� ������ � ��������
        arrayStr.erase(0, 1); // ������� ����������� ������
        arrayStr.erase(arrayStr.size() - 1); // ������� ����������� ������

        stringstream ss(arrayStr);
        string token;
        while (getline(ss, token, ',')) {
            array.push_back(stod(token)); // ����������� ������ � �����
        }
    }
    return array;
}

// ������� ��� �������� ������� �� id
bool isIdCorrect(pqxx::connection& C, int arrayId, vector<double>& unsortedArray, vector<double>& sortedArray) {
    pqxx::work W(C);
    stringstream query;
    query << "SELECT unsorted_array, sorted_array FROM arrays_table WHERE id = " << arrayId;

    pqxx::result R = W.exec(query.str());

    if (R.empty()) {
        return false;  // ������ � ����� id �� ������
    }
    return true;  // ������ ������
}