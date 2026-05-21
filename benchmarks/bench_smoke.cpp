#include "qp/features/rolling_window.hpp"
#include <chrono>
#include <cstdlib>
#include <iostream>
int main(int argc, char** argv){ int iterations=100000; for(int i=1;i+1<argc;++i) if(std::string(argv[i])=="--iterations") iterations=std::atoi(argv[i+1]); qp::features::RollingWindow w(64); auto start=std::chrono::steady_clock::now(); double checksum=0; for(int i=0;i<iterations;++i){ w.push(static_cast<double>(i%97)); checksum += w.mean(); } auto end=std::chrono::steady_clock::now(); auto ns=std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count(); std::cout<<"benchmark=rolling_window\niterations="<<iterations<<"\nelapsed_ns="<<ns<<"\nns_per_op="<<(static_cast<double>(ns)/iterations)<<"\nchecksum="<<checksum<<"\n"; return checksum>=0 ? 0 : 1; }
