# cpp-spreadsheet
Проект: Электронная таблица<br>
<br>
Читать на других языках: [English](README.md), [Русский](README.RUS.md)<br>

# Описание программы
«Электронная таблица»‎ это упрощённый аналог существующих решений: лист таблицы Microsoft Excel или Google Sheets. В ячейках таблицы могут быть текст или формулы. Формулы, как и в существующих решениях, могут содержать индексы ячеек.<br>

# Сборка при помощи Cmake
To build this project on linux you need:<br>
1) If you don't have Cmake installed, install Cmake<br>
2) [ANTLR](https://www.antlr.org/) is used during the program operation<br>
ANTLR is written in Java, so you will need Java to work with it:<br>
Install **Java SE Runtime Environment**. This is necessary for the "Spreadsheet" assembly.<br>
3) If the "Debug" or "Release" folders are not created:<br>

```
mkdir ../Debug
mkdir ../Release
```
4) Go to the `spreadsheet` folder and run the command for Debug and/or Release conf:<br>

```
cmake -E chdir ../Debug/ cmake -G "Unix Makefiles" ../spreadsheet/ -DCMAKE_BUILD_TYPE:STRING=Debug
cmake -E chdir ../Release/ cmake -G "Unix Makefiles" ../spreadsheet/ -DCMAKE_BUILD_TYPE:STRING=Release 
```
5) Go to "Debug" or "Release" folder and build:<br>

```
cmake --build .
```

6) To **Run** program - in the debug or release folder run:<br>

```
./spreadsheet
```
# Системые требования:
  1. C++17(STL)
  2. GCC (MinG w64) 11.2.0  
  
# Паны по доработке:
1. Добавить UI<br>
2. Добавить поддержку выполнения дополнительных функций над ячейками<br>
3. Добавить возможность сохранения и открытия таблиц в/из файлов.<br>

# Стек технологий:
  1. ANTLR <br>
  2. AST - abstract syntax tree<br>
  3. Наследование и полиморфизм, абстрактные классы, интерфейсы<br>
  4. Поиск циклических зависимостей<br>
  5. Кеширование данных<br>
  6. Обработка ошибок<br>
  7. Юнит тестирование<br>

# Использование

## Перед тем как начать:
  0. Установка и настройкка всех требуемых компонентов в среде разработки длля запуска приложения<br>
  1. Вариант использования и тесты показаны в main.cpp.<br>

# Описание возможностей:
## Ячейки
Ячейка таблицы задаётся своим индексом, то есть строкой вида “А1”, “С14” или “RD2”. Причём ячейка с индексом “А1” — это ячейка в левом верхнем углу таблицы. Количество строк и столбцов в таблице не превышает 16384. То есть предельная позиция ячейки — (16383, 16383) с индексом “XFD16384”. Если позиция ячейки выходит за эти границы, то она не валидна по определению.<br>
<br>
Структура  **Position** в файле common.h. Она содержит поля introw и int col — номера строки и столбца ячейки в программном представлении. Используется для доступа к ячейкам.<br>

## Индексы
Пользователь имеет доступ к ячейке по индексу, то есть по строке вида “А1” или “RD2”. Функции для конвертации: `Position::FromString()` и `Position::ToString()`.<br>

## Минимальная печатная область
Чтобы напечатать таблицу, нужно знать размер минимальной печатной области. Это минимальная прямоугольная область с вершиной в ячейке A1, содержащая все непустые ячейки.<br>
Структура **Size** определена в файле common.h. Она содержит количество строк и столбцов в минимальной печатной области.<br>

## Методы, обращающиеся к ячейке по индексу:
 - `SetCell(Position, std::string)` задаёт содержимое ячейки по индексу **Position**. Если ячейка пуста, надо её создать. Нужно задать ячейке текст методом `Cell::Set(std::string)`;<br>
 - `Cell* GetCell(Position pos)` константный и неконстантный геттеры, которые возвращают указатель на ячейку, расположенную по индексу pos. Если ячейка пуста, возвращают nullptr;<br>
 - `void ClearCell(Position pos)` очищает ячейку по индексу. Последующий вызов `GetCell()` для этой ячейки вернёт nullptr. При этом может измениться размер минимальной печатной области.<br>

