#ifndef DOCUMENTPRESENTER_HPP
#define DOCUMENTPRESENTER_HPP

#include <QList>
#include <QObject>
#include "interfaces/presenters/idocumentpresenter.hpp"
#include "utilities/initializehelper.hpp"

class DocumentPresenter : public QObject, public IDocumentPresenter
{
    Q_OBJECT
public:
    DocumentPresenter() : _initHelper(this) {}
    virtual ~DocumentPresenter() = default;

    void initialize(EmptyInitOptions option);
    void initialize(QSize size, QColor backgroundColor);
    void initialize(QWeakPointer<IDocument> model);

    QWeakPointer<IDocument> getModel() { _initHelper.check(); return _model; }
    bool isEmpty() { _initHelper.check(); return !_model; }

    QSize getSize() { _initHelper.check(); return _model ? _model.toStrongRef()->getSize() : QSize(); }
    QColor getBackgroundColor() { _initHelper.check(); return _model ? _model.toStrongRef()->getBackgroundColor() : QColor(); }

    QList<ILayerPresenter*> getLayers() { _initHelper.check(); return _layers; }

private:
    QSize _size;
    QColor _backgroundColor;

    QList<ILayerPresenter*> _layers;

    QWeakPointer<IDocument> _model;

    InitializeHelper<DocumentPresenter> _initHelper;
};

#endif // DOCUMENTPRESENTER_HPP