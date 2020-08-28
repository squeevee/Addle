/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include <QSignalBlocker>

#include "favoriteassetspicker.hpp"
#include "interfaces/presenters/tools/iassetselectionpresenter.hpp"

#include "utilities/qobject.hpp"

using namespace Addle;

FavoriteAssetsPicker::FavoriteAssetsPicker(IAssetSelectionPresenter& presenter, QWidget* parent)
    : QWidget(parent), _presenter(presenter)
{
    _layout = new QHBoxLayout(this);

    connect_interface(
        &_presenter,
        SIGNAL(favoritesChanged(QList<AddleId>)),
        this,
        SLOT(onFavoritesChanged())
    );

    connect_interface(
        &_presenter,
        SIGNAL(selectionChanged(QList<AddleId>)),
        this,
        SLOT(onSelectionChanged())
    );

    onFavoritesChanged();
    onSelectionChanged();
}

void FavoriteAssetsPicker::onFavoritesChanged()
{
    const int c = _buttons.count();

    for(int i = _presenter.favorites().count(); i < c; ++i)
    {
        //more buttons than favorites
        delete _buttons[i];
        _buttons.removeLast();
    }
    for(int i = c; i < _presenter.favorites().count(); ++i)
    {
        //more favorites than buttons
        FavoriteAssetButton* button = new FavoriteAssetButton(this);
        _buttons.append(button);
        connect(button, &QAbstractButton::clicked, this, &FavoriteAssetsPicker::onButtonClicked);

        _layout->addWidget(button);
    }

    _buttons_byAsset.clear();
    int index = 0;    
    for (auto id : _presenter.favorites())
    {
        auto asset = _presenter.assetById(id);
        FavoriteAssetButton* button = _buttons[index];
        button->setPresenter(asset);
        _buttons_byAsset.insert(asset->id(), button);

        ++index;
    }
}

void FavoriteAssetsPicker::onSelectionChanged()
{
    for (auto button : _buttons)
    {
        const QSignalBlocker blocker(button);
        button->setChecked(false);
    }
    for (auto id : _presenter.selectedIds())
    {   
        FavoriteAssetButton* button = _buttons_byAsset.value(id);
        const QSignalBlocker blocker(button);
        button->setChecked(true);
    }
}

void FavoriteAssetsPicker::onButtonClicked(bool checked)
{
    auto button = qobject_cast<FavoriteAssetButton*>(sender());
    if (!button) return;

    auto asset = button->presenter();
    if (!asset) return;

    _presenter.select(asset->id());
}

FavoriteAssetButton::FavoriteAssetButton(QWidget* parent)
    : QToolButton(parent)
{
    setCheckable(true);
}

void FavoriteAssetButton::setPresenter(PresenterAssignment<IAssetPresenter> presenter)
{
    if (presenter == _presenter) return;

    _presenter = presenter;

    if (!_presenter) return;

    setIcon(_presenter->icon());
}
