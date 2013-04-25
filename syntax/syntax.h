
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
	,validator                             = 36
	,regex                                 = 37
	,list                                  = 38
	,string                                = 39
	,hash                                  = 40
	,data                                  = 41
	,index                                 = 42
	,slot                                  = 43
	,javascript                            = 44
	,anchor                                = 45
	,image                                 = 46
	,submit                                = 47
	,radio                                 = 48
	,textarea                              = 49
	,input                                 = 50
	,dropdown                              = 51
	,multi                                 = 52
	,container                             = 53
	,password                              = 54
	,stack                                 = 55
	,foreach                               = 56
	,name                                  = 57
	,hidden                                = 58
	,image_link                            = 59
	,to                                    = 60
	,as                                    = 61
	,at                                    = 62
	,widget                                = 63
	,retrieve                              = 64
	,type                                  = 65
	,mode                                  = 66
	,source                                = 67
	,location                              = 68
	,template_                              = 69
	,OPER_OPPAREN				=2147483642
	,OPER_CLPAREN				=2147483643
	,OPER_PLUS				=2147483644
	,OPER_MINUS				=2147483645
	,OPER_MULT				=2147483646
	,OPER_DIV				=2147483647
	,TOKEN_DELIM				=2147483641

}; // END MoguSyntax
#endif //SYNTAX_H_