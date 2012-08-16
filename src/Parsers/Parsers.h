#ifndef FF_ENUM_PARSERS_H_
#define FF_ENUM_PARSERS_H_


#include <declarations.h>
#include <TurnLeftLib/Utils/enumparser.h>
#include <Wt/WContainerWidget>

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

class WidgetTypeParser :
    public TurnLeft::Utils::EnumParser <Enums::WidgetTypes::WidgetTypes>
{
public: WidgetTypeParser();
};

class BitMaskParser :
	public TurnLeft::Utils::EnumParser <Enums::BitMasks::BitMasks>
{
public: BitMaskParser();
};

class StyleDirectiveParser :
    public TurnLeft::Utils::EnumParser <Enums::StyleDirectives::StyleDirectives>
{
public: StyleDirectiveParser();
};

class WtAlignmentParser :
    public TurnLeft::Utils::EnumParser <Wt::AlignmentFlag>
{
public: WtAlignmentParser();
};

class FamilyMemberParser :
    public TurnLeft::Utils::EnumParser <Enums::Family::_Family>
{
public: FamilyMemberParser();
};

class NodeValueTypeParser :
    public TurnLeft::Utils::EnumParser
        <Enums::NodeValueTypes::NodeValueTypes>
{
public: NodeValueTypeParser();
};

class SignalActionParser:
    public TurnLeft::Utils::EnumParser<Enums::SignalActions::SignalAction>
{
public: SignalActionParser();
};

class StorageModeParser :
	public TurnLeft::Utils::EnumParser<Enums::SubmissionPolicies::StorageMode>
{
public: StorageModeParser();
};

class StorageTypeParser :
	public TurnLeft::Utils::EnumParser<Enums::SubmissionPolicies::StorageType>
{
public: StorageTypeParser();
};

class StorageWrappingParser :
	public TurnLeft::Utils::EnumParser<Enums::SubmissionPolicies::DataWrapping>
{
public: StorageWrappingParser();
};

class SignalTriggerParser:
    public TurnLeft::Utils::EnumParser<Enums::SignalTriggers::SignalTrigger>
{
public: SignalTriggerParser();
};

class ValidatorTypeParser:
	public TurnLeft::Utils::EnumParser <Enums::Validators::ValidatorTypes>
{
public: ValidatorTypeParser();
};

class WtAnimationParser:
    public TurnLeft::Utils::EnumParser<Wt::WAnimation::AnimationEffect>
{
public: WtAnimationParser();
};

class HashableAnswer_ErrorFlagParser
{
    std::map
        <char, Enums::HashableAnswer_ErrorFlags::HashableAnswer_ErrorFlags>
            enumMap;
public:
    HashableAnswer_ErrorFlagParser();
    Enums::HashableAnswer_ErrorFlags::HashableAnswer_ErrorFlags 
        parse(unsigned char&);
};



} // <-- namespace Parsers

#endif //FF_ENUM_PARSERS_H_
