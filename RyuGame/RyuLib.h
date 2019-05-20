#pragma once

#include <string>
#include <iostream>

#define RYU_API

#ifdef _DEBUG

#define rlog(logText) std::cout<<"[LOG] : "<<logText;
#define rlog_warn(logText) std::cout<<"[WARNING] : "<<logText;
#define rlog_err(logText) std::cout<<"[ERROR] : "<<logText;

#elif NDEBUG

#define rlog(logText)
#define rlog_warn(logText)
#define rlog_err(logText)

#endif // _DEBUG



