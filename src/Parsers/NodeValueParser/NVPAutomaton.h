/*
 * NVPAutomaton.h
 *
 *  Created on: Oct 24, 2012
 *      Author: tom
 */

#ifndef NVPAUTOMATON_H_
#define NVPAUTOMATON_H_

#include <declarations.h>
#include <FiniteAutomaton/FiniteAutomaton.h>
#include <Parsers/MoguScript_Tokenizer.h>
#include <Types/NodeValue.h>
#include <cassert>

namespace Parsers {

/*!\brief This represents all posible states of the machine. The prefix, NVP,
 * is to avoid collisions. The second portion, one or more letters, corresponds
 * to the letters of the different states in the NVP automaton diagram document.
 * The last portion (T, F, or TF) represents whether the state is traversed,
 * final, or a combination of the two.
 */
enum NVP_States
{
    NVP_START    //!< NVP_START
    ,
    NVP_ABCL_F    //!< NVP_ABCL_F
    ,
    NVP_K_T    //!< NVP_K_T
    ,
    NVP_D_F    //!< NVP_D_F
    ,
    NVP_E_TF    //!< NVP_E_TF
    ,
    NVP_F_TF    //!< NVP_F_TF
    ,
    NVP_J_TF    //!< NVP_J_TF
    ,
    NVP_GHI_F    //!< NVP_GHI_F
    ,
    NVP_M_F    //!< NVP_M_F
};

/*!\brief The node value parser is the logic unit for handling Mogu syntax.
 *
 * This is the runtime interpreter for mogu syntax encountered when reading
 * data from the Redis database. Most lines read in from the database will
 * go through this machine.
 */
class NodeValueParser: public Automaton::FiniteAutomatonTmpl<std::string, void,
    NVP_States>
{

public:

    /*!\brief These are the possible types of output for each token encountered.
     * A string value is treated as everything that isn't something else.
     */
    enum Outputs
    {
        /*!\brief A value that is interpreted as a generic string within C++ */
        string_value = 0x1    //1

        /*!\brief  A value that will be interpreted using stoi() into a
         * C++ integer.
         */
        ,
        integer_value = 0x2    //2

        /*!\brief A value that will be interpreted using ftof() into a
         * C++ floating type value.
         */
        ,
        float_value = 0x4    //4

        /*!\brief A string that will be hashed using Mogu's hashing method
         * (currently cityhash), after which it will be used as a std::string
         */
        ,
        hashed_string = 0x8    //8

        /*!\brief A value that can be parsed into an enumerated value using
         * an enum callback. This requires an extra parser be passed into the
         * machine.
         */
        ,
        enum_value = 0x10    //16

        /*!\brief A value that is evaluated on the fly to determine some
         * characteristic about a widget currently displayed on the user's screen.
         * This requires a token preceeding it, such as "|widget_name|" or "{self}".
         * {self} will refer to the widget invoking the parser.
         */
        ,
        widget_state = 0x20    //32

        /*!\brief A value that represents a node in the Redis instance that
         * belongs to a user's keyspace. This value will be hashed and then looked
         * up. If a second token follows, that token must assist the Redis lookup.
         * For instance, if the node in Redis is a hash, a string or integer may
         * follow. If a list, an integer must accompany this in order to properly
         * look up the index of the data requested.
         */
        ,
        dynamic_storage = 0x40    //64

        /*!\brief Much like dynamic storage, but instead points to a globally
         * accessible node not specific to any user.
         */
        ,
        data_node = 0x80    //128

        /*!\brief A widget that has been given a name and exists within the
         * application registry.
         */
        ,
        registered_widget = 0x100    //256

        /*!\brief This specific token indicates group access */
        ,
        access_token = 0x200

    };

    NodeValueParser();
    NodeValueParser(
        std::string input, NodeValue& v, Moldable* broadcaster = 0);

    /*!\brief Implemented from FiniteAutomatonTmpl */
    void giveInput(std::string input);

    /*!\brief Allows the automaton to remain live for multiple uses. */
    void giveInput(
        std::string input, NodeValue& v, Moldable* broadcaster = 0);

    inline void reset()
    {
        __broadcaster = 0;
        __iovalue = 0;
        tokens.clear();
        tokenTypes.clear();
        __tokenizer.reset();
    }

    /*!\brief Upon deletion, will delete all states as well. */
    ~NodeValueParser()
    {
        std::map<NVP_States, Automaton::AbstractState*>::iterator iter =
            __states.begin();
        while (iter != __states.end()) {
            if (iter->second != nullptr) delete iter->second;
            ++iter;
        }
    }

    /*!\brief Implemented from FiniteAutomatonTmpl */
    inline bool hasNextToken()
    {
        return __tokenizer.hasNext();
    }

private:
    /*!\brief Valid outputs from the starting state */
    uint16_t __vo_Start;

    /*!\brief Valid outputs from state ABCL */
    uint16_t __vo_ABCL;

    /*!\brief Valid outputs from state K */
    uint16_t __vo_K;

    /*!\brief Valid outputs from state E */
    uint16_t __vo_E;

    /*!\brief Valid outpus from state D */
    uint16_t __vo_D;

    /*!\brief Valid outputs from state F */
    uint16_t __vo_F;

    /*!\brief Valid outputs from state J */
    uint16_t __vo_J;

    /*!\brief Valid outputs from state GHI */
    uint16_t __vo_GHI;

    /*!\brief Instantiationi of the MoguScript_Tokenizer which splits any
     * mogu syntax string into its requisite parts.
     */
    Parsers::MoguScript_Tokenizer __tokenizer;

    /*!\brief A vector which holds each token as it is encountered. */
    std::vector<std::string> tokens;

    /*!\brief A vector which holds each token type as it is encountered. */
    std::vector<NodeValueParser::Outputs> tokenTypes;

    /*!\brief A reference to the value passed into the machine. */
    NodeValue* __iovalue;

    /*!\brief Enables reuse of initializers. */
    void __init__();

    /*!\brief The widget that invoked the NVP */
    Moldable* __broadcaster;

};

using Automaton::State;
using Automaton::FiniteAutomaton;

/*!\brief A state within a NodeValueParser machine. */
class NVP_State: public State<NodeValueParser::Outputs, std::string, NVP_States,
    uint16_t>
{
public:
    /*!\brief The constructor.
     *
     * @param NVP_Instance The NodeValueParser housing this state.
     * @param state_type Whether this state is a start, traversal, final, or
     * some combination thereof.
     * @param name This state's identifier (\sa NVP_States)
     * @param valid_outputs A bit-masked integral literal.
     * @param callback The test this state will run against its input.
     */
    NVP_State(
        FiniteAutomaton* NVP_Instance, uint8_t state_type,
        const NVP_States& name, const uint16_t& valid_outputs,
        NodeValueParser::Outputs (*callback)(
            const std::string&))
        : State<NodeValueParser::Outputs, std::string, NVP_States, uint16_t>(
            NVP_Instance, state_type, name, valid_outputs, callback)
    {
    }
};

/*!\brief Based on an input token and its wrapper chars, returns an
 * enumerated value.
 * @param token Any input string.
 * @return The Mogu type of that string.
 */
NodeValueParser::Outputs getMoguType(
    const std::string& token);

/* For enums, ints, floats, strings, hashed strings */
/*!\brief For single tokens that require no database lookup.
 *
 * @param v A NodeValue which will hold the result.
 * @param input The input string.
 * @param tokentype The type of the input, as determined by getMoguType
 * @param broadcaster (optional) The widget invoking the parser
 */
void parse_single_static_token(
    NodeValue& v
    , std::string& input
    , NodeValueParser::Outputs tokentype);

/*!\brief Resolves a node from within a user's keyspace within the Redis instance.
 *
 * @param v The NodeValue which will hold the result.
 * @param base The base name for the node (i.e. "[profile]")
 * @param arg (optional, used if node is a list or hash)
 * @param arg_type (optional, used if node is a lsit or hash)
 */
void parse_session_node(
    NodeValue& v
    , std::string& base
    , std::string arg = EMPTY,
    NodeValueParser::Outputs arg_type = NodeValueParser::string_value);

/*!\brief Resolves a node from within the data keyspace, globally accessible.
 *
 * @param v The NodeValue which will hold the result.
 * @param base The base name for the node (i.e. "@data.bunch_of_numbers@")
 * @param arg (optional, used if node is a list or hash)
 * @param arg_type (optional, used if node is a list or hash)
 */
void parse_data_node(
    NodeValue& v
    , std::string& base
    , std::string arg = EMPTY,
    NodeValueParser::Outputs arg_type = NodeValueParser::string_value,
    Moldable* broadcaster = nullptr);

/*!\brief Resolves information about a widget currently displayed on the
 * user's screen.
 * @param v The NodeValue which will hold the result.
 * @param widget_identifier Either "{self}" or a '|registered_name|'.
 * @param state The "$state$" to be resolved from the widget.
 * @param broadcaster The widget requesting the information; only used if
 * the identifier is {self}.
 */
void parse_widget_state(
    NodeValue& v
    , std::string widget_identifier
    , std::string& state
    , Moldable* broadcaster = 0);

}    //namespace Parsers
#endif /* NVPAUTOMATON_H_ */
