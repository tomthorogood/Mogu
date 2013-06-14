/*
 * MoldablePassword.h
 *
 *  Created on: Dec 10, 2012
 *      Author: tom
 */

#ifndef MOLDABLEPASSWORD_H_
#define MOLDABLEPASSWORD_H_

#include "MoldableInput.h"

class MoldablePassword : public MoldableInput
{
protected:

public:
    MoldablePassword (const std::string& node);
};

#endif /* MOLDABLEPASSWORD_H_ */
