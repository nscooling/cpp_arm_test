#ifndef TRACE_H_
#define TRACE_H_

#ifdef TRACE_ENABLED
#include <iostream>

#define TRACE_MSG(msg) std::clog << "DEBUG: " << msg << std::endl
#define TRACE_VALUE(variable)                                                  \
  std::clog << "DEBUG: " << #variable << " : " << variable << std::endl

#else
#define TRACE_MSG(msg)
#define TRACE_VALUE(variable)

#endif

#endif