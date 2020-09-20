/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef BRUSHBUILDER_HPP
#define BRUSHBUILDER_HPP

#include <QMetaType>
#include <QSharedData>
#include <QIcon>

#include "interfaces/models/ibrush.hpp"
#include "idtypes/brushid.hpp"
#include "idtypes/brushengineid.hpp"
namespace Addle {

class BrushBuilder
{
public:
    ~BrushBuilder() = default;
    
    BrushId id() const { return _id; }
    BrushBuilder& setId(BrushId id) { _id = id; return *this; }

    BrushEngineId engine() const { return _engine; }
    BrushBuilder& setEngine(BrushEngineId engine) { _engine = engine; return *this; }

    QVariantHash customEngineParameters() const { return _customEngineParameters; }
    BrushBuilder& setCustomEngineParameters(QVariantHash customEngineParameters) { _customEngineParameters = customEngineParameters; return *this; }

    QIcon icon() const { return _icon; }
    BrushBuilder& setIcon(QIcon icon) { _icon = icon; return *this; }

    bool isSizeInvariant() const { return _isSizeInvariant; }
    BrushBuilder& setSizeInvariant(bool isSizeInvariant) { _isSizeInvariant = isSizeInvariant; return *this; }

    bool isPixelAliased() const { return _isPixelAliased; }
    BrushBuilder& setPixelAliased(bool isPixelAliased) { _isPixelAliased = isPixelAliased; return *this; }

    bool eraserMode() const { return _eraserMode; }
    BrushBuilder& setEraserMode(bool eraserMode) { _eraserMode = eraserMode; return *this; }

    bool copyMode() const { return _copyMode; }
    BrushBuilder& setCopyMode(bool copyMode) { _copyMode = copyMode; return *this; }

    double minSize() const { return _minSize; }
    BrushBuilder& setMinSize(double minSize) { _minSize = minSize; return *this; }

    double maxSize() const { return _maxSize; }
    BrushBuilder& setMaxSize(double maxSize) { _maxSize = maxSize; return *this; }

    QList<double> preferredSizes() const { return _preferredSizes; }
    BrushBuilder& setPreferredSizes(QList<double> preferredSizes) { _preferredSizes = preferredSizes; return *this; }

    bool strictSizing() const { return _strictSizing; }
    BrushBuilder& setStrictSizing(double strictSizing) { _strictSizing = strictSizing; return *this; }

    double preferredStartingSize() const { return _preferredStartingSize; }
    BrushBuilder& setPreferredStartingSize(double preferredStartingSize) { _preferredStartingSize = preferredStartingSize; return *this; }

    IBrush::PreviewHints previewHints() const { return _previewHints; }
    BrushBuilder& setPreviewHints(IBrush::PreviewHints previewHints) { _previewHints = previewHints; return *this; }

private:
    BrushId _id;
    BrushEngineId _engine;
    QVariantHash _customEngineParameters;

    QString _name;
    QIcon _icon;

    bool _isSizeInvariant = false;
    bool _isPixelAliased = false;

    bool _eraserMode = false;
    bool _copyMode = false;

    double _minSize = 0;
    double _maxSize = Q_INFINITY;

    QList<double> _preferredSizes;
    bool _strictSizing = false;
    double _preferredStartingSize = Q_QNAN;

    IBrush::PreviewHints _previewHints;
};

} // namespace Addle
#endif // BRUSHBUILDER_HPP
