all:
	g++ -std=c++0x -W -Wall -Wextra -pedantic MP_test.cpp TokenManager.cpp MathParser.cpp

tm:
	g++ -std=c++0x -W -Wall -Wextra -pedantic TM_test.cpp TokenManager.cpp

parsers:
	g++ -std=c++0x -W -Wall -Wextra -pedantic test.cpp ../../Moldable/Moldable.cpp ../../Types/NodeValue.cpp ../NodeValueParser.cpp StateParser.cpp MathParser.cpp -lturnleft -lwt -lhiredis -lboost_signals
