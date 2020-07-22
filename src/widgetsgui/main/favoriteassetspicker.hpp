#ifndef FAVORITEASSETSPICKER_HPP
#define FAVORITEASSETSPICKER_HPP

#include <QWidget>
#include <QToolButton>
#include <QList>
#include <QHash>
#include <QHBoxLayout>

#include "interfaces/presenters/tools/iassetselectionpresenter.hpp"
#include "interfaces/presenters/assets/iassetpresenter.hpp"

#include "utilities/presenter/presenterassignment.hpp"

class FavoriteAssetButton;
class FavoriteAssetsPicker : public QWidget 
{
    Q_OBJECT
public:
    FavoriteAssetsPicker(IAssetSelectionPresenter& presenter, QWidget* parent = nullptr);
    virtual ~FavoriteAssetsPicker() = default;

private slots:
    void onFavoritesChanged();
    void onSelectionChanged();
    void onButtonClicked(bool checked);

private:
    QHash<PersistentId, FavoriteAssetButton*> _buttons_byAsset;
    QList<FavoriteAssetButton*> _buttons;

    QHBoxLayout* _layout;

    IAssetSelectionPresenter& _presenter;
};

class FavoriteAssetButton : public QToolButton
{
    Q_OBJECT
public:
    FavoriteAssetButton(QWidget* parent = nullptr);
    virtual ~FavoriteAssetButton() = default;

    void setPresenter(PresenterAssignment<IAssetPresenter> presenter);
    PresenterAssignment<IAssetPresenter> presenter() const { return _presenter; }

private:
    PresenterAssignment<IAssetPresenter> _presenter;
};

#endif // FAVORITEASSETSPICKER_HPP