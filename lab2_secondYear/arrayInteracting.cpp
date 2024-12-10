#include <iostream>
#include <vector>
#include "input.h"
#include "workWithFiles.h"

using namespace std;
enum saveMenu { Yes = 1, No = 2 };

// Функция для замены всех "отрицательных нулей" (значений -0.0) на обычный 0.0 в массиве
void replaceNegativeZero(vector<double>& array) {
	for (int i = 0; i < array.size(); i++) {
		double val = 0.0;
		if (array[i] == 0.0) {
			array[i] = val;
		}
	}
}

// Функция для создания массива на основе пользовательского ввода
vector<double> setArrayByUserInput() {
	int arraySize = inputInt("Введите размер массива:", 0, INT_MAX);
	vector<double> array;

	for (int i = 0; i < arraySize; i++) {
		cout << "Введите " << i + 1 << " элемент массива: ";
		array.push_back(inputDouble("", -DBL_MAX, DBL_MAX));
	}
	replaceNegativeZero(array);
	return array;
}

// Функция для вывода массива на экран
void printArray(vector<double>& array, size_t size, string message) {
	if (array.empty()) { return; }
	cout << message << endl << endl;
	cout << "[ ";
	for (int i = 0; i < size; i++) {
		cout << array[i];
		if (i != size - 1) {
			cout << ", ";
		}
	}
	cout << "]";
	cout << endl;
	cout << endl;
}

// Функция для загрузки данных из файла в массив
void loadDataFromFile(vector<double>& array) {
	string filepath = getValidFilepath(); // Запрашиваем путь к файлу у пользователя

	vector<double> arrayFromFile = loadArrayFromFile(filepath); // Загружаем массив из файла

	if (arrayFromFile.empty()) { cerr << "Ошибка чтения из файла" << endl; } // Проверка на пустой массив
	else {
		array = arrayFromFile; // Присваиваем загруженные данные массиву
	}
}

// Функция для запроса у пользователя о необходимости сохранения массива
void isNeedToSaveArrays(vector<double>& array, vector<double>& arrayForInsertionSort) {
	int userChoice = 0;
	userChoice = inputInt("Хотите ли сохранить массивы? \n[1]Да\n[2]Нет", 0, 3);
	switch (userChoice) {
	case(Yes): {
		saveArraysWithResultsToFile(array, arrayForInsertionSort);
	}
	case(No): {
		break;
	}
	}
}