## Методы, применимые к таблице целиком:
 - `Size GetPrintableSize()` определяет размер минимальной печатной области. Специально для него в файле common.h и определена структура **Size**. Она содержит количество строк и столбцов в минимальной печатной области.<br>
 - Печать таблицы выводит в стандартный поток вывода std::ostream& минимальную прямоугольную печатную область. Ячейки из одной строки разделены табуляцией \t, в конце строки ставится символ перевода строки \n.<br>
 - `void PrintText(std::ostream&)` выводит текстовые представления ячеек:<br>
для текстовых ячеек это текст, который пользователь задал в методе `Set()`, то есть не очищенный от ведущих апострофов ';<br>
для формульных это формула, очищенная от лишних скобок, как `Formula::GetExpression()`, но с ведущим знаком “=”.<br>
 - `void PrintValues(std::ostream&)` выводит значения ячеек — строки, числа или **FormulaError**, — как это определено в `Cells::GetValue()`. <br>

## Вычисление значений в ячейках
Рассмотрим пример. В ячейке С2 записана формула “=A3/A2”. Чтобы её вычислить, надо разделить значение ячейки А3 на значение ячейки А2.<br>
В ячейке А3 находится формула “=1+2*7”. Её легко вычислить: это 15.<br>
В ячейке A2 находится текст “3”. Формально ячейка не формульная. Но её текст можно интерпретировать как число.<br> Поэтому предполагаем, что её значение 3.<br>
Результат 15/3=5.<br>
Если формула содержит индекс пустой ячейки, предполагаем, что значение пустой ячейки — 0.<br>

## Возможные ошибки и исключения
### Ошибки вычисления
В вычислениях могут возникнуть ошибки. Например, «‎деление на 0»‎. <br>
Если делитель равен 0, значение ячейки — ошибка FormulaError типа **#DIV/0!**<br>

### Некорректная формула. 
Если ячейку, чей индекс входит в формулу, нельзя проинтерпретировать как число, возникает ошибка **#VALUE!**<br>
Если в ячейку методом `Sheet::SetCell()` пытаются записать синтаксически некорректную формулу, например =A1+\*, реализация выбросит исключение **FormulaException**, а значение ячейки не изменится. Формула считается синтаксически некорректной, если она не удовлетворяет предоставленной грамматике. <br>

## Некорректная позиция. 
Формула может содержать ссылку на ячейку, которая выходит за границы возможного размера таблицы, например С2 (=А1234567+ZZZZ1). Такая формула может быть создана, но не может быть вычислена, поэтому её вычисление вернёт ошибку **#REF!**<br>
Программно возможно создать экземпляр класса Position c некорректной позицией, например (-1, -1). Если пользователь передаёт её в методы, программа выдаст исключение **InvalidPositionException**. Методы интерфейсов — например `Cell::GetReferencedCells()` — всегда возвращает корректные позиции.<br>

### Циклические зависимости
Таблица должна всегда оставаться корректной. Если ячейки циклически зависят друг от друга, мы не сможем вычислить значения ячеек. Поэтому нельзя позволить, чтобы возникли циклические зависимости между ячейками.<br>
Если пользователь пытается в методе `Sheet::SetCell()` записать в ячейку формулу, которая привела бы к циклической зависимости, реализация выбросит исключение **CircularDependencyException**, а значение ячейки не изменится.<br>

Ошибки распространяются вверх по зависимостям. Например: формула в С4 зависит от С2 (=С2+8). Формула в С2 выдала ошибку вычисления **#VALUE!** Значит, формула в С4 выдаст ту же ошибку при вычислении.<br>

Точно так же распространяются ошибки **#DIV/0!** и **#REF!** Если формула зависит от нескольких ячеек, каждая из которых содержит ошибку вычисления, результирующая ошибка может соответствовать любой из них.<br>

## Этапы вычисления
Формула состоит из операндов и операций.<br>
Какие бывают операции и операнды:<br>
операции могут быть арифметическими (+, -, *, /), логическими (&, |, ⇒, ~);<br>
операнды могут быть числами (1, 5, 1000.01), булевыми переменными (TRUE, FALSE), константами (π, e, g, ν).<br>
<br>
(abstract syntax tree, AST).<br>
Для абстрактного синтаксического дерева мы можем сами определить классы-узлы и добавить в них метод для вычисления. Назовём его, например, `GetValue()`.<br>
<br>
ANTLR — это специальная программа, которая генерирует код лексического и синтаксического анализаторов, а также код для обхода дерева разбора на С++.<br>