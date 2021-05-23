#include "globals.hpp"
//#include "utilities/configuration/serviceconfig.hpp"
#include "utilities/model/brushbuilder.hpp"
#include "utilities/model/palettebuilder.hpp"

#include "editing/brushengines/pathbrushengine.hpp"
#include "editing/brushengines/rasterbrushengine.hpp"

#include "editing/rasterdiff.hpp"
#include "editing/rastersurface.hpp"

#include "models/layer.hpp"
#include "models/layergroup.hpp"
#include "models/document.hpp"
#include "models/brush.hpp"
#include "models/palette.hpp"

#include "presenters/maineditorpresenter.hpp"
#include "presenters/documentpresenter.hpp"
#include "presenters/canvaspresenter.hpp"
#include "presenters/viewportpresenter.hpp"
#include "presenters/layerpresenter.hpp"
#include "presenters/layergrouppresenter.hpp"
#include "presenters/colorselectionpresenter.hpp"
#include "presenters/palettepresenter.hpp"
#include "presenters/messagecontext.hpp"
#include "presenters/assets/brushpresenter.hpp"
#include "presenters/tools/assetselectionpresenter.hpp"
#include "presenters/tools/navigatetoolpresenter.hpp"
#include "presenters/tools/brushtoolpresenter.hpp"
#include "presenters/tools/sizeselectionpresenter.hpp"
#include "presenters/operations/brushoperationpresenter.hpp"

#include "presenters/messages/notificationpresenter.hpp"
#include "presenters/messages/fileissuepresenter.hpp"

#include "rendering/renderer.hpp"

#include "services/applicationservice.hpp"
#include "services/errorservice.hpp"
#include "services/formatservice.hpp"

#include "format/qtimageformatdriver.hpp"

#include "utilities/view/viewrepository.hpp"
#include "interfaces/services/irepository.hpp"

#include "utilities/config/injectorconfig.hpp"

#include <QtDebug>

using namespace Addle;

