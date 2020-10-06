#ifndef ICONLABEL_HPP
#define ICONLABEL_HPP

#include <QLabel>
#include <QIcon>

namespace Addle {
    
class IconLabel : public QLabel
{
    Q_OBJECT
public:
    IconLabel(const QIcon& icon = QIcon(), QWidget* parent = nullptr);
    virtual ~IconLabel() = default;
    
    QIcon icon() const { return _icon; }
    void setIcon(QIcon icon);
    
protected:
    void resizeEvent(QResizeEvent* event) override;
    
private:
    QIcon _icon;
};
    
} // namespace Addle

#endif // ICONLABEL_HPP
