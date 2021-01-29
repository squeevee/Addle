/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IBRUSH_HPP
#define IBRUSH_HPP

#include <QIcon>
#include <QList>

#include "idtypes/brushid.hpp"
#include "idtypes/brushengineid.hpp"

#include "interfaces/traits.hpp"

namespace Addle {

class BrushBuilder;
class IBrush
{
public:
    enum PreviewHint
    {
        Subtractive = 1
    };
    Q_DECLARE_FLAGS(PreviewHints, PreviewHint);

    virtual ~IBrush() = default;

    virtual void initialize(const BrushBuilder& builder) = 0;

    virtual BrushId id() const = 0;
    virtual BrushEngineId engineId() const = 0;

    virtual QIcon icon() const = 0;

    // Common engine parameters, like pressure and velocity dynamics, will be
    // given properties here.

    // virtual QVariantHash& customEngineParameters() = 0;
    virtual QVariantHash customEngineParameters() const = 0;

    virtual bool isSizeInvariant() const = 0;
    virtual bool isPixelAliased() const = 0;
    virtual bool eraserMode() const = 0;
    virtual bool copyMode() const = 0;

    virtual double minSize() const = 0;
    virtual double maxSize() const = 0;
    virtual QList<double> preferredSizes() const = 0;
    virtual bool strictSizing() const = 0;
    virtual double preferredStartingSize() const = 0;

    virtual PreviewHints previewHints() const = 0;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(IBrush::PreviewHints);

namespace aux_IBrush { using namespace config_detail::generic_id_parameter; }
ADDLE_DECL_SINGLETON_REPO_MEMBER(IBrush, BrushId);

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::IBrush, "org.addle.IBrush")

#endif // IBRUSH_HPP
