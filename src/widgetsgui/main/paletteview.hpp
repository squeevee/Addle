#ifndef PALETTEVIEW_HPP
#define PALETTEVIEW_HPP

#include <QWidget>
#include <QDockWidget>

#include "interfaces/presenters/ipalettepresenter.hpp"
#include "widgetsgui/utilities/presenterassignment.hpp"

class PaletteView : public QWidget
{
    Q_OBJECT
public:
    PaletteView(QWidget* parent = nullptr);
    virtual ~PaletteView() = default;

    void setPresenter(PresenterAssignment<IPalettePresenter> presenter);

private:
    void updatePalette();

    PresenterAssignment<IPalettePresenter> _presenter;
};

#endif // PALETTEVIEW_HPP