/*
 * ForEach.h
 *
 *  Created on: Nov 20, 2012
 *      Author: tom
 */

#ifndef FOREACH_H_
#define FOREACH_H_

#include <declarations.h>
#include <Core/Moldable.h>
#include <Types/GroupManager.h>
#include <Mogu.h>

namespace Goo {
namespace MoldableFactory {
namespace Sculptory {

class AbstractForEach
{
private:
    Wt::WContainerWidget* receiver;

protected:
    Mogu* app;
    std::vector<std::string> __values;
    virtual void populateValues() =0;
    virtual void getNumChildren() =0;
    void execute(
        Moldable& parent);
    MoldableTemplate& __tmpl;

public:
    AbstractForEach(
        MoldableTemplate& __tmpl, Moldable& m);
    virtual ~AbstractForEach()
    {
    }
};

class ForEachData: public AbstractForEach
{
    void populateValues();
    void getNumChildren();
public:
    ForEachData(
        MoldableTemplate& __tmpl, Moldable& m);
};

class ForEachGroupMember: public AbstractForEach
{
    std::vector<std::string> memberSessions;
    const std::string& groupName;
    GroupManager groupManager;

    void populateValues();
    void getNumChildren();
public:
    ForEachGroupMember(
        MoldableTemplate& __tmpl, Moldable& m);
};

}    //namespace Sculptory
}    //namespace MoldableFactory
}    //namespace Goo

#endif /* FOREACH_H_ */
