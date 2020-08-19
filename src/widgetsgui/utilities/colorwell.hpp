/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef COLORWELL_HPP
#define COLORWELL_HPP

#include <QFrame>
#include "utilities/model/colorinfo.hpp"

namespace Addle {

class ColorWell : public QFrame 
{
    Q_OBJECT
    Q_PROPERTY(
        Addle::ColorInfo info
        READ info 
        WRITE setInfo
    )
    Q_PROPERTY(
        bool isHighlighted 
        READ isHighlighted 
        WRITE setHighlighted
    )
public:
    ColorWell(QWidget* parent = nullptr);
    virtual ~ColorWell() = default; 

    QSize sizeHint() const override;

    ColorInfo info() const { return _info; }
    void setInfo(ColorInfo info);

    QColor color() const { return _info.color(); }

    void setSize(QSize size);

    bool isEmpty() const { return _isEmpty; }
    bool isHighlighted() const { return _isHighlighted; }

    void setHighlighted(bool value);

signals:
    void clicked();
    void doubleClicked();

protected:
    void paintEvent(QPaintEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseDoubleClickEvent(QMouseEvent* e) override;

private: 
    bool _isEmpty = true;
    bool _isHighlighted = false;

    ColorInfo _info;

    QSize _size;

    QPixmap _checkerTexture;
};

} // namespace Addle

#endif //COLORWELL_HPP
