#ifndef MEMBERCONTAINER_H_
#define MEMBERCONTAINER_H_

#include "MoldableContainer.h"

class MemberContainer : public MoldableContainer
{
public:
    MemberContainer(const std::string& node);

private:
    virtual void init();

};

#endif //MEMBERCONTAINER_H_
