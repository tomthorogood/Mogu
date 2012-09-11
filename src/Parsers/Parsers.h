#ifndef FF_ENUM_PARSERS_H_
#define FF_ENUM_PARSERS_H_


#include <declarations.h>
#include <Wt/WContainerWidget>

#include <Parsers/P_Signals.h>
#include <Parsers/P_Submissions.h>
#include <Parsers/P_Validators.h>
#include <Parsers/P_Widgets.h>
#include <Parsers/P_WidgetStyling.h>

namespace Parsers{

template <class EnumType>
class ReverseParser : public TurnLeft::Utils::EnumParser <EnumType>
{
    std::map <EnumType, std::string> stringMap;
public:
    std::string rparse (EnumType _enum)
    {
        return stringMap[_enum];
    }
};

template <class ParserClass>
int enum_callback(const std::string& val)
{
    ParserClass parser;
    return (int) parser.parse(val);
}

} // <-- namespace Parsers

#endif //FF_ENUM_PARSERS_H_