std::unique_ptr<InjectorConfig> Addle::core_config()
{
    using namespace Config;
    auto config = std::make_unique<InjectorConfig>(
        bind<IApplicationService, ApplicationService>(),
        bind<IMainEditorPresenter, MainEditorPresenter>(),
        bind<ICanvasPresenter, CanvasPresenter>(),
        bind<IColorSelectionPresenter, ColorSelectionPresenter>(),
        bind<IViewPortPresenter, ViewPortPresenter>(),
        bind<IMessageContext, MessageContext>(),
        bind<IPalettePresenter, PalettePresenter>(),
                                                   
        bind<IDocumentPresenter, DocumentPresenter>(),
        bind<ILayerPresenter, LayerPresenter>(),
        bind<ILayerGroupPresenter, LayerGroupPresenter>(),
                                                   
        bind<IPalette, Palette>(),
        bind<IDocument, Document>(),
        bind<ILayer, Layer>(),
        bind<ILayerGroup, LayerGroup>(),
                                                   
        defer_conditional_bind<IToolPresenter>(),
        fill_conditional_bind<IToolPresenter, BrushToolPresenter>(
            filter_by_id(CoreTools::Brush)
        ),
        fill_conditional_bind<IToolPresenter, BrushToolPresenter>(
            filter_by_id(CoreTools::Eraser)
        ),
        fill_conditional_bind<IToolPresenter, NavigateToolPresenter>(
            filter_by_id(CoreTools::Navigate)
        ),
        
        bind<IRenderer, Renderer>(),
        
        defer_binding<IMainEditorView>()
    );
    
    auto&& paletteRepo = config->getSingleton<IRepository<IPalette>>();
    paletteRepo.addNew(
        aux_IPalette::id_ = CorePalettes::BasicPalette,
        aux_IPalette::builder_ = PaletteBuilder()
            .setSize(QSize(16, 3))
            .setColor(0,0,QColor::fromRgb(255,255,255),"White")
            .setColor(0,1,QColor::fromRgb(218,218,218),"Light Gray")
            .setColor(0,2,QColor::fromRgb(164,161,168),"Gray")
            .setColor(1,0,QColor::fromRgb(116,116,116),"Dark Gray")
            .setColor(1,1,QColor::fromRgb(74,74,74),"Charcoal")
            .setColor(1,2,QColor::fromRgb(0,0,0),"Black")
            .setColor(2,0,QColor::fromRgb(236,33,74),"Red")
            .setColor(2,1,QColor::fromRgb(174,46,71),"Crimson")
            .setColor(2,2,QColor::fromRgb(111,34,58),"Burgundy")
            .setColor(3,0,QColor::fromRgb(252,219,160),"Peach")
            .setColor(3,1,QColor::fromRgb(255,151,22),"Orange")
            .setColor(3,2,QColor::fromRgb(206,78,4),"Vermilion")
            .setColor(4,0,QColor::fromRgb(255,241,99),"Yellow")
            .setColor(4,1,QColor::fromRgb(252,205,31),"Goldenrod")
            .setColor(4,2,QColor::fromRgb(187,148,52),"Ocher")
            .setColor(5,0,QColor::fromRgb(174,255,62),"Lime")
            .setColor(5,1,QColor::fromRgb(131,197,47),"Grass")
            .setColor(5,2,QColor::fromRgb(113,137,71),"Olive")
            .setColor(6,0,QColor::fromRgb(41,231,88),"Green")
            .setColor(6,1,QColor::fromRgb(17,154,50),"Emerald")
            .setColor(6,2,QColor::fromRgb(43,78,41),"Moss")
            .setColor(7,0,QColor::fromRgb(116,252,206),"Aquamarine")
            .setColor(7,1,QColor::fromRgb(62,191,191),"Spruce")
            .setColor(7,2,QColor::fromRgb(59,118,128),"Teal")
            .setColor(8,0,QColor::fromRgb(199,252,254),"Turquoise")
            .setColor(8,1,QColor::fromRgb(1,229,255),"Cyan")
            .setColor(8,2,QColor::fromRgb(2,157,255),"Cerulean")
            .setColor(9,0,QColor::fromRgb(157,189,255),"Cornflower")
            .setColor(9,1,QColor::fromRgb(0,91,246),"Blue")
            .setColor(9,2,QColor::fromRgb(0,55,149),"Indigo")
            .setColor(10,0,QColor::fromRgb(214,208,255),"Periwinkle")
            .setColor(10,1,QColor::fromRgb(202,138,255),"Lavender")
            .setColor(10,2,QColor::fromRgb(99,59,189),"Violet")
            .setColor(11,0,QColor::fromRgb(255,154,213),"Pink")
            .setColor(11,1,QColor::fromRgb(222,76,132),"Rose")
            .setColor(11,2,QColor::fromRgb(100,34,103),"Purple")
            .setColor(12,0,QColor::fromRgb(226,198,143),"Tan")
            .setColor(12,1,QColor::fromRgb(252,212,190),"Apricot")
            .setColor(12,2,QColor::fromRgb(227,227,211),"Alabaster")
            .setColor(13,0,QColor::fromRgb(197,127,38),"Caramel")
            .setColor(13,1,QColor::fromRgb(217,160,136),"Satin")
            .setColor(13,2,QColor::fromRgb(144,144,124),"Ash")
            .setColor(14,0,QColor::fromRgb(116,55,32),"Sienna")
            .setColor(14,1,QColor::fromRgb(196,108,72),"Bronze")
            .setColor(14,2,QColor::fromRgb(200,190,211),"Marble")
            .setColor(15,0,QColor::fromRgb(80,55,43),"Umber")
            .setColor(15,1,QColor::fromRgb(132,91,75),"Brown")
            .setColor(15,2,QColor::fromRgb(114,108,131),"Slate")
    );
    
    return config;
}


// boost::di::injector<IApplicationService&, IFactory<IMainEditorPresenter>&> config_core ()
// {   
//     return boost::di::make_injector(
//         boost::di::bind<IApplicationService>().to<ApplicationService>(),
//         boost::di::bind<IFactory<IMainEditorPresenter>>().to(
//             Addle::Config::FactoryBinding<MainEditorPresenter>()
//         )
//     );
// }

// namespace Addle { namespace Config {
//     
// template<>
// BindingSet* get_module_binding<STATIC_ID_WRAPPER(Modules::Core)>()
// {
//     auto config = new BindingSet();
//     
//     config->bind<IApplicationService, ApplicationService>();
//     config->bind<IMainEditorPresenter, MainEditorPresenter>();
//     
//     return config;
// }
// 
// }}

