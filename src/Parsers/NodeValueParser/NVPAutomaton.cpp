/*
 * NVPAutomaton.cpp
 *
 *  Created on: Oct 25, 2012
 *      Author: tom
 */

#include <Parsers/NodeValueParser/NVPAutomaton.h>
#include <Core/Moldable.h>

namespace Parsers
{
using namespace Automaton;
NodeValueParser::Outputs getMoguType(const std::string& token)
{
	char char0 = token.at(0);
	char fchar = token.at(token.length()-1);
	NodeValueParser::Outputs ntype;
	switch(char0)
	{
	case '{' : ntype = (fchar == '}')?
			NodeValueParser::enum_value : NodeValueParser::string_value;
		break;
	case '^' : ntype = (fchar == '^')?
			NodeValueParser::integer_value :  NodeValueParser::string_value;
		break;
	case '@' : ntype = (fchar == '@')?
			NodeValueParser::data_node : NodeValueParser::string_value;
		break;
	case '~' : ntype = (fchar == '~')?
			NodeValueParser::float_value : NodeValueParser::string_value;
		break;
	case '$' : ntype = (fchar == '$')?
			NodeValueParser::widget_state :  NodeValueParser::string_value;
		break;
	case '|' : ntype = (fchar == '|')?
			NodeValueParser::registered_node : NodeValueParser::string_value;
		break;
	case '[' : ntype = (fchar == ']')?
			NodeValueParser::dynamic_storage : NodeValueParser::string_value;
		break;
	case '&' : ntype = (fchar == '&')?
			NodeValueParser::hashed_string : NodeValueParser::string_value;
		break;
	default:
		ntype = NodeValueParser::string_value;
	}
	return ntype;
}

NodeValueParser::NodeValueParser(
		std::string input
		, Nodes::NodeValue& v
		, Goo::Moldable* broadcaster
		, int(*enumcallback)(const std::string& str)) :
	FiniteAutomatonTmpl <std::string,Nodes::NodeValue,NVP_States>()
	,__iovalue(v)
{
	__init__();
	__broadcaster = broadcaster;
	__enumcallback = enumcallback;
	Nodes::NodeValue cpy = giveInput(input);
	__iovalue.copy(&cpy);

}

void NodeValueParser::__init__()
{
	__broadcaster =0;
	__enumcallback =0;

	 __vo_Start	= 0xFFFF;
	 __vo_ABCL 	= 0xF;
	 __vo_K 	= 0x20;
	 __vo_E 	= 0x30;
	 __vo_D 	= 0xF;
	 __vo_F 	= 0xF;
	 __vo_J 	= 0xF;
	 __vo_GHI 	= 0xF;

	 uint8_t __t_start =
			 Automaton::AbstractState::START | Automaton::AbstractState::INTER;

	 uint8_t __t_final = Automaton::AbstractState::FINAL;
	 uint8_t __t_inter = Automaton::AbstractState::INTER;
	 uint8_t __t_infin = Automaton::AbstractState::FINAL|Automaton::AbstractState::INTER;

	 registerState(NVP_START, new NVP_State(
			 this
			 ,__t_start
			 ,NVP_START
	 	 	 ,__vo_Start
	 	 	 ,&getMoguType));

	 registerState(NVP_ABCL_F,new NVP_State(
			 this
			 ,__t_final
			 ,NVP_ABCL_F
			 ,__vo_ABCL
			 ,&getMoguType));

	 registerState(NVP_K_T, new NVP_State(
			 this
			 ,__t_inter
			 ,NVP_K_T
			 ,__vo_K
			 ,&getMoguType));

	 registerState(NVP_D_F, new NVP_State(
			 this
			 ,__t_final
			 ,NVP_D_F
			 ,__vo_D
			 ,&getMoguType));

	 registerState(NVP_E_TF, new NVP_State(
			 this
			 ,__t_infin
			 ,NVP_E_TF
			 ,__vo_E
			 ,&getMoguType));

	 registerState(NVP_F_TF, new NVP_State(
			 this
			 ,__t_infin
			 ,NVP_F_TF
			 ,__vo_F
			 ,&getMoguType));

	 registerState(NVP_J_TF, new NVP_State(
			 this
			 ,__t_infin
			 ,NVP_J_TF
			 ,__vo_J
			 ,&getMoguType));

	 registerState(NVP_GHI_F, new NVP_State(
			 this
			 ,__t_final
			 ,NVP_GHI_F
			 ,__vo_GHI
			 ,&getMoguType));

	 getState <NVP_State> (NVP_START)
		.registerDestination(string_value, getState(NVP_ABCL_F))
		.registerDestination(integer_value, getState(NVP_ABCL_F))
		.registerDestination(float_value, getState(NVP_ABCL_F))
		.registerDestination(hashed_string, getState(NVP_ABCL_F))
		.registerDestination(widget_state,getState(NVP_K_T))
		.registerDestination(enum_value,getState(NVP_E_TF))
		.registerDestination(dynamic_storage,getState(NVP_F_TF))
		.registerDestination(data_node, getState(NVP_J_TF))
	;

	getState <NVP_State> (NVP_K_T)
		.registerDestination(widget_state, getState(NVP_D_F))
	;

	getState <NVP_State> (NVP_E_TF)
		.registerDestination(widget_state, getState(NVP_D_F))
	;

	getState <NVP_State> (NVP_F_TF)
		.registerDestination(string_value, getState(NVP_GHI_F))
		.registerDestination(integer_value, getState(NVP_GHI_F))
		.registerDestination(hashed_string, getState(NVP_GHI_F))
	;

	getState <NVP_State> (NVP_J_TF)
		.registerDestination(string_value, getState(NVP_GHI_F))
		.registerDestination(integer_value, getState(NVP_GHI_F))
		.registerDestination(hashed_string, getState(NVP_GHI_F))
	;
}

Nodes::NodeValue NodeValueParser::giveInput(const std::string& input)
{
	__tokenizer.newString(input).reset();
	NVP_States current_state = NVP_START;
	Nodes::NodeValue final_output;
	Outputs token_type;
	do {
		std::string next_token = __tokenizer.next();
		if ((current_state == NVP_START) && (next_token == "!DNP!"))
		{
			final_output.setString(__tokenizer.getRemaining());
			return final_output;
		}
		NVP_State& state = getState <NVP_State>(current_state);
		__status = state.input(next_token);
		if (__status >= OK_ICPL)
		{
			tokens.push_back(next_token);
			tokenTypes.push_back(state.getOutputType());
		}
		if (__status == OK_ICPL)
		{
			token_type = state.getOutputType();
			NVP_State* next_state = (NVP_State*) state.destination(token_type);
			current_state = next_state->getID();
		}
	} while (hasNextToken() && ( __status == OK_ICPL));

	if (__status <= ERR_ECPL)
	{
		/*TODO handle error! */
		return final_output;
	}

	switch(current_state)
	{
	case NVP_ABCL_F:
		parse_single_static_token(
				final_output
				, tokens[0]
				, token_type
				, __broadcaster
				, __enumcallback);
		break;

	case NVP_D_F: //cascades on purpose!
	case NVP_E_TF:
		parse_widget_state(
				final_output
				,tokens[0]
				,tokens[1]
				,__broadcaster);
		break;
	case NVP_F_TF:
		parse_session_node(
				final_output
				,tokens[0]);
		break;
	case NVP_J_TF:
		parse_data_node(
				final_output
				,tokens[0]);
		break;
	case NVP_GHI_F:
		if (tokens[0].at(0) == '@')
			parse_data_node(final_output, tokens[0], tokens[1]);
		else if (tokens[0].at(0) == '[')
			parse_session_node(final_output, tokens[0], tokens[1]);
		break;
	default:
		/*TODO throw error.*/
		break;
	}

	return final_output;
}


}//namespace Parses
