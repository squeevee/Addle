/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "brush.hpp"
#include "utils.hpp"

#include "utilities/model/brushbuilder.hpp"
using namespace Addle;

void Brush::initialize(const BrushBuilder& builder)
{
    const Initializer init(_initHelper);

//     ADDLE_ASSERT(!builder.id() || _id == builder.id());
//     _engineId = builder.engine();
    _customEngineParameters = builder.customEngineParameters();

    _icon = builder.icon();

    _isSizeInvariant = builder.isSizeInvariant();
    _isPixelAliased = builder.isPixelAliased();
    _eraserMode = builder.eraserMode();
    _copyMode = builder.copyMode() || _eraserMode;

    _minSize = builder.minSize();
    _maxSize = builder.maxSize();
    _preferredSizes = builder.preferredSizes();
    _strictSizing = builder.strictSizing();

    _preferredStartingSize = builder.preferredStartingSize();

    _previewHints = builder.previewHints();
}
