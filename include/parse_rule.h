#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include "rule_expression.h"
#include "expression_chain.h"

template <typename T>
class expression_chain;

template<typename T>
class rule_expression;

template <typename T>
class parse_tree;

template <typename T>
using function_t = std::function<bool(parse_tree<T>&, T&, std::string&)>;

template<typename T>
class parse_rule {
	std::string _name;
	std::vector<expression_chain<T>*> _expressions;
public:
	//TO-DO: default random string
	parse_rule(const std::string& name = "XXX") : _name(name) {}
	//TO-DO: delete this
	void print(std::ostream& o) {
		o << _name << " = ";
		for (size_t i = 0; i < _expressions.size(); ++i) {
			if (i)
				o << " | ";
			for (expression_chain<T>* iter = _expressions[i]; iter; iter = iter->next()) {
				if (iter != _expressions[i])
					o << " + ";

				if (!iter->always())
					o << "[";

				if (iter->is_terminal())
					o << "'";

				o << iter->to_string();

				if (iter->is_terminal())
					o << "'";

				if (!iter->always())
					o << "]";
			}
		}
		o << std::endl;
	}
	std::string name() {
		return _name;
	}
	size_t size() {
		return _expressions.size();
	}

	bool operator ==(const parse_rule& o) const {
		return this == &o;
	}
	expression_chain<T>* operator [](size_t i) {
		return _expressions[i];
	}
	parse_rule& operator =(parse_rule& rule) {
		for (auto x : _expressions)
			delete x;
		_expressions.clear();
		_expressions.push_back(new expression_chain<T>(&rule));
		return *this;
	}
	parse_rule& operator =(rule_expression<T>&& exp) {
		_expressions = exp.move_expressions();
		exp.clear();
		return *this;
	}
	rule_expression<T> operator ~() {
		return rule_expression<T>(*this, false);
	}
	rule_expression<T> operator [](function_t<T> function) {
		return rule_expression<T>(*this)[function];
	}
	rule_expression<T> operator +(parse_rule& rule) {
		return rule_expression<T>(*this, rule);
	}
	rule_expression<T>&& operator +(rule_expression<T>&& exp) {
		exp.put_ahead(*this);
		return std::move(exp);
	}
	rule_expression<T> operator |(parse_rule& rule) {
		return rule_expression<T>(*this) | rule;
	}
	rule_expression<T>&& operator |(rule_expression<T>&& exp) {
		exp.put_or_ahead(*this);
		return std::move(exp);
	}
	~parse_rule() {
		for (auto& x : _expressions)
			delete x;
	}

};
