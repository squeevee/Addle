#ifndef UTILS_HPP
#define UTILS_HPP

// A variety of commonly used and generic utilities in one file
// This is meant as a convenience to reduce a lot of exhaustive include directives,
// but should by no means include *all* utilities. By virtue of being so widely
// included, this is liable to cause unnecessary recompiles if it or any of its
// dependencies are changed too frequently. Do not include this file in public
// interfaces, and do not include interconnected public interfaces in this file.

#include "servicelocator.hpp"

#include "utilities/addle_icon.hpp"
#include "utilities/addle_text.hpp"

#include "utilities/collections.hpp"
#include "utilities/math.hpp"

#include "utilities/qtextensions/qmeta.hpp"
#include "utilities/qtextensions/qobject.hpp"

#ifdef ADDLE_DEBUG
#include "utilities/debugging.hpp"
#endif

#endif // UTILS_HPP