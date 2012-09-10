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
    string_value        =0x0,//!< string_value

    /*!\brief When read in from the database, field values are attributed
     * to a field in the node being read.
     */
    field_value         =0x1,//!< field_value

    /*!\brief When read in from the database, integer values are transformed
     * into integers.
     */
    integer_value       =0x2,//!< integer_value

    /*!\brief When read in from the database, enum values are parsed with a
     * callback that returns an integer representation of the enumerated type.
     */
    enum_value          =0x3,//!< enum_value

    /*!\brief When readin from the database, the value from another node
     * in the database is read in.
     */
    static_node_value   =0x5,

    /*!\brief When read in from the database, dynamic node values themselves
     * contain variables; these are used mainly to access values specific to
     * a given user. This functionality has not yet been implemented.
     */
    dynamic_node_value  =0xD,//!< dynamic_node_value

    /*!\brief When read in from the database, float_values are parsed as
     * floats.
     */
    float_value         =0x12,//!< float_value

    /*!\brief When read in from the datbase, file_values will attempt to open
     * a file at the listed location, and will read the contents of the file.
     */
    file_value          =0x18
};
} //namespace NodeValueTypes

#endif /* E_NODEVALUES_H_ */