// extern "C" void addle_core_config(Config::ServiceContainer& container)
// {
//     auto coreBindings = new config_detail::BindingSet();
//     
//     coreBindings->bind<IApplicationService, ApplicationService>();
//     coreBindings->bind<IMainEditorPresenter, MainEditorPresenter>();
//     
//     container.addBindings(coreBindings);
//     
//     auto config = new CoreConfig(container, Modules::Core);
//     
//     config->addFactory<IApplicationService, ApplicationService>();
//     config->addFactory<IFormatService, FormatService>();
//     config->addFactory<IErrorService, ErrorService>();
//     
//     config->addFactory<ICanvasPresenter, CanvasPresenter>();
//     config->addFactory<IMainEditorPresenter, MainEditorPresenter>();
//     config->addFactory<IColorSelectionPresenter, ColorSelectionPresenter>();
//     
//     config->addFactory<IViewPortPresenter, ViewPortPresenter>();
//     config->addFactory<IMessageContext, MessageContext>();
//     config->addFactory<IPalettePresenter, PalettePresenter>();
//     
//     config->commit();

//     auto config = new CoreConfig(Modules::Core);
//     
//     # Editing    
//     config->addAutoFactory<IBrushEngine, PathBrushEngine>(CoreBrushEngines::PathEngine);
//     config->addAutoFactory<IBrushEngine, RasterBrushEngine>(CoreBrushEngines::RasterEngine);
//     
//     config->addAutoFactory<IRasterDiff, RasterDiff>();
//     config->addAutoFactory<IRasterSurface, RasterSurface>();
//     
//     # Format
//     config->addAutoFactory<IDocumentFormatDriver, QtImageFormatDriver>(
//         CoreConfig::Filter()
//             .byId<DocumentFormatId>(&QtImageFormatDriver::idIsSupported)
//             .disallowNullIds()
//     );
//     
//     # Models
//     config->addAutoFactory<ILayer, Layer>();
//     config->addAutoFactory<IDocument, Document>();
//     
//     config->addAutoFactory<IBrush, Brush>();
//     config->setupPersistentObject<IBrush>(CoreBrushes::BasicBrush,  
//         BrushBuilder()
//             .setEngine(CoreBrushEngines::PathEngine)
//             .setPreferredStartingSize(7.0)
//     );
//     config->setupPersistentObject<IBrush>(CoreBrushes::SoftBrush,
//         BrushBuilder()
//             .setEngine(CoreBrushEngines::RasterEngine)
//             .setCustomEngineParameters({
//                 { "mode", "Gradient" },
//                 { "hardness", 1.0 },
//                 { "spacing", 0.12 }
//             })
//             .setMinSize(21.0)
//             .setPreferredStartingSize(60.0)
//     );
//     config->setupPersistentObject<IBrush>(CoreBrushes::BasicEraser, 
//         BrushBuilder()
//             .setEngine(CoreBrushEngines::PathEngine)
//             .setPreviewHints(IBrush::Subtractive)
//             .setEraserMode(true)
//             .setPreferredStartingSize(21.0)
//     );
//     
//     config->addAutoFactory<IPalette, Palette>();
//     config->setupPersistentObject<IPalette>(CorePalettes::BasicPalette, 
//         TODO: i18n
//         PaletteBuilder()
//             .setSize(QSize(16, 3))
//             .setColor(0,0,QColor::fromRgb(255,255,255),"White")
//             .setColor(0,1,QColor::fromRgb(218,218,218),"Light Gray")
//             .setColor(0,2,QColor::fromRgb(164,161,168),"Gray")
//             .setColor(1,0,QColor::fromRgb(116,116,116),"Dark Gray")
//             .setColor(1,1,QColor::fromRgb(74,74,74),"Charcoal")
//             .setColor(1,2,QColor::fromRgb(0,0,0),"Black")
//             .setColor(2,0,QColor::fromRgb(236,33,74),"Red")
//             .setColor(2,1,QColor::fromRgb(174,46,71),"Crimson")
//             .setColor(2,2,QColor::fromRgb(111,34,58),"Burgundy")
//             .setColor(3,0,QColor::fromRgb(252,219,160),"Peach")
//             .setColor(3,1,QColor::fromRgb(255,151,22),"Orange")
//             .setColor(3,2,QColor::fromRgb(206,78,4),"Vermilion")
//             .setColor(4,0,QColor::fromRgb(255,241,99),"Yellow")
//             .setColor(4,1,QColor::fromRgb(252,205,31),"Goldenrod")
//             .setColor(4,2,QColor::fromRgb(187,148,52),"Ocher")
//             .setColor(5,0,QColor::fromRgb(174,255,62),"Lime")
//             .setColor(5,1,QColor::fromRgb(131,197,47),"Grass")
//             .setColor(5,2,QColor::fromRgb(113,137,71),"Olive")
//             .setColor(6,0,QColor::fromRgb(41,231,88),"Green")
//             .setColor(6,1,QColor::fromRgb(17,154,50),"Emerald")
//             .setColor(6,2,QColor::fromRgb(43,78,41),"Moss")
//             .setColor(7,0,QColor::fromRgb(116,252,206),"Aquamarine")
//             .setColor(7,1,QColor::fromRgb(62,191,191),"Spruce")
//             .setColor(7,2,QColor::fromRgb(59,118,128),"Teal")
//             .setColor(8,0,QColor::fromRgb(199,252,254),"Turquoise")
//             .setColor(8,1,QColor::fromRgb(1,229,255),"Cyan")
//             .setColor(8,2,QColor::fromRgb(2,157,255),"Cerulean")
//             .setColor(9,0,QColor::fromRgb(157,189,255),"Cornflower")
//             .setColor(9,1,QColor::fromRgb(0,91,246),"Blue")
//             .setColor(9,2,QColor::fromRgb(0,55,149),"Indigo")
//             .setColor(10,0,QColor::fromRgb(214,208,255),"Periwinkle")
//             .setColor(10,1,QColor::fromRgb(202,138,255),"Lavender")
//             .setColor(10,2,QColor::fromRgb(99,59,189),"Violet")
//             .setColor(11,0,QColor::fromRgb(255,154,213),"Pink")
//             .setColor(11,1,QColor::fromRgb(222,76,132),"Rose")
//             .setColor(11,2,QColor::fromRgb(100,34,103),"Purple")
//             .setColor(12,0,QColor::fromRgb(226,198,143),"Tan")
//             .setColor(12,1,QColor::fromRgb(252,212,190),"Apricot")
//             .setColor(12,2,QColor::fromRgb(227,227,211),"Alabaster")
//             .setColor(13,0,QColor::fromRgb(197,127,38),"Caramel")
//             .setColor(13,1,QColor::fromRgb(217,160,136),"Satin")
//             .setColor(13,2,QColor::fromRgb(144,144,124),"Ash")
//             .setColor(14,0,QColor::fromRgb(116,55,32),"Sienna")
//             .setColor(14,1,QColor::fromRgb(196,108,72),"Bronze")
//             .setColor(14,2,QColor::fromRgb(200,190,211),"Marble")
//             .setColor(15,0,QColor::fromRgb(80,55,43),"Umber")
//             .setColor(15,1,QColor::fromRgb(132,91,75),"Brown")
//             .setColor(15,2,QColor::fromRgb(114,108,131),"Slate")
//     );
//     
//     # Presenters
//     config->addAutoFactory<IMainEditorPresenter, MainEditorPresenter>();
//     config->addAutoFactory<ICanvasPresenter, CanvasPresenter>();
//     config->addAutoFactory<IColorSelectionPresenter, ColorSelectionPresenter>();
//     config->addAutoFactory<IViewPortPresenter, ViewPortPresenter>();
//     config->addAutoFactory<IMessageContext, MessageContext>();
//     config->addAutoFactory<ILayerPresenter, LayerPresenter>();
//     config->addAutoFactory<IDocumentPresenter, DocumentPresenter>();
//     config->addAutoFactory<IBrushOperationPresenter, BrushOperationPresenter>();
//     config->addAutoFactory<IPalettePresenter, PalettePresenter>();
// 
//     ## Asset presenters
//     config->addAutoFactory<IBrushPresenter, BrushPresenter>();
//     
//     ## Message presenters
//     config->addAutoFactory<INotificationPresenter, NotificationPresenter>();
//     config->addAutoFactory<IFileIssuePresenter, FileIssuePresenter>();
// 
//     ## Tool presenters
//     config->addAutoFactory<IAssetSelectionPresenter, AssetSelectionPresenter>();
//     config->addAutoFactory<INavigateToolPresenter, NavigateToolPresenter>();
//     config->addAutoFactory<IBrushToolPresenter, BrushToolPresenter>();
//     config->addAutoFactory<ISizeSelectionPresenter, SizeSelectionPresenter>();
// 
//     # Rendering
//     config->addAutoFactory<IRenderer, Renderer>();
// 
//     # Services
//     config->addAutoFactory<IAppearanceService, AppearanceService>();
//     config->addAutoFactory<IApplicationService, ApplicationService>();
//     config->addAutoFactory<IErrorService, ErrorService>();
//     config->addAutoFactory<IFormatService, FormatService>();
//     
//     config->commit();
// }
