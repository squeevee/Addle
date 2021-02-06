#include "fileissuedialog.hpp"

#include "utilities/view/guitrids.hpp"

#include "utilities/iconlabel.hpp"

#include "utils.hpp"

#include <QBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QSpacerItem>
#include <QStyle>

using namespace Addle;

FileIssueDialog::FileIssueDialog(IFileIssuePresenter& presenter)
    : _presenter(presenter),
    _tlvHelper(this, std::bind(&FileIssueDialog::setupUi, this))
{
    //ADDLE_ASSERT(_presenter);
    
    _tlvHelper.onOpened.bind(&FileIssueDialog::tlv_opened, this);
    _tlvHelper.onClosed.bind(&FileIssueDialog::tlv_closed, this);
    
    connect_interface(this, SIGNAL(rejected()), &_presenter, SIGNAL(cancel()));
}

void FileIssueDialog::setupUi()
{
    ASSERT_INIT();

    QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
    setLayout(layout);
    
    QBoxLayout* textRow = new QBoxLayout(QBoxLayout::LeftToRight);
    
    auto tone = _presenter.tone();
    if (tone == IMessagePresenter::Issue || tone == IMessagePresenter::Problem)
    {
        QIcon icon = style()->standardIcon(
            tone == IMessagePresenter::Issue ? QStyle::SP_MessageBoxWarning : QStyle::SP_MessageBoxCritical
        );
        
        IconLabel* label_icon = new IconLabel(icon);
        label_icon->setMaximumSize(64, 64);
        label_icon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        textRow->addWidget(label_icon);
    }
    
    QLabel* label_text = new QLabel;
    label_text->setText(_presenter.text());
    textRow->addWidget(label_text);
    
    layout->addLayout(textRow);
    
    QBoxLayout* infoRow = new QBoxLayout(QBoxLayout::LeftToRight);
    
    if (_presenter.canPreview())
    {
        _preview = new QLabel;
        _preview->setMinimumSize(64, 64);
        _preview->setMaximumSize(64, 64); // TODO
        _preview->setPixmap(_presenter.preview());
        infoRow->addWidget(_preview);
    }
    
    QScrollArea* statsArea = new QScrollArea;
    statsArea->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    statsArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    statsArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    infoRow->addWidget(statsArea);
    
    QWidget* statsWidget = new QWidget;
    QFormLayout* statsLayout = new QFormLayout;
    
    for (auto stat : noDetach(_presenter.fileStats()))
    {
        QLabel* statLabel = new QLabel(stat.first);
        QLabel* statValue = new QLabel(stat.second);
        statValue->setTextInteractionFlags(Qt::TextSelectableByMouse);
        statsLayout->addRow(statLabel, statValue);
    }
    
    statsWidget->setLayout(statsLayout);
    statsArea->setWidget(statsWidget);
    layout->addLayout(infoRow);
    
    QBoxLayout* buttonStrip = new QBoxLayout(QBoxLayout::RightToLeft);
    
    QPushButton* button_cancel = new QPushButton;
    button_cancel->setText(qtTrId(GuiTrIds::DIALOG_CANCEL));
    button_cancel->setDefault(true);
    connect(button_cancel, &QAbstractButton::clicked, this, &QDialog::reject);
    buttonStrip->addWidget(button_cancel);
    
    if (_presenter.canOverwrite())
    {
        QPushButton* button_overwrite = new QPushButton;
        connect_interface(button_overwrite, SIGNAL(clicked()), &_presenter, SIGNAL(overwrite()));
        buttonStrip->addWidget(button_overwrite);
    }
    
    buttonStrip->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
    layout->addLayout(buttonStrip);
}
