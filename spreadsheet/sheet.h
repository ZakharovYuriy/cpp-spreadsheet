#pragma once

#include "cell.h"
#include "common.h"

#include <functional>
#include <vector>
#include <map>
#include <iostream>

class Cell;

class Sheet: public SheetInterface {
public:
	~Sheet();

	void SetCell(Position pos, std::string text) override;

	const CellInterface* GetCell(Position pos) const override;
	CellInterface* GetCell(Position pos) override;

	void ClearCell(Position pos) override;

	Size GetPrintableSize() const override;

	void PrintValues(std::ostream &output) const override;
	void PrintTexts(std::ostream &output) const override;

	// Можете дополнить ваш класс нужными полями и методами
	const std::map<Position, CellInterface::Value>& GetCash() const;
	void SetCashValue(Position, const CellInterface::Value&);

private:
	// Можете дополнить ваш класс нужными полями и методами
	Size size_;
	std::map<Position, Cell> cells_;
	std::vector<std::vector<Cell*>> table_;

	//std::map<Position, std::vector<Position>> cell_dependency_; //хранит информацию о том, какие ячейки необходимы для вычисления текущей.
	std::map<Position, std::vector<Position>> dependent_cells_; //показывает какие ячейки зависят от изменения текущей
	std::map<Position, CellInterface::Value> cash_;

	void ChangeSize(Size new_size) {
		if (new_size == Size { 0, 0 }) {
			table_.clear();
			size_ = new_size;
			return;
		}
		ChangeColsSize(new_size.cols);
		ChangeRowsSize(new_size.rows);
	}
	//строки
	void ChangeColsSize(int new_col_size) {
		for (auto &col : table_) {
			col.resize(new_col_size);
		}
		size_.cols = new_col_size;
	}
	//столбцы
	void ChangeRowsSize(int new_raw_size) {
		std::vector<Cell*> row(size_.cols);
		table_.resize(new_raw_size, row);
		size_.rows = new_raw_size;
	}

	bool PositionBiggerThenSize(Position pos) const {
		return size_.rows - 1 < pos.row || size_.cols - 1 < pos.col;
	}

	bool IsEmpty() const {
		return size_ == Size { 0, 0 };
	}

	Size ChooseTheLargestSizeParameter(Position pos) {
		Size new_size;
		pos.col + 1 > size_.cols ? new_size.cols = pos.col + 1 : new_size.cols =
											size_.cols;
		pos.row + 1 > size_.rows ? new_size.rows = pos.row + 1 : new_size.rows =
											size_.rows;
		return new_size;
	}

//std::string, double, FormulaError
	struct OstreamSolutionPrinter {
		std::ostream &out;

		void operator()(std::string str) const {
			out << str;
		}
		void operator()(double val) const {
			out << val;
		}
		void operator()(FormulaError er) const {
			out << er;
		}
	};

	bool CheckСircle(Position pos, Cell &cel);

	bool RecurciveCheck(Position ref, Position pos);
};
