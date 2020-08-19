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
    struct BrushBuilderData : QSharedData
    {
        BrushId id;
        BrushEngineId engine;
        QVariantHash customEngineParameters;

        QString name;
        QIcon icon;

        bool isSizeInvariant = false;
        bool isPixelAliased = false;

        bool eraserMode = false;
        bool copyMode = false;

        double minSize = 0;
        double maxSize = Q_INFINITY;

        QList<double> preferredSizes;
        bool strictSizing = false;
        double preferredStartingSize = Q_QNAN;

        IBrush::PreviewHints previewHints;
    };

public:

    BrushBuilder()
        : _data(new BrushBuilderData)
    {
    }

    BrushId id() const { return _data->id; }
    BrushBuilder& setId(BrushId id) { _data->id = id; return *this; }

    BrushEngineId engine() const { return _data->engine; }
    BrushBuilder& setEngine(BrushEngineId engine) { _data->engine = engine; return *this; }

    QVariantHash customEngineParameters() const { return _data->customEngineParameters; }
    BrushBuilder& setCustomEngineParameters(QVariantHash customEngineParameters) { _data->customEngineParameters = customEngineParameters; return *this; }

    QIcon icon() const { return _data->icon; }
    BrushBuilder& setIcon(QIcon icon) { _data->icon = icon; return *this; }

    bool isSizeInvariant() const { return _data->isSizeInvariant; }
    BrushBuilder& setSizeInvariant(bool isSizeInvariant) { _data->isSizeInvariant = isSizeInvariant; return *this; }

    bool isPixelAliased() const { return _data->isPixelAliased; }
    BrushBuilder& setPixelAliased(bool isPixelAliased) { _data->isPixelAliased = isPixelAliased; return *this; }

    bool eraserMode() const { return _data->eraserMode; }
    BrushBuilder& setEraserMode(bool eraserMode) { _data->eraserMode = eraserMode; return *this; }

    bool copyMode() const { return _data->copyMode; }
    BrushBuilder& setCopyMode(bool copyMode) { _data->copyMode = copyMode; return *this; }

    double minSize() const { return _data->minSize; }
    BrushBuilder& setMinSize(double minSize) { _data->minSize = minSize; return *this; }

    double maxSize() const { return _data->maxSize; }
    BrushBuilder& setMaxSize(double maxSize) { _data->maxSize = maxSize; return *this; }

    QList<double> preferredSizes() const { return _data->preferredSizes; }
    BrushBuilder& setPreferredSizes(QList<double> preferredSizes) { _data->preferredSizes = preferredSizes; return *this; }

    bool strictSizing() const { return _data->strictSizing; }
    BrushBuilder& setStrictSizing(double strictSizing) { _data->strictSizing = strictSizing; return *this; }

    double preferredStartingSize() const { return _data->preferredStartingSize; }
    BrushBuilder& setPreferredStartingSize(double preferredStartingSize) { _data->preferredStartingSize = preferredStartingSize; return *this; }

    IBrush::PreviewHints previewHints() const { return _data->previewHints; }
    BrushBuilder& setPreviewHints(IBrush::PreviewHints previewHints) { _data->previewHints = previewHints; return *this; }

private:
    QSharedDataPointer<BrushBuilderData> _data;
};

} // namespace Addle
#endif // BRUSHBUILDER_HPP