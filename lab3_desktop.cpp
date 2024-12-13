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

// Перечисление для выбора метода сортировки
enum sortMethod { sortIncrease = 0, sortDecrease = 1 };



int main()
{
    locale::global(locale("C"));
    // Инициализация GLFW и Dear ImGui
    if (!glfwInit()) return -1;
    // Создание окна GLFW
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Программа сортировки методом вставок", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // Установка контекста OpenGL для текущего окна
    glfwSwapInterval(1); // Включение вертикальной синхронизации


    // Инициализация ImGui
    IMGUI_CHECKVERSION();// Проверка версии ImGui
    ImGui::CreateContext();// Создание контекста ImGui
    ImGui_ImplGlfw_InitForOpenGL(window, true);// Инициализация привязки ImGui к GLFW
    ImGuiIO& io = ImGui::GetIO(); // Получение объекта ввода/вывода ImGui
    io.Fonts->AddFontFromFileTTF("D:/VS code/lab3_desktop/font/Roboto.ttf", 20.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());// Добавление шрифта с поддержкой кириллицы
    ImGui_ImplOpenGL3_Init("#version 130");// Инициализация привязки ImGui к OpenGL3

    // Переменные для работы с массивами и сообщениями
    char inputArrayBuf[BUFSIZ] =""; // Буфер для ввода массива
    bool isArraySort = false;
    vector<double> unsortedArray;
    vector<double> sortedArray;
    string resultMessage;
    string errorMessage;
    string saveMessage;
    int arrayIdToEdit = 0; // ID массива для редактирования
    vector<pair<int, pair<vector<double>, vector<double>>>> dbData; // Данные из базы, хранит в себе id, неотсортированный и отсортированный массив

    // Переменная для выбора типа сортировки
    sortMethod sortMethod = sortIncrease;  // По умолчанию сортировка по возрастанию
    const char* sortMethods[] = { "По возрастанию", "По убыванию" }; // Массив названий методов сортировки

    // Создаем объект класса сортировки
    InsertionSort sorter;

    // Основной цикл
    while (!glfwWindowShouldClose(window)) {// Пока окно не закрыто
        glfwPollEvents();// Обработка событий окна
        ImGui_ImplOpenGL3_NewFrame();// Начало нового кадра для OpenGL3
        ImGui_ImplGlfw_NewFrame();// Начало нового кадра для GLFW
        ImGui::NewFrame();// Начало нового кадра для ImGui

        // Интерфейс
        ImGui::Begin("Сортировка методом вставок");

        if (ImGui::Button("Справка", ImVec2(100, 30))) { // Кнопка размером 100x30
            // Выводим модальное окно со справкой
            ImGui::OpenPopup("HelpPopup");
        }

        // Устанавливаем размер окна справки перед его отображением
        ImGui::SetNextWindowSize(ImVec2(500, 250), ImGuiCond_Always); // Размер окна 400x300

        // Описание всплывающего окна справки
        if (ImGui::BeginPopup("HelpPopup")) {
            ImGui::Text("Справка о программе");
            ImGui::Separator();
            ImGui::TextWrapped("Данная программа позволяет сортировать массивы методом вставок. " //Вывод многострочного кода
                "Вы можете работать с массивами из базы данных, "
                "добавлять новые массивы, редактировать и сортировать их.");
            ImGui::TextWrapped("Числа вводятся через запятую, дробная часть разделяется с целой частью точкой.");
            ImGui::Separator(); // Разделитель для элементов
            if (ImGui::Button("Закрыть")) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        // Поле ввода массива
        ImGui::InputText("Введите массив", inputArrayBuf, IM_ARRAYSIZE(inputArrayBuf));

        // Выбор метода сортировки
        ImGui::Combo("Тип сортировки", reinterpret_cast<int*>(&sortMethod), sortMethods, IM_ARRAYSIZE(sortMethods));

        // Кнопка сортировки
        if (ImGui::Button("Сортировать")) {
            isArraySort = true;
            // Преобразуем строку в массив
            unsortedArray.clear();
            sortedArray.clear();
            resultMessage.clear();
            errorMessage.clear();
            string inputString(inputArrayBuf); // Преобразование буфера в строку

            // Проверка на пустой ввод
            if (inputString.empty() || inputString.find_first_not_of(' ') == std::string::npos) {
                errorMessage = "Ошибка: Пустая строка или неверный формат ввода";
            }
            else {
                // Обработка строки ввода
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
                            if (abs(num) >= INT_MAX) {
                                errorMessage = "Ошибка: число '" + token + "' превышает допустимое значение";
                                break;
                            }
                            unsortedArray.push_back(num);
                        }
                        catch (invalid_argument& e) { // Проверка на неверные данные
                            errorMessage = "Ошибка: элемент массива '" + token + "' не является числом.";
                            break;
                        }
                        catch (out_of_range& e) { // Проверка на выход за предел диапазона 
                            errorMessage = "Ошибка: элемент массива '" + token + "' выходит за пределы диапазона.";
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
            ImGui::Text("Отсортированный массив:");
            ImGui::PushTextWrapPos(ImGui::GetContentRegionAvail().x); // Устанавливаем максимальную ширину для переноса
            ImGui::TextWrapped("%s", resultMessage.c_str()); // Используем TextWrapped для переноса текста
            ImGui::PopTextWrapPos(); // Сбрасываем настройки переноса
        }

        // Кнопка для сохранения массивов в базу данных
        if (ImGui::Button("Сохранить массивы")) {
            errorMessage.clear();
            saveMessage.clear();
            if (!isArraySort) {
                resultMessage = "";
                errorMessage = "Ошибка: Новый массив не был отсортирован или попытка сохранить тот-же массив. Сохранение невозможно.";
            }
            else {
                if (unsortedArray.empty() || sortedArray.empty()) {
                    errorMessage = "Ошибка: Один или оба массива пусты! Сохранение невозможно.";
                }
                else {
                    try {
                        // Пересортировываем массив перед сохранением
                        sortedArray = unsortedArray; // Копируем неотсортированный массив
                        if (sortMethod == sortIncrease) {
                            sorter.sortIncrease(sortedArray); // Сортировка по возрастанию
                        }
                        else {
                            sorter.sortDecrease(sortedArray); // Сортировка по убыванию
                        }
                        connectAndSaveData(unsortedArray, sortedArray); // Вызов функции сохранения
                        saveMessage = "Данные сохранены успешно!";
                    }
                    catch (exception& e) {
                        errorMessage = string("Ошибка при сохранении: ") + e.what();
                    }
                }
            }
            isArraySort = false;
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
                //Проверка на правильный ID
                if (isIdCorrect(C, arrayIdToEdit, unsortedArray, sortedArray)) {
                    // Загружаем массив из базы данных с указанным ID
                    unsortedArray = getArrayFromDatabase(C, arrayIdToEdit);
                    // Преобразуем массив в строку и помещаем в буфер для отображения в поле ввода
                    stringstream ss;
                    for (size_t i = 0; i < unsortedArray.size(); ++i) {
                        if (i != 0) ss << ", "; // Добавляем запятые между числами
                        ss << unsortedArray[i]; // Добавляем число
                    }
                    string arrayStr = ss.str(); // Конвертируем поток в строку
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


        // Кнопка для загрузки всех массивов из базы данных
        if (ImGui::Button("Загрузить все массивы")) {
            try {
                pqxx::connection C("dbname=DesktopInsertionSort user=postgres password=Hp78219038 host=localhost port=5432");
                dbData = getAllArraysFromDatabase(C); // Загружаем все массивы из базы данных
            }
            catch (const std::exception& e) {
                errorMessage = std::string("Ошибка: ") + e.what();
            }
        }
        // Проверяем, есть ли загруженные массивы
        if (!dbData.empty()) {
            ImGui::Text("Список массивов:");
            for (const auto& record : dbData) { // Перебираем каждый загруженный массив
                ImGui::Separator(); // Разделительная линия между элементами списка
                ImGui::Text("ID: %d", record.first); // Отображаем ID массива
                ImGui::Text("Unsorted Array: %s", arrayToString(record.second.first).c_str()); // Отображаем исходный массив
                ImGui::Text("Sorted Array: %s", arrayToString(record.second.second).c_str()); // Отображаем отсортированный массив
            }
        }


        ImGui::End();// Завершение окна ImGui

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

