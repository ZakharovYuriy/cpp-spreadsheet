#pragma once

#include "common.h"
#include "formula.h"
#include "sheet.h"

#include <functional>
#include <unordered_set>

class Sheet;

class Cell: public CellInterface {
public:
	Cell(Sheet *sheet, const Position &position);
	Cell() = default;
	~Cell();

	void Set(std::string text);
	void Clear();

	Value GetValue() const override;
	std::string GetText() const override;
	std::vector<Position> GetReferencedCells() const override;

	bool IsReferenced() const;

	Cell& operator=(const Cell &rhs) {
		sheet_ = rhs.sheet_;
		impl_ = std::move(impl_);
		position_ = rhs.position_;
		return *this;
	}

private:
	// Добавьте поля и методы для связи с таблицей, проверки циклических
	// зависимостей, графа зависимостей и т. д.
	Sheet *sheet_ = nullptr;
	Position position_;

	class Impl {
	public:
		virtual ~Impl() = default;
		virtual void Set(std::string text) = 0;
		virtual Value GetValue() const = 0;
		virtual std::string GetText() const = 0;
		virtual std::vector<Position> GetReferencedCells() const = 0;
	};

	class EmptyImpl final : public Impl {
	public:
		EmptyImpl() = default;
		~EmptyImpl() = default;
		void Set(std::string text);
		Value GetValue() const;
		std::string GetText() const;
		std::vector<Position> GetReferencedCells() const;
	};

	class TextImpl final : public Impl {
	public:
		TextImpl() = default;
		~TextImpl() = default;
		void Set(std::string text);
		Value GetValue() const;
		std::string GetText() const;
		std::vector<Position> GetReferencedCells() const;
	private:
		std::string text_val_;
	};

	class FormulaImpl final : public Impl {
	public:
		FormulaImpl(Sheet &sheet) :
				formula_sheet_(sheet) {
		}
		;
		~FormulaImpl() = default;
		void Set(std::string text);
		Value GetValue() const;
		std::string GetText() const;
		std::vector<Position> GetReferencedCells() const;
	private:
		Sheet &formula_sheet_;
		std::string text_val_;
		std::unique_ptr<FormulaInterface> formula_;
	};

	std::unique_ptr<Impl> impl_;
};
