#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>

// Реализуйте следующие методы
Cell::Cell(Sheet *sheet, const Position &position) :
		sheet_(sheet), position_(position) {
}

Cell::~Cell() {
}

void Cell::Set(std::string text) {
	if (text.empty()) {
		impl_ = std::make_unique<EmptyImpl>();
		impl_->Set(text);
	} else if (text[0] == '=' && text.size() > 1) {
		impl_ = std::make_unique<FormulaImpl>(*sheet_);
		impl_->Set(text);
	} else {
		impl_ = std::make_unique<TextImpl>();
		impl_->Set(text);
	}
}
void Cell::Clear() {
	impl_ = nullptr;
}
Cell::Value Cell::GetValue() const {
	if (impl_ != nullptr) {
		if (sheet_ != nullptr && sheet_->GetCash().count(position_)) {
			return sheet_->GetCash().at(position_);
		}
		sheet_->SetCashValue(position_, impl_->GetValue());
		return sheet_->GetCash().at(position_);
	}
	return "";
}
std::string Cell::GetText() const {
	if (impl_ != nullptr) {
		return impl_->GetText();
	}
	return "";
}

std::vector<Position> Cell::GetReferencedCells() const {
	if (impl_ != nullptr) {
		return impl_->GetReferencedCells();
	}
	return std::vector<Position> { };
}

void Cell::EmptyImpl::Set(std::string text) {
}
Cell::Value Cell::EmptyImpl::GetValue() const {
	return "";
}
std::string Cell::EmptyImpl::GetText() const {
	return "";
}
std::vector<Position> Cell::EmptyImpl::GetReferencedCells() const {
	return std::vector<Position> { };
}

void Cell::TextImpl::Set(std::string text) {
	text_val_ = text;
}
Cell::Value Cell::TextImpl::GetValue() const {
	if (text_val_[0] == '\'') {
		return text_val_.substr(1);
	}
	return text_val_;
}
std::string Cell::TextImpl::GetText() const {
	return text_val_;
}
std::vector<Position> Cell::TextImpl::GetReferencedCells() const {
	return std::vector<Position> { };
}

void Cell::FormulaImpl::Set(std::string text) {
	text_val_ = text;
	formula_ = ParseFormula(std::move(text.substr(1)));
}
Cell::Value Cell::FormulaImpl::GetValue() const {
	auto result = formula_->Evaluate(formula_sheet_);
	if (std::holds_alternative<double>(result)) {
		return std::get<double>(result);
	}
	return std::get<FormulaError>(result);
}
std::string Cell::FormulaImpl::GetText() const {
	return "=" + formula_->GetExpression();
}
std::vector<Position> Cell::FormulaImpl::GetReferencedCells() const {
	return formula_->GetReferencedCells();
}

