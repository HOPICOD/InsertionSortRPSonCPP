﻿#include <iostream>
using namespace std;

// Функция вывода приветствия
void showGreeting() {
	cout << "Вторая лабораторная работа." << endl;
	cout << "Студент: Бахметов Кирилл Алексеевич. Группа 434." << endl;
	cout << "Вариант 12." << endl;
	cout << "Цель работы: реализовать программу сортировки вставками." << endl;
	cout << endl << endl << endl;
}
// Функция вывода меню
void showMenu() {
	cout << "[1] - Добавить массив ручным вводом." << endl;
	cout << "[2] - Добавить массив из файла." << endl;
	cout << "[3] - Отсортировать массив." << endl;
	cout << "[4] - Выход." << endl;
}