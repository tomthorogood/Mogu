
root_dir = $(CURDIR)/../../
GCC := g++ -std=c++0x $(flags) -DNOLOGOVERRIDE
includes := -I$(root_dir)

all: QueryHandler MoguQueryHandler NodeEditor

QueryHandler: obj/QueryHandler.o
	@$(GCC) $(includes) -o $@ $@.Test.cpp $< -lhiredis -lcpptest
	@echo "Created $@ test suite"

NodeEditor: obj/NodeEditor.o
	@$(GCC) $(includes)  -o $@ $@.Test.cpp $< obj/QueryHandler.o \
		$(root_dir)/src/Security/Encryption.cpp \
		$(root_dir)/src/Types/MoguLogger.cpp \
		$(root_dir)/src/Redis/DatabaseConfigReader.cpp \
		$(root_dir)/src/Redis/MoguQueryHandler.cpp \
		$(root_dir)/src/crypt/BlowfishKey.cpp \
		-lhiredis -lcpptest -lcrypto
	@echo "Created $@ test suite"

MoguQueryHandler: obj/MoguQueryHandler.o
	@$(GCC) $(includes)  -o $@ $@.Test.cpp $< obj/QueryHandler.o \
		$(root_dir)/src/Redis/NodeEditor.cpp \
		$(root_dir)/src/Types/MoguLogger.cpp \
		$(root_dir)/src/Redis/DatabaseConfigReader.cpp \
		$(root_dir)/src/Security/Encryption.cpp \
		$(root_dir)/src/crypt/BlowfishKey.cpp \
		-lhiredis -lcpptest -lcrypto
	@echo "Created $@ test suite"

obj/QueryHandler.o: obj
	@$(GCC) $(includes) -c -o $@ $(root_dir)/src/Redis/QueryHandler/QueryHandler.cpp -lhiredis
	@echo "Created $@ object file" 

obj/MoguQueryHandler.o: obj/QueryHandler.o
	@$(GCC) $(includes) -c  -o $@ $(root_dir)/src/Redis/MoguQueryHandler.cpp
	@echo "Created $@ object file" 

obj/NodeEditor.o: obj/QueryHandler.o
	@$(GCC) $(includes) -c  -o $@ $(root_dir)/src/Redis/NodeEditor.cpp 
	@echo "Created $@ object file" 

obj:
	@mkdir -pv $@

test: QueryHandler MoguQueryHandler NodeEditor
	@($(util) ./QueryHandler && \
	$(util) ./MoguQueryHandler && \
	$(util) ./NodeEditor) \
	|| echo "Could not complete tests, because one or more base tests failed."

make clean:
	rm -rf obj/
	rm -f QueryHandler
	rm -f NodeEditor
