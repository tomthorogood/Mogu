/*
 * P_Signals.h
 *
 *  Created on: Aug 18, 2012
 *      Author: tom
 */

#ifndef P_SIGNALS_H_
#define P_SIGNALS_H_

namespace Parsers {

class FamilyMemberParser: public TurnLeft::Utils::EnumParser<
    Enums::Family::_Family>
{
public:
    FamilyMemberParser()
    {
        namespace Family = Enums::Family;
        enumMap["self"] = Family::self;
        enumMap["parent"] = Family::parent;
        enumMap["children"] = Family::children;
        enumMap["siblings"] = Family::siblings;
        enumMap["child"] = Family::child;
        enumMap["sibling"] = Family::sibling;
        enumMap["app"] = Family::application;
        enumMap["application"] = Family::application;
    }
};

class SignalActionParser: public TurnLeft::Utils::EnumParser<
    Enums::SignalActions::SignalAction>
{
public:
    SignalActionParser()
    {

        namespace Action = Enums::SignalActions;
        enumMap["bubble"] = Action::bubble;
        enumMap["emit"] = Action::emit;
        enumMap["trickle"] = Action::trickle;
        enumMap["rebroadcast"] = Action::rebroadcast;
        enumMap["set_style"] = Action::set_style;
        enumMap["set_index"] = Action::set_index;
        enumMap["set_path"] = Action::set_internal_path;
        enumMap["store"] = Action::store_value;
        enumMap["remove_child"] = Action::remove_child;
        enumMap["add_child"] = Action::add_widget;
        enumMap["block"] = Action::BLOCK;
        enumMap["add_class"] = Action::add_class;
        enumMap["remove_class"] = Action::remove_class;
        enumMap["increment_index"] = Action::increment_index;
        enumMap["decrement_index"] = Action::decrement_index;
        enumMap["clear"] = Action::clear;
        enumMap["change_session"] = Action::change_session;
        enumMap["register_user"] = Action::register_user;
        enumMap["slot"] = Action::slot;
        enumMap["match"] = Action::match;
        enumMap["delete_value"] = Action::delete_value;
        enumMap["set_text"] = Action::set_text;
        enumMap["test_text"] = Action::test_text;
        enumMap["test"] = Action::test;
        enumMap["javascript"] = Action::javascript;
        enumMap["email_user"] = Action::email_user;
        enumMap["read_state"] = Action::read_state;
        enumMap["reset_password"] = Action::reset_password;
        enumMap["onload"] = Action::onload;
        enumMap["reload"] = Action::reload;
        enumMap["activate_global"] = Action::activate_global;
        enumMap["change_group"] = Action::change_group;
        enumMap["stdout"] = Action::STDOUT;
#ifdef TERM_ENABLED
        enumMap["TERMINATE"] = Action::TERM;
#endif
    }
};

class SignalTriggerParser: public TurnLeft::Utils::EnumParser<
    Enums::SignalTriggers::SignalTrigger>
{
public:
    SignalTriggerParser()
    {
        namespace Trigger = Enums::SignalTriggers;
        enumMap["click"] = Trigger::click;
        enumMap["mouseover"] = Trigger::mouseover;
        enumMap["mouseout"] = Trigger::mouseout;
        enumMap["style_changed"] = Trigger::style_changed;
        enumMap["fail"] = Trigger::fail;
        enumMap["succeed"] = Trigger::succeed;
        enumMap["keyup"] = Trigger::keyup;
        enumMap["enter"] = Trigger::enter_pressed;
        enumMap["index_changed"] = Trigger::index_changed;
        enumMap["hidden_changed"] = Trigger::hidden_changed;
    }
};

}    //namespace Parsers

#endif /* P_SIGNALS_H_ */
