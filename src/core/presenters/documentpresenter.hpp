#ifndef DOCUMENTPRESENTER_HPP
#define DOCUMENTPRESENTER_HPP

#include <QList>
#include <QObject>
#include "interfaces/presenters/idocumentpresenter.hpp"
#include "utilities/initializehelper.hpp"
#include "core/compat.hpp"

class ADDLE_CORE_EXPORT DocumentPresenter : public QObject, public IDocumentPresenter
{
    Q_OBJECT
public:
    DocumentPresenter() : _initHelper(this) {}
    virtual ~DocumentPresenter() = default;

    void initialize(EmptyInitOptions option);
    void initialize(QSize size, QColor backgroundColor);
    void initialize(QSharedPointer<IDocument> model);

    QSharedPointer<IDocument> getModel() { _initHelper.check(); return _model; }
    bool isEmpty() { _initHelper.check(); return !_model; }

    QSize getSize() { _initHelper.check(); return _model ? _model->getSize() : QSize(); }
    QRect getRect() { return QRect(QPoint(), getSize()); }
    QColor getBackgroundColor() { _initHelper.check(); return _model ? _model->getBackgroundColor() : QColor(); }

    QList<QSharedPointer<ILayerPresenter>> getLayers() { _initHelper.check(); return _layers; }

private:
    QSize _size;
    QColor _backgroundColor;

    QList<QSharedPointer<ILayerPresenter>> _layers;

    QSharedPointer<IDocument> _model;

    InitializeHelper<DocumentPresenter> _initHelper;
};

#endif // DOCUMENTPRESENTER_HPP