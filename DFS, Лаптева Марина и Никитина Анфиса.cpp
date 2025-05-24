#include <iostream>
#include <map>
#include <vector>
#include <stack>
#include <set>
#include <thread>
#include <chrono>
#include <string>
#include <algorithm>

using std::cout;
using std::endl;
using std::map;
using std::pair;
using std::set;
using std::stack;
using std::string;
using std::to_string;
using std::vector;
using std::min;
using std::max;
using std::this_thread::sleep_for;
using std::chrono::milliseconds;

const int CANVAS_HEIGHT = 9; // высота холста
const int CANVAS_WIDTH = 25; // ширина холста

// Граф (создаем мапу с ключем - вершиной и значением - списком связанные с ним вершины) в котором у вершины 1 связи с 5 и 6, 2 вершины связи с 3, 4, 5)
map<int, vector<int>> graph = {
    {1, {5, 6}},
    {2, {3, 4, 5}},
    {3, {2}},
    {4, {2, 5, 6}},
    {5, {1, 2, 4}},
    {6, {1, 4, 7}},
    {7, {6}}
};

// Позиции на экране  
map<int, pair<int, int>> positions = {
    {1, {4, 4}},
    {2, {2, 14}},
    {3, {0, 14}},
    {4, {2, 9}},
    {5, {4, 14}},
    {6, {7, 9}},
    {7, {7, 14}}
};

// Холст (создаем пустой канвас на котором будем рисовать)
vector<string> canvas(CANVAS_HEIGHT, string(CANVAS_WIDTH, ' '));

// Функция, которая рисует вершину под номером id
void drawVertex(int id, bool visited = false) {
    // Получаем положение вершины по ее номеру из мапы с позициями
    pair<int, int> pos = positions[id];
    int row = pos.first;
    int col = pos.second;

    // Выбираем как отобразить вершину, как * (если посетили)или ее номер (если не посещали)
    string label = visited ? "[*]" : "[" + to_string(id) + "]";
    for (int i = 0; i < 3; ++i) {
        if (col + i < (int)canvas[0].size()) {
            canvas[row][col + i] = label[i];
        }
    }
}

// Функция, которая рисует связи между вершинами 
void drawEdge(int from, int to) {
    // Берем положения вершин
    pair<int, int> pos1 = positions[from];
    pair<int, int> pos2 = positions[to];
    int y1 = pos1.first, x1 = pos1.second;
    int y2 = pos2.first, x2 = pos2.second;

    if (y1 == y2) {
        // Рисуем - для связи вершин у которых одинаковый y
        for (int x = min(x1, x2) + 3; x < max(x1, x2); ++x) {
            canvas[y1][x] = '-';
        }
    }
    else if (x1 == x2) {
        // Рисуем | для связи вершин у которых одинаковый x
        for (int y = min(y1, y2) + 1; y < max(y1, y2); ++y) {
            canvas[y][x1 + 1] = '|';
        }
    }
    else {
        // Диагональ
        int dy = (y2 > y1) ? 1 : -1; // тернарный оператор (если y2>y1) то dy = 1, (если y2 <= y1) то dy = -1
        int dx = (x2 > x1) ? 1 : -1;
        int y = y1 + 1, x = x1 + 2; 

        // Рисуем диагональные линии
        if (from != 4 && to != 4) {
            while (y != y2 && x != x2) {
                if (y >= 0 && y < CANVAS_HEIGHT && x >= 0 && x < CANVAS_WIDTH) {
                    canvas[y][x] = (dy == dx) ? '\\' : '/';
                }
                y += dy; // движемся по вертикали к второй вершине
                x += dx; // движемся по горизонтали к второй вершине
            }
        }
    }
}


void drawGraph(const set<int>& visited) {
    // перед рисованием, очищаем холст от прошлых рисунков
    canvas.assign(CANVAS_HEIGHT, string(CANVAS_WIDTH, ' '));

    // проходим про графу
    for (const auto& item : graph) {
        // получаем номер каждой вершины
        int id = item.first;
        // рисуем вершину, и передаем информацию, проходили ли мы через эту вершину
        drawVertex(id, visited.count(id) > 0);
    }

    // проходим про графу
    for (const auto& item : graph) {
        // получаем номер каждой вершины
        int from = item.first;
        const vector<int>& toList = item.second;
        // от одной вершины может выходить несколько связей, поэтому мы проходим по всем toList (по всем вершинам в которые будет рисоваться связь)
        for (int to : toList) {
            // у нас неориентированный граф, то есть если есть ребро между 1 и 2, оно записано у обеих вершин:
            // graph[1] = {2}
            // graph[2] = {1}
            // поэтому мы рисуем только тогда, когда from < to
            if (from < to) {
                drawEdge(from, to);
            }
        }
    }

    // отрисовка
    for (const string& line : canvas) {
        cout << line << '\n';
    }
    cout << endl;
}

// алгоритм поиска
void dfs(int start) {
    set<int> visited;  // Множество посещённых вершин, чтобы не заходить в одну и ту же дважды
    stack<int> s;      // Стек для хранения вершин, которые надо посетить (стек это структура данных, работающая по правилу FIFO)
    s.push(start);     // Начинаем обход с вершины start — кладём её в стек

    while (!s.empty()) { 
        int current = s.top();  // Пока есть вершины в стеке, берём вершину сверху стека (последнюю добавленную)
        s.pop();                // Убираем её из стека, т.к. сейчас будем её обрабатывать

        if (visited.count(current)) 
            continue;           // Если вершина уже посещена, пропускаем и берём следующую

        visited.insert(current); // Отмечаем вершину как посещённую

        drawGraph(visited);      // Рисуем граф с учётом посещённых вершин (для визуализации процесса)
        sleep_for(milliseconds(3000)); // Пауза, чтобы можно было увидеть текущий шаг

        const vector<int>& neighbors = graph[current];  // Получаем соседей текущей вершины
        // Проходим соседей в обратном порядке (чтобы обход был в "правильном" порядке)
        // rbegin - начало обхода в обратном порядке
        for (auto it = neighbors.rbegin(); it != neighbors.rend(); ++it) {
            if (!visited.count(*it))   // Если сосед ещё не посещён
                s.push(*it);           // Добавляем его в стек для будущего посещения
        }
    }
}

int main() {
    dfs(5);
    return 0;
}
