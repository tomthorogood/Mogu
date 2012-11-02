#include <declarations.h>

enum MoguToken {
	NONE			=0x0
	,self			=0x1
	,parent			=0x2
	,children		=0x3
	,siblings		=0x4
    ,application	=0x5
	,change			=0x6
	,add			=0x7
	,subtract		=0x8
	,multiply		=0x9
	,divide			=0xa
	,remove			=0xb
	,clear			=0xc
	,increment		=0xd
	,decrement		=0xe
	,match			=0xf
	,email			=0x10
	,reload			=0x11
	,append			=0x12
	,replace		=0x13
	,slide			=0x14
	,pop			=0x15
	,fade			=0x16
	,emit			=0x17
	,count			=0x18
	,right			=0x19
	,left			=0x1a
	,up			    =0x1b
	,down			=0x1c
	,regex			=0x1d
	,list			=0x1e
	,string			=0x1f
	,hash			=0x20
	,style			=0x21
	,index			=0x22
	,session		=0x23
	,user			=0x24
	,slot			=0x25
	,javascript		=0x26
	,value			=0x27
	,link			=0x28
	,image			=0x29
	,submit			=0x2a
	,radio			=0x2b
	,textarea		=0x2c
	,input			=0x2d
	,dropdown		=0x2e
	,checkbox		=0x2f
	,multi			=0x30
	,container		=0x31
	,password		=0x32
	,stack			=0x33
	,foreach		=0x34
	,index			=0x35
	,name			=0x36
	,hidden			=0x37
	,value			=0x38
};


static std::map<std::string,MoguToken> tokenParser = create_map <std::string, MoguToken> ()
	("NONE")(NONE)
	("self")(self)
	("parent")(parent)
	("children")(children)
	("siblings")(siblings)
	("application")(application)
	("change")(change)
	("add")(add)
	("subtract")(subtract)
	("multiply")(multiply)
	("divide")(divide)
	("remove")(remove)
	("clear")(clear)
	("increment")(increment)
	("decrement")(decrement)
	("match")(match)
	("email")(email)
	("reload")(reload)
	("append")(append)
	("replace")(replace)
	("slide")(slide)
	("pop")(pop)
	("fade")(fade)
	("emit")(emit)
	("count")(count)
	("right")(right)
	("left")(left)
	("up")(up)
	("down")(down)
	("regex")(regex)
	("list")(list)
	("string")(string)
	("hash")(hash)
	("style")(style)
	("index")(index)
	("session")(session)
	("user")(user)
	("slot")(slot)
	("javascript")(javascript)
	("value")(value)
	("link")(link)
	("image")(image)
	("submit")(submit)
	("radio")(radio)
	("textarea")(textarea)
	("input")(input)
	("dropdown")(dropdown)
	("checkbox")(checkbox)
	("multi")(multi)
	("container")(container)
	("password")(password)
	("stack")(stack)
	("foreach")(foreach)
	("index")(index)
	("name")(name)
	("hidden")(hidden)
	("value")(value)
;
static std::map<MoguToken, std::string> reverseTokenParser = create_map <MoguToken,std::string> ()
	(NONE)("NONE")
	(self)("self")
	(parent)("parent")
	(children)("children")
	(siblings)("siblings")
	(application)("application")
	(change)("change")
	(add)("add")
	(subtract)("subtract")
	(multiply)("multiply")
	(divide)("divide")
	(remove)("remove")
	(clear)("clear")
	(increment)("increment")
	(decrement)("decrement")
	(match)("match")
	(email)("email")
	(reload)("reload")
	(append)("append")
	(replace)("replace")
	(slide)("slide")
	(pop)("pop")
	(fade)("fade")
	(emit)("emit")
	(count)("count")
	(right)("right")
	(left)("left")
	(up)("up")
	(down)("down")
	(regex)("regex")
	(list)("list")
	(string)("string")
	(hash)("hash")
	(style)("style")
	(index)("index")
	(session)("session")
	(user)("user")
	(slot)("slot")
	(javascript)("javascript")
	(value)("value")
	(link)("link")
	(image)("image")
	(submit)("submit")
	(radio)("radio")
	(textarea)("textarea")
	(input)("input")
	(dropdown)("dropdown")
	(checkbox)("checkbox")
	(multi)("multi")
	(container)("container")
	(password)("password")
	(stack)("stack")
	(foreach)("foreach")
	(index)("index")
	(name)("name")
	(hidden)("hidden")
	(value)("value")
;
