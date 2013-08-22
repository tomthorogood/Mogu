#ifndef DYNAMIC_SERVER_H_
#define DYNAMIC_SERVER_H_

#include "WidgetServerInterface.h"

class Dynamic_Server : public Widget_Server_Interface
{
public:
    Dynamic_Server (){}
    virtual Widget_Assembly request 
        ( const std::string& node
        , const int& u=-1
        , const int& g=-1);

    inline void set_user_id(const int& i) { user_id = i};
    inline void set_group_id(const int& i) { group_id = i;}

protected:

private:
    int user_id {};
    int group_id {};
};


#endif //DYNAMIC_SERVER_H_
