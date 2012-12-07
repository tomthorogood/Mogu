/*
 * P_Submissions.h
 *
 *  Created on: Aug 18, 2012
 *      Author: tom
 */

#ifndef P_SUBMISSIONS_H_
#define P_SUBMISSIONS_H_

namespace Parsers {

class StorageModeParser: public TurnLeft::Utils::EnumParser<
    Enums::SubmissionPolicies::StorageMode>
{
public:
    StorageModeParser()
    {
        namespace Mode = Enums::SubmissionPolicies;
        enumMap["append"] = Mode::append;
        enumMap["replace"] = Mode::replace;
    }
};

class StorageTypeParser: public TurnLeft::Utils::EnumParser<
    Enums::SubmissionPolicies::StorageType>
{
public:
    StorageTypeParser()
    {
        namespace Type = Enums::SubmissionPolicies;
        enumMap["list"] = Type::list;
        enumMap["string"] = Type::string;
        enumMap["hash"] = Type::hash;
    }
};

class StorageWrappingParser: public TurnLeft::Utils::EnumParser<
    Enums::SubmissionPolicies::DataWrapping>
{
public:
    StorageWrappingParser()
    {
        namespace Type = Enums::SubmissionPolicies;
        enumMap["none"] = Type::none;
        enumMap["string"] = Type::none;
        enumMap["enum"] = Type::enumerated_type;
        enumMap["static_node"] = Type::static_node;
        enumMap["dynamic_node"] = Type::dynamic_node;
        enumMap["integer"] = Type::integral_type;
        enumMap["float"] = Type::floating_type;
        enumMap["file"] = Type::file;
    }
};
}    //namespace Parsers

#endif /* P_SUBMISSIONS_H_ */
