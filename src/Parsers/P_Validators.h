/*
 * P_Validators.h
 *
 *  Created on: Aug 18, 2012
 *      Author: tom
 */

#ifndef P_VALIDATORS_H_
#define P_VALIDATORS_H_

namespace Parsers{

class ValidatorTypeParser:
	public TurnLeft::Utils::EnumParser <Enums::Validators::ValidatorTypes>
{
public:
	ValidatorTypeParser()
	{
		namespace Type = Enums::Validators;
		enumMap["regex"]	= Type::regex;
	}

};


}//namespace Parsers


#endif /* P_VALIDATORS_H_ */
