#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>
#include <set>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << fe.ToString();
}

namespace {
class Formula : public FormulaInterface {
public:
// Реализуйте следующие методы:
    explicit Formula(std::string expression): ast_(CheckFormula(expression)) {
    }
    
    Value Evaluate(const SheetInterface& sheet) const override {
    	try {
    		return ast_.Execute(sheet);
		} catch (FormulaError& ex) {
			return ex;
		}
    }
    
    std::string GetExpression() const override {
    	std::stringstream ss;
    	ast_.PrintFormula(ss);
    	return ss.str();
    }

    std::vector<Position> GetReferencedCells() const override {
    	std::set<Position> unique_elements(ast_.GetCells().begin(), ast_.GetCells().end());
    	return std::vector<Position>(std::make_move_iterator(std::begin(unique_elements)),
            std::make_move_iterator(std::end(unique_elements)));
    }

private:
    FormulaAST ast_;

    FormulaAST CheckFormula(std::string expression){
    	try {
			return ParseFormulaAST(expression);
		} catch (...) {
			throw FormulaException(expression);
		}
    }
};
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    return std::make_unique<Formula>(std::move(expression));
}
