#include "collatingstring.hpp"
#include "errors.hpp"

using namespace Addle;

int CollatingString::compare(const CollatingString& a, const CollatingString& b)
{
    ADDLE_ASSERT_M(
        a._collator.locale()                == b._collator.locale()
        && a._collator.caseSensitivity()    == b._collator.caseSensitivity()
        && a._collator.ignorePunctuation()  == b._collator.ignorePunctuation()
        && a._collator.numericMode()        == b._collator.numericMode(), 
        //% "Comparison on non-equivalent collators is not allowed."
        qUtf8Printable(qtTrId("debug-messages.non-equivalent-collators"))
    );
    return a._collator.compare(a._string, b._string);
}
