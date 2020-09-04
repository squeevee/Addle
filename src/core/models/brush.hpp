/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef BRUSH_HPP
#define BRUSH_HPP

#include "compat.hpp"

#include "interfaces/models/ibrush.hpp"
#include "utilities/initializehelper.hpp"
namespace Addle {
class ADDLE_CORE_EXPORT Brush : public IBrush
{
public:

    void initialize(const BrushBuilder& builder);

    BrushId id() const { ASSERT_INIT(); return _id; }
    BrushEngineId engineId() const { ASSERT_INIT(); return _engineId; }
    QVariantHash customEngineParameters() const { ASSERT_INIT(); return _customEngineParameters; }

    QIcon icon() const { ASSERT_INIT(); return _icon; }

    //BrushInfo info() const { ASSERT_INIT(); return _info; }

    bool isSizeInvariant() const { return _isSizeInvariant; }
    bool isPixelAliased() const { return _isPixelAliased; }
    bool eraserMode() const { return _eraserMode; }
    bool copyMode() const { return _copyMode; }

    double minSize() const { return _minSize; }
    double maxSize() const { return _maxSize; }
    QList<double> preferredSizes() const { return _preferredSizes; }
    bool strictSizing() const { return _strictSizing; }
    double preferredStartingSize() const { return _preferredStartingSize; }

    PreviewHints previewHints() const { return _previewHints; }

private:
    BrushId _id;
    BrushEngineId _engineId;
    //BrushInfo _info;
    QVariantHash _customEngineParameters;

    QIcon _icon; 

    bool _isSizeInvariant;
    bool _isPixelAliased;
    bool _eraserMode;
    bool _copyMode;

    double _minSize;
    double _maxSize;
    QList<double> _preferredSizes;
    bool _strictSizing;
    double _preferredStartingSize;

    PreviewHints _previewHints;

    InitializeHelper _initHelper;
};

} // namespace Addle
#endif // BRUSH_HPP