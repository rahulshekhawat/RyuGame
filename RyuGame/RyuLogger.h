#pragma once

#include <string>
#include <iostream>

//~ @todo
#define rlog(logText) std::cout<<"[LOG]: "<<logText<<"\n";
#define rlog_warn(logText) std::cout<<"[WARNING]: "<<logText<<"\n";
#define rlog_error(logText) std::cout<<"[ERROR]: "<<logText<<"\n";

#define renginelog
#define rfilelog

#define rdebuglog
