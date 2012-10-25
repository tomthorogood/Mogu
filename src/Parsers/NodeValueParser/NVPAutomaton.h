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


namespace Parsers{


enum NVP_States
{
	NVP_START
	,NVP_ABCL_F
	,NVP_K_T
	,NVP_D_F
	,NVP_E_TF
	,NVP_F_TF
	,NVP_J_TF
	,NVP_GHI_F
};

class NodeValueParser: public Automaton::FiniteAutomatonTmpl
  <std::string, Nodes::NodeValue, NVP_States>
{

public:
	enum Outputs {
		string_value	=0x1	//1
		,integer_value	=0x2	//2
		,float_value	=0x4	//4
		,hashed_string	=0x8	//8
		,enum_value		=0x10	//16
		,widget_state	=0x20	//32
		,dynamic_storage=0x40	//64
		,data_node		=0x80	//128
		,registered_node=0x100	//256
	};

	NodeValueParser(
			std::string input
			,Nodes::NodeValue& v
			,Goo::Moldable* broadcaster =0
			,int (*enumparser)(const std::string&) =0);

	Nodes::NodeValue giveInput (const std::string& input);

	~NodeValueParser()
	{
		std::map<NVP_States,Automaton::AbstractState*>::iterator iter
			= __states.begin();
		while (iter != __states.end())
		{
			delete iter->second;
			++iter;
		}
	}

	inline bool hasNextToken()
	{
		return __tokenizer.hasNext();
	}

private:
	uint16_t __vo_Start;
	uint16_t __vo_ABCL;
	uint16_t __vo_K;
	uint16_t __vo_E;
	uint16_t __vo_D;
	uint16_t __vo_F;
	uint16_t __vo_J;
	uint16_t __vo_GHI;

	Parsers::MoguScript_Tokenizer __tokenizer;
	std::vector <std::string> tokens;
	std::vector <NodeValueParser::Outputs> tokenTypes;
	Nodes::NodeValue& __iovalue;
	void __init__();
	Goo::Moldable* __broadcaster;
	int (*__enumcallback)(const std::string&);
};

using Automaton::State;
using Automaton::FiniteAutomaton;

class NVP_State : public State
	<NodeValueParser::Outputs, std::string, NVP_States, uint16_t>
{
public:
	NVP_State
		( FiniteAutomaton* NVP_Instance
			,uint8_t state_type
			,const NVP_States& name
			,const uint16_t& valid_outputs
			,NodeValueParser::Outputs (*callback)(const std::string&))
	: State <NodeValueParser::Outputs, std::string, NVP_States, uint16_t>
	  (NVP_Instance
			, state_type
			, name
			, valid_outputs
			, callback){}
};

NodeValueParser::Outputs getMoguType(const std::string& token);


/* For enums, ints, floats, strings, hashed strings */
void parse_single_static_token (
		Nodes::NodeValue& v
		,std::string& input
		,NodeValueParser::Outputs tokentype
		,Goo::Moldable* broadcaster =0
		,int(*enumcallback)(const std::string&)=0
	);

void parse_session_node (
		Nodes::NodeValue& v
		,std::string& base
		,std::string arg=EMPTY
		,NodeValueParser::Outputs arg_type = NodeValueParser::string_value
		);

void parse_data_node (
		Nodes::NodeValue& v
		,std::string& base
		,std::string arg=EMPTY
		,NodeValueParser::Outputs arg_type = NodeValueParser::string_value
		);


void parse_widget_state (
		Nodes::NodeValue& v
		,std::string& widget_identifier
		,std::string& state
		,Goo::Moldable* broadcaster=0
		);

}//namespace Parser
#endif /* NVPAUTOMATON_H_ */
