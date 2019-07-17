#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

#include "interfaces/presenters/ieditorpresenter.h"
#include "interfaces/presenters/ieditorviewportpresenter.h"

namespace Ui {
class MainWindow;
class MainWindow_ZoomWidget;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow* _ui;
    Ui::MainWindow_ZoomWidget* _zoomWidgetUi;
    QWidget* _zoomWidget;

    IEditorPresenter* _presenter;
    IEditorViewPortPresenter* _editorViewPortPresenter;

    void updateZoomWidget();

private slots:
    //void editorZoomChanged();
    //void editorRotationChanged();
    
    void zoomWidgetSpinBoxValueChanged(double value);
    void zoomWidgetSliderValueChanged(int value);
};


#endif // MAINWINDOW_H
