#include <Parsers/Parsers.h>

namespace Parsers{

FamilyMemberParser::FamilyMemberParser()
:   EnumParser <Enums::Family::_Family>()
{
    namespace Family = Enums::Family;
    enumMap["self"]     =   Family::self;
    enumMap["parent"]   =   Family::parent;
    enumMap["children"] =   Family::children;
    enumMap["siblings"] =   Family::siblings;
    enumMap["child"]    =   Family::child;
    enumMap["sibling"]  =   Family::sibling;
    enumMap["app"] 		=	Family::application;
    enumMap["application"] =Family::application;
}

WidgetTypeParser::WidgetTypeParser()
: EnumParser <Enums::WidgetTypes::WidgetTypes> ()
{
    namespace WidgetTypes = Enums::WidgetTypes;
    enumMap["text"]             = WidgetTypes::text;
    enumMap["link"]             = WidgetTypes::link;
    enumMap["image"]            = WidgetTypes::image;
    enumMap["image link"]       = WidgetTypes::image_link;
    enumMap["image_link"]       = WidgetTypes::image_link;
    enumMap["submit button"]    = WidgetTypes::button;
    enumMap["submit"]           = WidgetTypes::button;
    enumMap["radio button"]     = WidgetTypes::radio_button_menu;
    enumMap["text area"]        = WidgetTypes::textarea;
    enumMap["textarea"]         = WidgetTypes::textarea;
    enumMap["input"]            = WidgetTypes::input_text;
    enumMap["input text"]       = WidgetTypes::input_text;
    enumMap["dropdown menu"]    = WidgetTypes::dropdown_menu;
    enumMap["dropdown"]         = WidgetTypes::dropdown_menu;
    enumMap["checkbox menu"]    = WidgetTypes::checkbox_menu;
    enumMap["checkbox"]         = WidgetTypes::checkbox_menu;
    enumMap["multi select"]     = WidgetTypes::multi_select;
    enumMap["container"]        = WidgetTypes::container;
    enumMap["stacked"]          = WidgetTypes::stack;
    enumMap["stack"]			= WidgetTypes::stack;
}

BitMaskParser::BitMaskParser()
: EnumParser <Enums::BitMasks::BitMasks> ()
{
	namespace Mask = Enums::BitMasks;
	enumMap["dynamic"]			= Mask::widget_is_dynamic;
	enumMap["rw"]				= Mask::widget_write_permissions;
}

StyleDirectiveParser::StyleDirectiveParser()
: EnumParser <Enums::StyleDirectives::StyleDirectives> ()
{
    namespace StyleDirectives = Enums::StyleDirectives;
    enumMap["class"]                = StyleDirectives::css_class;
    enumMap["children"]             = StyleDirectives::children;
    enumMap["alignment"]            = StyleDirectives::content_alignment;
    enumMap["content"]              = StyleDirectives::text_content;
    enumMap["file"]                 = StyleDirectives::text_file;

    /*!\brief Location of the source file for the image. */
    enumMap["source"]               = StyleDirectives::source;

    /*!\brief The location of an EXTERNAL link. */
    enumMap["location"]             = StyleDirectives::location;

    /*!\brief Either 'on' or 'off' */
    enumMap["events"]               = StyleDirectives::events;

    /*!\brief The type of the widget.
     * \sa Enums::WidgetTypes
     */
    enumMap["type"]                 = StyleDirectives::widget_type;
    enumMap["content node"]         = StyleDirectives::content_node;

    /*!\brief The path name of an INTERNAL path. */
    enumMap["internal_path"]        = StyleDirectives::internal_path;
    enumMap["internal path"]        = StyleDirectives::internal_path;
}

WtAlignmentParser::WtAlignmentParser ()
: EnumParser <Wt::AlignmentFlag> ()
{

    enumMap["bottom"]           = Wt::AlignBottom;
    enumMap["baseline"]         = Wt::AlignBaseline;
    enumMap["top"]              = Wt::AlignTop;
    enumMap["text top"]         = Wt::AlignTextTop;
    enumMap["text bottom"]      = Wt::AlignTextBottom;
    enumMap["left"]             = Wt::AlignLeft;
    enumMap["center"]           = Wt::AlignCenter;
    enumMap["middle"]           = Wt::AlignMiddle;
    enumMap["right"]            = Wt::AlignRight;
    enumMap["justify"]          = Wt::AlignJustify;
}

WtAnimationParser::WtAnimationParser ()
: EnumParser <Wt::WAnimation::AnimationEffect>()
{
    enumMap["slideInFromLeft"]  = Wt::WAnimation::SlideInFromLeft;
    enumMap["slideInFromRight"] = Wt::WAnimation::SlideInFromRight;
    enumMap["SlideInFromBottom"] = Wt::WAnimation::SlideInFromBottom;
    enumMap["SlideInFromTop"] = Wt::WAnimation::SlideInFromTop;
    enumMap["Pop"] = Wt::WAnimation::Pop;
    enumMap["Fade"] = Wt::WAnimation::Fade;
}

SignalActionParser::SignalActionParser()
: EnumParser <Enums::SignalActions::SignalAction>()
{
    namespace Action = Enums::SignalActions;
    enumMap["bubble"]       = Action::bubble;
    enumMap["trickle"]      = Action::trickle;
    enumMap["rebroadcast"]  = Action::rebroadcast;
    enumMap["set_style"]    = Action::set_style;
    enumMap["set_index"]    = Action::set_index;
    enumMap["set_path"]		= Action::set_internal_path;
    enumMap["block"]        = Action::BLOCK;
}

NodeValueTypeParser::NodeValueTypeParser()
: EnumParser <Enums::NodeValueTypes::NodeValueTypes> ()
{
    namespace NodeValueTypes = Enums::NodeValueTypes;
    enumMap["string_value"]     = NodeValueTypes::string_value;
    enumMap["field_value"]      = NodeValueTypes::field_value;
    enumMap["enum_value"]       = NodeValueTypes::enum_value;
    enumMap["dynamic_node_value"] = NodeValueTypes::dynamic_node_value;
    enumMap["static_node_value"] = NodeValueTypes::static_node_value;
    enumMap["float_value"]      = NodeValueTypes::float_value;
}

StorageModeParser::StorageModeParser()
: EnumParser <Enums::SubmissionPolicies::StorageMode>()
{
	namespace Mode = Enums::SubmissionPolicies;
	enumMap["append"] = Mode::append;
	enumMap["replace"] = Mode::replace;
}

StorageTypeParser::StorageTypeParser()
: EnumParser <Enums::SubmissionPolicies::StorageType>()
{
	namespace Type 	= Enums::SubmissionPolicies;
	enumMap["list"] 	= Type::list;
	enumMap["string"] 	= Type::string;
	enumMap["hash"] 	= Type::hash;
}

StorageWrappingParser::StorageWrappingParser()
: EnumParser <Enums::SubmissionPolicies::DataWrapping>()
{
	namespace Type 		= Enums::SubmissionPolicies;
	enumMap["none"] 		= Type::NONE;
	enumMap["enum"] 		= Type::enumerated_type;
	enumMap["static_node"] 	= Type::static_node;
	enumMap["dynamic_node"] = Type::dynamic_node;
	enumMap["integer"] 		= Type::integral_type;
	enumMap["float"]		= Type::floating_type;
	enumMap["file"]			= Type::file;
}

HashableAnswer_ErrorFlagParser::HashableAnswer_ErrorFlagParser()
{
    namespace Flag = Enums::HashableAnswer_ErrorFlags;
    enumMap[1]                  = Flag::err_no_hashable_data;
    enumMap[3]                  = Flag::err_no_hashable_question;
    enumMap[5]                  = Flag::err_no_hashable_question;
    enumMap[7]                  = Flag::err_no_hashable_question;
    enumMap[9]                  = Flag::err_no_hashable_form;
    for (unsigned char c = 11; c < 33; c+=2)
    {
        enumMap[c]              = Flag::err_no_hashable_answer;
    }
    enumMap[33]                 = Flag::err_no_hashable_form;
    enumMap[35]                 = Flag::err_no_hashable_question;
    enumMap[37]                 = Flag::err_no_hashable_question;
    enumMap[39]                 = Flag::err_no_hashable_question;
    enumMap[41]                 = Flag::err_no_hashable_form;
    for (unsigned char c = 43; c < 65; c+=2)
    {
        enumMap[c]              = Flag::is_hashable;
    }
    enumMap[65]                 = Flag::err_hash_already_set;
    enumMap[129]                = Flag::err_unknown;
}

inline Enums::HashableAnswer_ErrorFlags::HashableAnswer_ErrorFlags
HashableAnswer_ErrorFlagParser::parse (unsigned char& ch)
{
    if (ch % 2 == 0) return
            Enums::HashableAnswer_ErrorFlags::err_no_database_selected;
    if (ch >129) return enumMap[129];
    if (ch >65) return enumMap[65];
    return enumMap[ch];
}

Parsers::SignalTriggerParser::SignalTriggerParser()
:EnumParser<Enums::SignalTriggers::SignalTrigger>()
{
    namespace Trigger = Enums::SignalTriggers;
    enumMap["click"]    = Trigger::click;
    //enumMap["action"]   = Trigger::action_name;
    enumMap["mouseover"]    = Trigger::mouseover;
    enumMap["mouseout"]     = Trigger::mouseout;
    enumMap["style_changed"]= Trigger::style_changed;
}

}// <-namespace Parsers
