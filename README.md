# cpp-spreadsheet
Project: Spreadsheet<br>
<br>
Read in other languages: [English](README.md), [Русский](README.RUS.md)<br>

# The description
of the Spreadsheet program is a simplified analogue of existing solutions: a Microsoft Excel spreadsheet or Google Sheets. The table cells can contain text or formulas. Formulas, as in existing solutions, can contain cell indexes.<br>

# Build using Cmake
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
# System requirements:
  1. C++17(STL)
  2. GCC (MinG w64) 11.2.0  
  
# Plans for completion:
1. Add UI<br>
2. Add support for performing additional functions on cells<br>
3. Add the ability to save and open tables to/from files.<br>

# Technology stack:
1. ANTLR <br>
2. AST - abstract syntax tree<br>
3. Inheritance and polymorphism, abstract classes, interfaces<br>
4. Search for cyclic dependencies<br>
5. Data caching<br>
6. Error handling<br>
7. Unit testing<br>

# Using

## Before you start:
0. Installation and configuration of all required components in the development environment to run the application<br>
1. The use case and tests are shown in main.cpp .<br>

# Description of features:
## Cells
A table cell is defined by its index, that is, a row of the form “A1”, “C14” or “RD2". Moreover, the cell with the index “A1” is the cell in the upper left corner of the table. The number of rows and columns in the table does not exceed 16384. That is, the limit position of the cell is (16383, 16383) with the index “XFD16384". If the cell position goes beyond these boundaries, then it is not valid by definition.<br>
<br>
The **Position** structure in the common.h file. It contains the introw and int col fields — the row and column numbers of the cell in the program view. Used to access cells.<br>

## Indexes
The user has access to the cell by index, that is, by a row of the type “A1” or “RD2". Functions for conversion: `Position::FromString()` and `Position::ToString()`.<br>

## Minimum print area
To print a table, you need to know the size of the minimum printable area. This is a minimal rectangular area with a vertex in cell A1 containing all non-empty cells.<br>
The **Size** structure is defined in the common.h file. It contains the number of rows and columns in the minimum printable area.<br>

## Methods accessing a cell by index:
 - `SetCell(Position, std::string)` sets the contents of the cell by the **Position** index. If the cell is empty, you need to create it. You need to set the cell text using the `Cell::Set(std::string)` method;<br>
 - `Cell* GetCell(Position pos)` constant and non-constant getters that return a pointer to a cell located at the pos index. If the cell is empty, null ptr is returned;<br>
 - void `ClearCell(Position pos)` clears the cell by index. A subsequent call to `GetCell()` for this cell will return nullptr. At the same time, the size of the minimum print area may change.<br>

## Methods applicable to the entire table:
- `Size GetPrintableSize()` determines the size of the minimum printable area. The **Size** structure is defined specifically for it in the common.h file. It contains the number of rows and columns in the minimum printable area.<br>
 - Table printing outputs the minimum rectangular printed area to the standard output stream std::ostream&. Cells from one line are separated by a tab \t, a newline character \n is placed at the end of the line.<br>
 - `void PrintText(std::ostream&)` outputs text representations of cells:<br>
for text cells, this is the text that the user specified in the `Set()` method, that is, not cleared of leading apostrophes';<br>
for formula cells, this is a formula cleared of unnecessary parentheses, like `Formula::GetExpression()`, but with a leading sign “=".<br>
 - `void PrintValues(std::ostream&)` outputs cell values — strings, numbers, or **FormulaError** — as defined in `Cells::GetValue()`. <br>

## Calculating values in cells
Consider an example. The formula “=A3/A2” is written in cell C2. To calculate it, you need to divide the value of cell A3 by the value of cell A2.<br>
In cell A3 is the formula “=1+2*7 ”. It is easy to calculate: this is 15.<br>
In cell A2 is the text “3". Formally, the cell is not formulaic. But its text can be interpreted as a number.<br> Therefore, we assume that its value is 3.<br>
The result is 15/3=5.<br>
If the formula contains the index of an empty cell, we assume that the value of the empty cell is 0.<br>

## Possible errors and exceptions
### Calculation errors
Errors may occur in the calculations. For example, "division by 0". <br>
If the divisor is 0, the cell value is a **FormulaError** error of type **#DIV/0!**<br>

### Incorrect formula.
If the cell whose index is included in the formula cannot be interpreted as a number, the error **#VALUE** occurs!<br>
If in the cell by the method `Sheet::SetCell()` tries to write a syntactically incorrect formula, for example =A1+\*, the implementation throws a **FormulaException** exception, and the cell value does not change. A formula is considered syntactically incorrect if it does not satisfy the provided grammar. <br>

## Incorrect position.
The formula may contain a reference to a cell that goes beyond the limits of the possible size of the table, for example C2 (=A1234567+ZZZZ1). Such a formula can be created, but cannot be calculated, so its calculation will return an error **#REF!**<br>
Programmatically, it is possible to create an instance of the Position class with an incorrect position, for example (-1, -1). If the user passes it to methods, the program will throw an **InvalidPositionException** exception. Interface methods — for example `Cell::GetReferencedCells()` — always returns the correct positions.<br>

### Cyclic dependencies
The table must always remain correct. If the cells are cyclically dependent on each other, we will not be able to calculate the cell values. Therefore, cyclic dependencies between cells cannot be allowed to occur.<br>
If the user tries in the method `Sheet::SetCell()` write a formula to the cell that would lead to a cyclic dependency, the implementation throws a **CircularDependencyException** exception, and the value of the cell will not change.<br>

Errors propagate up the dependencies. For example: the formula in C4 depends on C2 (=C2+8). The formula in C2 gave an error calculating **#VALUE!** So, the formula in C4 will give the same error when calculating.<br>

The errors **#DIV/0!** and **#REF!** are distributed in the same way. If the formula depends on several cells, each of which contains a calculation error, the resulting error may correspond to any of them.<br>

## Calculation stages
The formula consists of operands and operations.<br>
What are the operations and operands:<br>
operations can be arithmetic (+, -, *, /), logical (&, |, ⇒, ~);< br>
operands can be numbers (1, 5, 1000.01), Boolean variables (TRUE, FALSE), constants (π, e, g, v).<br>
<br>
(abstract syntax tree, AST).<br>
For an abstract syntax tree, we can define node classes ourselves and add a method to them for calculation. Let's call it, for example, `GetValue()`.<br>
<br>
ANTLR is a special program that generates lexical and parser code, as well as code for traversing the parse tree in C++.<br>