#ifndef CANVASPRESENTER_H
#define CANVASPRESENTER_H

#include <QObject>
#include <QPixmap>
#include <QImage>
#include <QColor>
#include <QRect>

#include "interfaces/models/idocument.h"
#include "interfaces/presenters/icanvaspresenter.h"
#include "interfaces/presenters/ieditorpresenter.h"
#include "interfaces/presenters/ieditorviewportpresenter.h"

#include "utilities/initializehelper.h"

class CanvasPresenter : public QObject, public ICanvasPresenter
{
    Q_OBJECT

public:
    CanvasPresenter();
    virtual ~CanvasPresenter() = default;

    void initialize(IEditorPresenter* getEditorPresenter);

    IEditorPresenter* getEditorPresenter() { _initHelper.assertInitialized(); return _editorPresenter; }

    const QPixmap getRender();

    bool isEmpty();
    QSize getSize();
    QColor getBackgroundColor();

    bool hasDocument() { _initHelper.assertInitialized(); return (bool)_document; }
    IDocument* getDocument() { _initHelper.assertInitialized(); return _document; }
    void setDocument(IDocument* document);

signals:
    void boundaryChanged(QRect newBoundary);

    void applyingDrawingOperation(const IDrawingOperation& operation);
    void appliedDrawingOperation(const IDrawingOperation& operation);
private:
    const QColor DEFAULT_BACKGROUND_COLOR = Qt::GlobalColor::white;

    IEditorPresenter* _editorPresenter;

    bool _renderIsValid = false;

    IDocument* _document = nullptr;

    QPixmap _render;

protected:
    InitializeHelper _initHelper;
};

#endif //CANVASPRESENTER_H