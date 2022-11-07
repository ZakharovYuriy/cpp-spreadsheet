#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

using namespace std::literals;

Sheet::~Sheet() {
}

void Sheet::SetCell(Position pos, std::string text) {
	if (!pos.IsValid()) {
		throw InvalidPositionException("invalid position");
	}

	auto temp_cell = Cell(this, pos);
	temp_cell.Set(text);

	if (!CheckСircle(pos, temp_cell)) {
		throw CircularDependencyException("CircularDependencyException");
	}

	if (cash_.count(pos)) {
		ClearCell(pos);
	}

	if (PositionBiggerThenSize(pos) || IsEmpty()) {
		ChangeSize(ChooseTheLargestSizeParameter(pos));
	}

	cells_[pos] = Cell(this, pos);
	cells_[pos].Set(text);
	table_[pos.row][pos.col] = &cells_[pos];

	auto referenced_cells = cells_[pos].GetReferencedCells();
	dependent_cells_[pos] = referenced_cells; //от каких ячеек зависит текущая
	//изменение каких ячеек повлияет на текущую
	/*for (auto position : referenced_cells) {
	 cell_dependency_[position].push_back(pos);
	 }*/
}

const CellInterface* Sheet::GetCell(Position pos) const {
	if (!pos.IsValid()) {
		throw InvalidPositionException("invalid position");
	}
	if (PositionBiggerThenSize(pos) || IsEmpty()) {
		return nullptr;
	}
	return table_.at(pos.row).at(pos.col);
}

CellInterface* Sheet::GetCell(Position pos) {
	if (!pos.IsValid()) {
		throw InvalidPositionException("invalid position");
	}
	if (PositionBiggerThenSize(pos) || IsEmpty()) {
		return nullptr;
	}
	return table_.at(pos.row).at(pos.col);
}

void Sheet::ClearCell(Position pos) {
	if (!pos.IsValid()) {
		throw InvalidPositionException("invalid position");
	}

	if (PositionBiggerThenSize(pos) || IsEmpty()) {
		return;
	}

	if (table_.at(pos.row).at(pos.col) == nullptr) {
		return;
	}

	cells_.erase(pos);
	table_.at(pos.row).at(pos.col) = nullptr;

	for (auto pos_of_dependent_cell : dependent_cells_.at(pos)) {
		cash_.erase(pos_of_dependent_cell);
	}
	cash_.erase(pos);
	dependent_cells_.erase(pos);

	if (size_.rows - 1 == pos.row || size_.cols - 1 == pos.col) {
		int max_row = -1;
		int max_col = -1;

		for (int i = size_.rows - 1; i >= 0; --i) {
			for (int n = size_.cols - 1; n >= 0; --n) {
				if (table_.at(i).at(n) != nullptr) {
					if (max_row < i) {
						max_row = i;
					}
					if (max_col < n) {
						max_col = n;
					}
				}
			}
		}
		Size new_size;
		new_size.cols = max_col + 1;
		new_size.rows = max_row + 1;
		ChangeSize(new_size);
	}
}

Size Sheet::GetPrintableSize() const {
	return size_;
}

void Sheet::PrintValues(std::ostream &output) const {
	for (auto const &col : table_) {
		for (auto iter = col.begin(); iter < col.end(); ++iter) {
			auto const &cell = *iter;
			if (cell != nullptr) {
				visit(OstreamSolutionPrinter { output }, cell->GetValue());
			}
			if (iter != (col.end() - 1)) {
				output << '\t';
			}
		}
		output << '\n';
	}
}

void Sheet::PrintTexts(std::ostream &output) const {
	for (auto const &col : table_) {
		for (auto iter = col.begin(); iter < col.end(); ++iter) {
			auto const &cell = *iter;
			if (cell != nullptr) {
				output << cell->GetText();
			}
			if (iter != (col.end() - 1)) {
				output << '\t';
			}
		}
		output << '\n';
	}
}

const std::map<Position, CellInterface::Value>& Sheet::GetCash() const {
	return cash_;
}
void Sheet::SetCashValue(Position pos, const CellInterface::Value &val) {
	cash_[pos] = val;
}

bool Sheet::CheckСircle(Position pos, Cell &cel) {
	for (auto cells_in_formula : cel.GetReferencedCells()) {
		if (cells_in_formula == pos) {
			return false;
		}
		if (!RecurciveCheck(pos, cells_in_formula)) {
			return false;
		}
	}
	return true;
}
bool Sheet::RecurciveCheck(Position ref, Position cel_in_formula_pos) {
	if (dependent_cells_.count(cel_in_formula_pos)) {
		for (auto cells_d : dependent_cells_.at(cel_in_formula_pos)) {
			if (cells_d == ref) {
				return false;
			}
			if (!RecurciveCheck(ref, cells_d)) {
				return false;
			}
		}
	}
	return true;
}
std::unique_ptr<SheetInterface> CreateSheet() {
	return std::make_unique<Sheet>();
}
