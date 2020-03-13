#ifndef DECORATIONHELPER_HPP
#define DECORATIONHELPER_HPP

#include <QAction>

#include "interfaces/presenters/ipropertydecoratedpresenter.hpp"
#include "utilities/presenter/propertydecoration.hpp"

#include "optionaction.hpp"

class DecorationHelper
{
public:
    DecorationHelper(const char* propertyName, const IPropertyDecoratedPresenter& presenter)
        : _propertyName(propertyName), _presenter(presenter)
    {
        decoration = _presenter.getPropertyDecoration(propertyName);
    }

    void decorate(QAction* action)
    {
        action->setIcon(decoration.getIcon());
        action->setText(decoration.getText());
        action->setToolTip(decoration.getToolTip());
    }

    void decorateOption(OptionAction* option)
    {
        OptionDecoration optionDecoration = decoration.getDecoration(option->getValue());

        option->setIcon(optionDecoration.getIcon());
        option->setText(optionDecoration.getText());
        option->setToolTip(optionDecoration.getToolTip());
    }

private:
    const char* _propertyName;
    PropertyDecoration decoration;
    const IPropertyDecoratedPresenter& _presenter;
};

#endif // DECORATIONHELPER_HPP