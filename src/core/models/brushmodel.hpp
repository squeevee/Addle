#ifndef BRUSHMODEL_HPP
#define BRUSHMODEL_HPP

#include "compat.hpp"

#include "interfaces/models/ibrushmodel.hpp"
#include "utilities/initializehelper.hpp"
namespace Addle {
class ADDLE_CORE_EXPORT BrushModel : public IBrushModel
{
public:

    void initialize(const BrushBuilder& builder);

    BrushId id() const { _initHelper.check(); return _id; }
    BrushEngineId engineId() const { _initHelper.check(); return _engineId; }
    QVariantHash customEngineParameters() const { _initHelper.check(); return _customEngineParameters; }

    QIcon icon() const { _initHelper.check(); return _icon; }

    //BrushInfo info() const { _initHelper.check(); return _info; }

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

    InitializeHelper<BrushModel> _initHelper;
};

} // namespace Addle
#endif // BRUSHMODEL_HPP