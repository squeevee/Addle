#ifndef FILEISSUEPRESENTERBUILDER_HPP
#define FILEISSUEPRESENTERBUILDER_HPP

#include <boost/optional.hpp>

#include <QString>

#include "interfaces/presenters/messages/imessagepresenter.hpp"
#include "interfaces/presenters/messages/inotificationpresenter.hpp"
#include "interfaces/presenters/messages/ifileissuepresenter.hpp"

namespace Addle {
    
template<typename PublicType>
class BaseMessagePresenterBuilder
{
public:
    typedef IMessagePresenter::Tone Tone;
    
    inline IMessageContext* context() const { return _context; }
    inline PublicType& setContext(IMessageContext* context) { _context = context; return this_(); }

    inline boost::optional<bool> isUrgentHint() const { return _isUrgentHint; }
    inline PublicType& setIsUrgentHint(bool value) { _isUrgentHint = value; return this_(); }
    
    inline boost::optional<Tone> toneHint() const { return _toneHint; }
    inline PublicType& setToneHint(Tone tone) { _toneHint = tone; return this_(); }
    
    inline QString textHint() const { return _textHint; }
    inline PublicType& setTextHint(QString textHint) { _textHint = textHint; return this_(); }
    
protected:
    IMessageContext* _context = nullptr;
    boost::optional<bool> _isUrgentHint = boost::none;
    boost::optional<Tone> _toneHint = boost::none;
    
    QString _textHint;
    
private:
    inline PublicType& this_() { return static_cast<PublicType&>(*this); }
};

class NotificationPresenterBuilder : public BaseMessagePresenterBuilder<NotificationPresenterBuilder>
{
public:
    NotificationPresenterBuilder(QString text, Tone tone)
    {
        _textHint = text;
        _toneHint = tone;
    }
    
    inline QSharedPointer<AddleException> exception() const { return _exception; }
    inline NotificationPresenterBuilder& setException(QSharedPointer<AddleException> ex)
    {  _exception = ex; return *this;  }
    
private:
    QSharedPointer<AddleException> _exception;
};
    
class FileIssuePresenterBuilder : public BaseMessagePresenterBuilder<FileIssuePresenterBuilder>
{
public:
    typedef IFileIssuePresenter::Issue Issue;
    
    inline Issue issue() const { return _issue; }
    inline FileIssuePresenterBuilder& setIssue(Issue issue) { _issue = issue; return *this; }
    
    inline QSharedPointer<AddleException> exception() const { return _exception; }
    inline FileIssuePresenterBuilder& setException(QSharedPointer<AddleException> exception) { _exception = exception; return *this; }
    
    inline QUrl url() const { return _url; }
    inline FileIssuePresenterBuilder& setUrl(QUrl url) { _url = url; return *this; }
    
    inline QDir relativeDir() const { return _relativeDir; }
    inline FileIssuePresenterBuilder& setRelativeDir(QDir dir) { _relativeDir = dir; return *this; }
    
private:
    Issue _issue = Issue::UnknownIssue;
    QSharedPointer<AddleException> _exception;
    
    QUrl _url;
    QDir _relativeDir;
};
    
} // namespace Addle

#endif // FILEISSUEPRESENTERBUILDER_HPP
