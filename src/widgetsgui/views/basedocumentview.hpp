#ifndef BASEDOCUMENTVIEW_HPP
#define BASEDOCUMENTVIEW_HPP

#include <QMainWindow>

#include "common/interfaces/presenters/idocumentpresenter.hpp"
#include "common/interfaces/presenters/iviewportpresenter.hpp"
#include "common/interfaces/views/idocumentview.hpp"

#include "common/utilities/initializehelper.hpp"

namespace Ui {
class DocumentView;
class ZoomRotateWidget;
}

class BaseDocumentView : public QMainWindow, public virtual IDocumentView
{
    Q_OBJECT

public:
    BaseDocumentView() : _initHelper(this)
    {
    }
    virtual ~BaseDocumentView();

    IDocumentPresenter* getDocumentPresenter() { _initHelper.assertInitialized(); return _presenter; }

public slots:
    void start();

protected:

    void initialize(IDocumentPresenter* presenter);
    virtual void setupUi();

    Ui::DocumentView* _ui;

private slots:
    void onViewPortScrollStateChanged();
    void onViewPortZoomChanged(double zoom);
    void onViewPortRotationChanged(double rotation);
    
    void onZoomSpinBoxValueChanged(double value);
    void onZoomSliderValueChanged(int value);

    void onVerticalViewPortScrollBarValueChanged(int value);
    void onHorizontalViewPortScrollBarValueChanged(int value);

    void onOpenFileAction();
    
    void onPresenterDocumentLoaded();
    void onPresenterErrorRaised(QSharedPointer<IErrorPresenter> error);

private:
    void updateZoomWidget();
    void updateViewPortScrollBars();

    Ui::ZoomRotateWidget* _zoomRotateWidgetUi;
    QWidget* _zoomRotateWidget;

    IDocumentPresenter* _presenter;
    IViewPortPresenter* _viewPortPresenter;

    InitializeHelper<BaseDocumentView> _initHelper;
};


#endif // BASEDOCUMENTVIEW_HPP
