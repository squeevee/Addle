#ifndef CANVASPRESENTER_HPP
#define CANVASPRESENTER_HPP

#include <QObject>
#include <QPixmap>
#include <QImage>
#include <QColor>
#include <QRect>
#include <QSharedPointer>

#include "common/interfaces/models/idocument.hpp"
#include "common/interfaces/presenters/icanvaspresenter.hpp"
#include "common/interfaces/presenters/ieditorpresenter.hpp"
#include "common/interfaces/views/icanvasview.hpp"
#include "common/utilities/initializehelper.hpp"

class CanvasPresenter : public QObject, public virtual ICanvasPresenter
{
    Q_OBJECT

public:
    CanvasPresenter() : _initHelper(this)
    {
    }
    virtual ~CanvasPresenter() = default;

    void initialize(IDocumentPresenter* documentPresenter);

    IDocumentPresenter* getDocumentPresenter();
    virtual ICanvasView* getView();

    const QPixmap getRender();

    bool isEmpty();
    QSize getSize();
    QColor getBackgroundColor();

private slots:
    void onDocumentChanged(QSharedPointer<IDocument> document);

signals:
    void boundaryChanged(QRect newBoundary);

    //void applyingDrawingOperation(const IDrawingOperation& operation);
    //void appliedDrawingOperation(const IDrawingOperation& operation);

private:
    const QColor DEFAULT_BACKGROUND_COLOR = Qt::GlobalColor::white;

    IDocumentPresenter* _documentPresenter;

    bool _renderIsValid = false;

    QSharedPointer<IDocument> _document = nullptr;

    QPixmap _render;

    ICanvasView* _view = nullptr;

    InitializeHelper<CanvasPresenter> _initHelper;
};

#endif // CANVASPRESENTER_HPP