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

    /*!\brief When read in from the database, integer values are transformed
     * into integers.
     */
    ,integer_value		=0x2

    /*!\brief When read in from the database, enum values are parsed with a
     * callback that returns an integer representation of the enumerated type.
     */
    ,float_value		=0x3

    /*!\brief When readin from the database, the value from another node
     * in the database is read in.
     */
    ,enum_value			=0x4

    /*!\brief Returns a string or integer based on a widget's state
     */
    ,widget_state		=0x5

    /*!\brief When read in from the datbase, file_values will attempt to open
     * a file at the listed location, and will read the contents of the file.
     */
    ,file_value			=0x8

    /*!\brief Reads a node from the database */
    ,node_value			=0x9
};
} //namespace NodeValueTypes

#endif /* E_NODEVALUES_H_ */
