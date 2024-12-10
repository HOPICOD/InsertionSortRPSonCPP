#include <iostream>
using namespace std;


// Функция для ввода целого числа с проверкой на корректность ввода и диапазон значений
int inputInt(string message, int min, int max) {
	int userInput;
	cout << message << endl;
	cin >> userInput;
	while (cin.fail() || userInput <= min || userInput >= max) { // Проверка на ошибку ввода или выход за пределы диапазона
		cin.clear(); // Сбрасываем флаг ошибки
		cin.ignore(INT_MAX, '\n'); // Очищаем буфер ввода
		cout << "Неверный ввод!" << endl;
		cin >> userInput;
	}
	cin.clear();// Сбрасываем флаг ошибки
	cin.ignore(INT_MAX, '\n'); // Очищаем буфер ввода
	return userInput;
}

// Функция для ввода числа с плавающей точкой с проверкой на корректность ввода и диапазон значений
double inputDouble(string message, double min, double max) {
	double userInput;
	cout << message << endl;
	cin >> userInput;
	while (cin.fail() || userInput < min || userInput > max) { // Проверка на ошибку ввода или выход за пределы диапазона
		cin.clear(); // Сбрасываем флаг ошибки
		cin.ignore(INT_MAX, '\n'); // Очищаем буфер ввода
		cout << "Неверный ввод!" << endl;
		cin >> userInput;
	}
	cin.clear();// Сбрасываем флаг ошибки
	cin.ignore(INT_MAX, '\n'); // Очищаем буфер ввода
	return userInput;
}