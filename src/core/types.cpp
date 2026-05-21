#include "qp/core/types.hpp"
#include <algorithm>
#include <cctype>
namespace qp {
static std::string normalize_symbol(std::string v){ v.erase(v.begin(), std::find_if(v.begin(), v.end(), [](unsigned char c){return !std::isspace(c);})); v.erase(std::find_if(v.rbegin(), v.rend(), [](unsigned char c){return !std::isspace(c);}).base(), v.end()); for(char& c:v) c=static_cast<char>(std::toupper(static_cast<unsigned char>(c))); return v; }
Symbol::Symbol(std::string v): value(normalize_symbol(std::move(v))) { if(value.empty()) throw std::invalid_argument("symbol cannot be empty"); }
Exchange::Exchange(std::string v): value(std::move(v)) { if(value.empty()) throw std::invalid_argument("exchange cannot be empty"); }
}
