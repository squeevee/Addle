#pragma once

#ifdef ADDLE_DEBUG
# if defined(ADDLE_USE_LIBBACKTRACE)
#  define BOOST_STACKTRACE_USE_BACKTRACE
# endif
#include <boost/stacktrace/stacktrace.hpp>
#endif // ADDLE_DEBUG
