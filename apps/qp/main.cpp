#include "qp/data/bar.hpp"
#include "qp/features/returns.hpp"
#include <iostream>
#include <string>
int main(int argc, char** argv){ std::string cmd = argc>1 ? argv[1] : "smoke"; if(cmd=="smoke"){ qp::data::OhlcvBar b{qp::Symbol{"SPY"}, qp::Timestamp::from_unix_nanos(0), qp::Timestamp::from_unix_nanos(1), qp::data::BarInterval::Day1, qp::Price{100}, qp::Price{101}, qp::Price{99}, qp::Price{100.5}, qp::Quantity{1000}}; auto ok=b.validate(); if(!ok){ std::cerr<<ok.error().message<<"\n"; return 1; } std::cout<<"cpp-quant-platform " << QP_VERSION << " smoke ok\n"; return 0; } std::cerr<<"commands: smoke\n"; return 2; }
