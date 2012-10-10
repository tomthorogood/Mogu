/*
 * E_NodeValues.h
 *
 *  Created on: Aug 18, 2012
 *      Author: tom
 */

#ifndef E_NODEVALUES_H_
#define E_NODEVALUES_H_

namespace NodeValueTypes{

enum NodeValueTypes{
    /*!\brief When read in from the database, string values are interpreted
     * exactly as they are read.
     */
    string_value		=0x0

    /*!\brief When readin from the database, the value from another node
     * in the database is read in.
     */
    ,enum_value			=0x1
    /*!\brief When read in from the database, integer values are transformed
     * into integers.
     */
    ,integer_value		=0x2

    /*!\brief When read in from the database, enum values are parsed with a
     * callback that returns an integer representation of the enumerated type.
     */
    ,float_value		=0x3

    /*!\brief Returns a string or integer based on a widget's state
     */
    ,widget_state		=0x5

    /*!\brief When read in from the datbase, file_values will attempt to open
     * a file at the listed location, and will read the contents of the file.
     */
    ,file_value			=0x8

    /*!\brief Reads a node from the database */
    ,node_value			=0x9

    ,registry_value		=0xA

    ,redis_command		=0xB
};

enum TokenTestResult{
	BEG			=0x0
	,ERR		=0x1
	,REQ_STR	=0x2
	,ICPL_CMD	=0x3
	,NXT_NODE	=0x4
	,NXT_REG	=0x5
	,NXT_CMD	=0x6
	,CPL		=0x8
};

enum InterpretationMode{
	VAL
	,ARG
};

namespace RedisTypes{
	const std::string REDIS_STR = "string";
	const std::string REDIS_LST = "list";
	const std::string REDIS_HSH = "hash";
	const std::string REDIS_NUL = "none";
}

} //namespace NodeValueTypes

#endif /* E_NODEVALUES_H_ */
