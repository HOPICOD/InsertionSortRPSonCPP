#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "InsertionSort.h"
#include "arrayInteraction.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <locale>
#include "workWithDB.h"
#include <pqxx/pqxx> 

using namespace std;

enum sortMethod { sortIncrease = 0, sortDecrease = 1 };



int main()
{
    locale::global(locale("C"));
    // Инициализация GLFW и Dear ImGui
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Программа сортировки методом вставок", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("D:/VS code/lab3_desktop/font/Roboto.ttf", 16.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
    ImGui_ImplOpenGL3_Init("#version 130");

    char inputArrayBuf[256] ="";
    vector<double> unsortedArray;
    vector<double> sortedArray;
    string resultMessage;
    string errorMessage;
    string saveMessage;
    int arrayIdToEdit = 0;
    vector<pair<int, pair<vector<double>, vector<double>>>> dbData;

    // Переменная для выбора типа сортировки
    sortMethod sortMethod = sortIncrease;  // По умолчанию сортировка по возрастанию
    const char* sortMethods[] = { "По возрастанию", "По убыванию" };

    // Создаем объект класса сортировки
    InsertionSort sorter;

    // Основной цикл
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Интерфейс
        ImGui::Begin("Сортировка методом вставок");

        // Поле ввода массива
       // Преобразуем std::string в C-style строку
        ImGui::InputText("Введите массив", inputArrayBuf, IM_ARRAYSIZE(inputArrayBuf));

        // Выбор метода сортировки
        ImGui::Combo("Тип сортировки", reinterpret_cast<int*>(&sortMethod), sortMethods, IM_ARRAYSIZE(sortMethods));

        // Кнопка сортировки
        if (ImGui::Button("Сортировать")) {
            // Преобразуем строку в массив
            unsortedArray.clear();
            sortedArray.clear();
            resultMessage.clear();
            errorMessage.clear();
            string inputString(inputArrayBuf); // Создаем строку для использования в istringstream

            if (inputString.empty() || inputString.find_first_not_of(' ') == std::string::npos) {
                errorMessage = "Ошибка: Пустая строка или неверный формат ввода";
            }
            else {

                stringstream ss(inputString); // Используем stringstream для разбора строки
                string token;

                while (getline(ss, token, ',')) { // Разбиваем строку по запятой
                    // Удаляем пробелы вокруг токена
                    token.erase(0, token.find_first_not_of(' ')); // Удалить начальные пробелы
                    token.erase(token.find_last_not_of(' ') + 1); // Удалить конечные пробелы
                    replace(token.begin(), token.end(), ',', '.');

                    if (!token.empty()) { // Проверяем не пустой ли токен
                        try {
                            double num = stod(token); // Преобразуем строку в число с плавающей запятой
                            unsortedArray.push_back(num);
                        }
                        catch (invalid_argument& e) { // Проверка на неверные данные
                            errorMessage = "Ошибка: токен '" + token + "' не является числом.";
                            break;
                        }
                        catch (out_of_range& e) { // Проверка на выход за предел диапазона 
                            errorMessage = "Ошибка: токен '" + token + "' выходит за пределы диапазона.";
                            break;
                        }
                    }
                }

            }

            if (errorMessage.empty()) {
                // Сортировка массива в зависимости от выбранного типа
                sortedArray = unsortedArray;
                if (sortMethod == sortIncrease) {
                    sorter.sortIncrease(sortedArray); // Сортировка по возрастанию
                }
                else {
                    sorter.sortDecrease(sortedArray); // Сортировка по убыванию
                }

                // Формирование результата
                resultMessage = arrayToString(sortedArray);
            }
        }
       
        // Выводим сообщения об ошибке
        if (!errorMessage.empty()) {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s", errorMessage.c_str());  // Выводим ошибку красным цветом
        }



        // Вывод результата
        if (!resultMessage.empty()) {
            ImGui::Text("Отсортированный массив: %s", resultMessage.c_str());
        }

        // Кнопка для сохранения массивов в базу данных
        if (ImGui::Button("Сохранить массивы в базу данных")) {
            errorMessage.clear();
            saveMessage.clear();
            if (unsortedArray.empty() || sortedArray.empty()) {
                errorMessage = "Ошибка: Один или оба массива пусты! Сохранение невозможно.";
            }
            else {
                try {
                    connectAndSaveData(unsortedArray, sortedArray); // Вызов функции сохранения
                    saveMessage = "Данные сохранены успешно!";
                }
                catch (exception& e) {
                    errorMessage = string("Ошибка при сохранении: ") + e.what();
                }
            }
        }

        // Выводим сообщения об сохранении
        if (!saveMessage.empty()) {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", saveMessage.c_str());  // Выводим успех зеленым цветом
        }

        // Вводим ID массива для редактирования
        ImGui::InputInt("Введите ID массива для редактирования", &arrayIdToEdit);

        // Кнопка для загрузки массива из базы данных
        if (ImGui::Button("Загрузить массив для редактирования")) {
            errorMessage.clear();
            try {
                pqxx::connection C("dbname=DesktopInsertionSort user=postgres password=Hp78219038 host=localhost port=5432");
                if (isIdCorrect(C, arrayIdToEdit, unsortedArray, sortedArray)) {
                    unsortedArray = getArrayFromDatabase(C, arrayIdToEdit); // Загружаем массив с ID = 1
                    // Преобразуем массив в строку и помещаем в буфер
                    stringstream ss;
                    for (size_t i = 0; i < unsortedArray.size(); ++i) {
                        if (i != 0) ss << ", ";
                        ss << unsortedArray[i];
                    }
                    string arrayStr = ss.str();
                    strncpy_s(inputArrayBuf, sizeof(inputArrayBuf), arrayStr.c_str(), _TRUNCATE); // Заполняем буфер для ввода
                }
                else {
                    errorMessage = "Ошибка! Массив под таким номером не найден! Воспользуйтесь списком со всеми массивами, которые находятся в базе данных.";
                }
            }
            catch (exception& e) {
                // Обработка ошибок
            }
        }

        // Кнопка для загрузки всех массивов
        if (ImGui::Button("Загрузить все массивы из базы данных")) {
            try {
                pqxx::connection C("dbname=DesktopInsertionSort user=postgres password=Hp78219038 host=localhost port=5432");
                dbData = getAllArraysFromDatabase(C);
            }
            catch (const std::exception& e) {
                errorMessage = std::string("Ошибка: ") + e.what();
            }
        }

        if (!dbData.empty()) {
            ImGui::Text("Список массивов:");
            for (const auto& record : dbData) {
                ImGui::Separator();
                ImGui::Text("ID: %d", record.first);
                ImGui::Text("Unsorted Array: %s", arrayToString(record.second.first).c_str());
                ImGui::Text("Sorted Array: %s", arrayToString(record.second.second).c_str());
            }
        }


        ImGui::End();

        // Рендеринг
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Очистка
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

