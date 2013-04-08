
#ifndef SYNTAX_H_
#define SYNTAX_H_

enum class MoguSyntax {
	__NONE__                               = 0
	,own                                    = 1
	,parent                                 = 2
	,children                               = 3
	,siblings                               = 4
	,perspective                            = 5
	,click                                  = 6
	,mouseover                              = 7
	,style_changed                          = 8
	,keyup                                  = 9
	,fail                                  = 10
	,succeed                               = 11
	,enter_pressed                         = 12
	,hidden_changed                        = 13
	,index_changed                         = 14
	,onload                                = 15
	,clear                                 = 16
	,increment                             = 17
	,decrement                             = 18
	,test                                  = 19
	,tooltip                               = 20
	,email                                 = 21
	,reload                                = 22
	,append                                = 23
	,replace                               = 24
	,set                                   = 25
	,path                                  = 26
	,user                                  = 27
	,session                               = 28
	,reset                                 = 29
	,style                                 = 30
	,group                                 = 31
	,remove_                                = 32
	,text                                  = 33
	,store                                 = 34
	,child                                 = 35
	,regex                                 = 36
	,list                                  = 37
	,string                                = 38
	,hash                                  = 39
	,data                                  = 40
	,index                                 = 41
	,slot                                  = 42
	,javascript                            = 43
	,anchor                                = 44
	,image                                 = 45
	,submit                                = 46
	,radio                                 = 47
	,textarea                              = 48
	,input                                 = 49
	,dropdown                              = 50
	,multi                                 = 51
	,container                             = 52
	,password                              = 53
	,stack                                 = 54
	,foreach                               = 55
	,name                                  = 56
	,hidden                                = 57
	,image_link                            = 58
	,to                                    = 59
	,as                                    = 60
	,at                                    = 61
	,m_widget                              = 62
	,retrieve                              = 63
	,type                                  = 64
	,mode                                  = 65
	,source                                = 66
	,location                              = 67
	,template_                              = 68
	,OPER_OPPAREN				=2147483642
	,OPER_CLPAREN				=2147483643
	,OPER_PLUS				=2147483644
	,OPER_MINUS				=2147483645
	,OPER_MULT				=2147483646
	,OPER_DIV				=2147483647

}; // END MoguSyntax
#endif //SYNTAX_H_