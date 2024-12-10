#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <regex>
#include "input.h"
#include "arrayInteracting.h"

using namespace std;

// Функция для проверки существования файла по заданному имени
bool fileExists(string filename) {
	ifstream file(filename); // Открываем файл
	return file.is_open(); // Возвращаем true, если файл открыт, иначе false
}

// Функция для подтверждения перезаписи существующего файла
bool confirmOverwrite(string filename) {

	const int YES = 1;

	cout << "Файл '" << filename << "' уже существует." << endl;
	cout << "Желаете ли вы его перезаписать?" << endl;
	cout << "[1] - Да" << endl;
	cout << "[2] - Нет" << endl;

	int userChoice = 0;

	userChoice = inputInt("Введите пункт меню:", 0, 3);

	if (userChoice == YES) { return true; }

	return false;

}

// Функция для проверки корректности пути к файлу
bool isFilepathValid(string filepath) {

	regex filepathRegex("^(?:[a-zA-Z]\\:|\\\\)\\\\([^\\\\]+\\\\)*[^\\/:*?\"<>|]+\\.txt$"); // Регулярное выражение для проверки пути

	if (!regex_match(filepath, filepathRegex)) { // Если путь не соответствует шаблону
		cerr << "Ошибка: неверный путь файла!" << endl;
		return false;
	}

	return true;
}

// Функция для проверки корректности имени файла
bool isFilenameValid(string filename) {
	regex filenameRegex("^[^\\/:*?\"<>|]+\\.txt$"); // Шаблон для проверки расширения .txt

	regex filenameReservedNames("^(?:CON|PRN|AUX|NUL|COM[1-9]|LPT[1-9]|con|prn|aux|nul|com[1-9]|lpt[1-9])(?:\\..*)?$");// Зарезервированные имена

	regex filenameReservedChars("[\\/:*?\"<>|]"); // Запрещенные символы

	// Проверки на правильность имени файла
	if (!regex_match(filename, filenameRegex)) {
		cerr << "Ошибка: неправильное имя файла!" << endl;
		return false;
	}

	if (regex_match(filename, filenameReservedNames)) {
		cerr << "Ошибка: неправильное имя файла!" << endl;
		return false;
	}

	if (regex_search(filename, filenameReservedChars)) {
		cerr << "Ошибка: неправильное имя файла!" << endl;
		return false;
	}

	return true;
}

// Функция для получения корректного пути к файлу от пользователя
string getValidFilepath() {
	bool isPathValid = false;
	bool isNameValid = false;

	string filepath{ "" };

	while (!isPathValid || !isNameValid) {
		cout << "Введите полный путь файла с его названием (разрешен только формат .txt): " << endl;
		getline(cin, filepath); // Чтение пути к файлу
		size_t pos = filepath.find_last_of("\\"); // Находим последний ввод символа '\'

		// Если указано только имя файла (без разделителей пути)
		// Проверяем, является ли оно корректным
		if (pos > sizeof(filepath)) { // Проверка корректности имени файла

			if (isFilenameValid(filepath)) {

				isPathValid = true;
				isNameValid = true;

			}
		}
		// Если указан полный путь с именем файла
		// Проверяем, является ли оно корректным
		if (pos != string::npos) {
			string filename = filepath.substr(pos + 1); // Извлекаем имя файла после последнего '\'
			string path = filepath.substr(0, pos + 1); // Извлекаем путь к файлу до последнего '\'

			if (isFilepathValid(path + filename) && isFilenameValid(filename)) {
				isPathValid = true;
				isNameValid = true;
			}

		}
	}

	return filepath;
}

// Функция для подтверждения перезаписи существующего файла
string getOverwriteConfirmation(string& fullPath) {
	if (fileExists(fullPath)) { // Проверка существования файла
		if (!confirmOverwrite(fullPath)) { // Если пользователь не подтвердил перезапись
			cout << "Пожалуйста, выберите другой файл." << endl;
			fullPath = getValidFilepath(); // Запрашиваем новый путь
			return getOverwriteConfirmation(fullPath); // Рекурсивно запрашиваем функцию для подтверждения
		}
	}
	return fullPath; // Возвращаем корректный путь
}

// Функция для загрузки чисел из файла в вектор
vector<double> loadArrayFromFile(string filename) {

	vector<double> array;

	ifstream file(filename); // Открываем файл для чтения
	if (!file.is_open()) { // Проверка, удалось ли открыть файл
		cerr << "Невозможно открыть файл " << filename << endl;
		return array;
	}

	string line;
	while (getline(file, line)) { // Чтение строки из файла
		double num;
		istringstream iss(line); // Создаем поток для чтения строки
		bool validLine = true;
		bool anyNumber = false;
		while (iss >> num) { // Чтение числа из строки
			anyNumber = true;
			if (!anyNumber) { // Проверка на отсутствие чисел
				validLine = false;
			}
			if (validLine) { // Добавляем число в массив, если строка корректна
				array.push_back(num);
			}
			file.close(); // Закрываем файл после завершения чтения
		}
		replaceNegativeZero(array); // Замена отрицательных нулей в массиве
		return array;
	}
}

// Функция для сохранения массивов с результатами в файл
void saveArraysWithResultsToFile(vector<double>& array, vector<double>& arrayForInsertionSort) {
	string fullPath = getValidFilepath(); // Получаем корректный путь к файлу
	fullPath = getOverwriteConfirmation(fullPath); // Проверяем перезапись, если файл существует

	ofstream file(fullPath); // Открываем файл для записи
	if (!file) { // Проверка на успешное открытие файла
		cerr << "Ошибка: файл находится в режиме:" << "'Только для чтения' " << "или произошла другая ошибка!" << endl;
		file.close();

		while (true) { // Повторяем запрос пути, если не удалось открыть файл
			cout << "Пожалуйста, введите новый путь." << endl;
			fullPath = getValidFilepath();
			fullPath = getOverwriteConfirmation(fullPath);

			file.open(fullPath);
			if (file) { // Если файл открыт, выходим из цикла
				break;
			}
			else {
				cerr << "Ошибка: файл находится в режиме:" << "'Только для чтения' " << "или произошла другая ошибка!" << endl;
				file.close();
			}
		}
	}
	// Записываем оригинальный массив в файл
	file << "Оригинальный массив:" << endl;
		for (size_t i = 0; i < array.size(); ++i) {
			file << array[i];
			if (i < array.size() - 1) {
				file << " "; // Разделение элементов в строке пробелом
			}
		}
	file << endl << endl;

	// Записываем отсортированный массив в файл
	file << "Массив отсортированный методом вставок:" << endl;
	for (size_t i = 0; i < arrayForInsertionSort.size(); ++i) {
		file << arrayForInsertionSort[i];
		if (i < arrayForInsertionSort.size() - 1) {
			file << " "; // Разделение элементов в строке пробелом
		}
	}
	file << endl << endl;
}