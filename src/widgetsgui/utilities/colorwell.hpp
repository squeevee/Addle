#ifndef COLORWELL_HPP
#define COLORWELL_HPP

#include <QFrame>

class ColorWell : public QFrame 
{
    Q_OBJECT 
public:
    ColorWell(QWidget* parent = nullptr);
    virtual ~ColorWell() = default; 

    QSize sizeHint() const override;

    void setColor(QColor color);

protected:
    void paintEvent(QPaintEvent* e) override;
    
    void mousePressEvent(QMouseEvent* e) override;

signals:
    bool clicked();

private: 
    QColor _color;
    QString _name;
};

#endif //COLORWELL_HPP