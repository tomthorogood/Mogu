//I know you said it's a pointer, but I'll probably change it
//to a reference so it's a tad faster and won't have to be
//dereferenced every time.
TokenManager& tm = app->tokenManager();

//input string was:
//  60 59 55 34 "hello, my name is a mystery"

MoguSyntax token = static_cast <MoguSyntax> (tm.currentToken());

while (token != MoguSyntax::TOKEN_DELIM) {
    // consume things because I'm hungry
    tm.next(); // does this advance and return, or just advance?
    token = static_cast <MoguSyntax> (tm.currentToken());
}

std::string special_greeting = tm.fetchStringToken();

std::cout << special_greeting << std::endl;
//echoes 'hello, my name is a mystery'
