//----------------------------------------
// widget foo will be instantiated and added into the widget tree
// as a child of widget bar
append widget foo to widget bar

// Resulting in:
CommandValue.action     = (MoguSyntax) "append"
CommandValue.object     = (MoguSyntax) "widget"
CommandValue.identifier = "bar"

// This is counterintuitive, I know, but it's the best way I can
// think of to maintain the same data type in this edge case. I'm 
// open to suggestiosn.
CommandValue.arg        = NodeValue((MoguSyntax) "widget")
CommandValue.Value      = NodeValue("foo")

//----------------------------------------
// "foobar" will be added as a css class to a currently 
// instantiated widget. This pattern is valid for any
// 'state'; if 'self' is the object, 'identifier' will be 
// left blank
append "foobar" to widget bar css

CommandValue.action     = (MoguSyntax) "append"
CommandValue.object     = (MoguSyntax) "widget"
CommandValue.identifier = "bar"
CommandValue.arg        = NodeValue((MoguSyntax) "css")
CommdnValue.value       = NodeValue("foobar")

//----------------------------------------
// an entry will be appended to a list node in the Redis database

append "foobar" to user somelist

CommandValue.action     = (MoguSyntax) "append"
CommandValue.object     = (MoguSyntax) "user"
CommandValue.identifier = "somelist"
CommandValue.value      = NodeValue("foobar")
