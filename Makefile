NAME = mrmkcs
DEBUG = -DDEBUG -DDEBUG_STDERR -DMALLOC_DEBUG

MACOS_VER = 10.14
CPPFLAGS += -std=c++17 -mmacosx-version-min=$(MACOS_VER) -MD -MP -g $(DEBUG)

LIBS = -lc -lc++ -lncurses -lportmidi
LDFLAGS += $(LIBS) -macosx_version_min $(MACOS_VER)

prefix = /usr/local
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin

SRC = $(wildcard src/*.cpp) $(wildcard src/curses/*.cpp)
OBJS = $(SRC:%.cpp=%.o)
TEST_SRC = $(wildcard test/*.cpp)
TEST_OBJS = $(TEST_SRC:%.cpp=%.o)
TEST_OBJ_FILTERS = src/main.o

CATCH_CATEGORY ?= ""

.PHONY: all test install uninstall tags clean distclean

all: $(NAME)

$(NAME): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

-include $(SRC:%.cpp=%.d)
-include $(TEST_SRC:%.cpp=%.d)

test: $(NAME)_test
	./$(NAME)_test --use-colour no $(CATCH_CATEGORY)

$(NAME)_test:	$(OBJS) $(TEST_OBJS)
	$(LD) $(LDFLAGS) -o $@ $(filter-out $(TEST_OBJ_FILTERS),$^)

install:	$(bindir)/$(NAME)

$(bindir)/$(NAME):	$(NAME)
	cp ./$(NAME) $(bindir)
	chmod 755 $(bindir)/$(NAME)

uninstall:
	rm -f $(bindir)/$(name)

tags:	TAGS

TAGS:	$(SRC)
	etags $(SRC)

clean:
	rm -f $(NAME) $(NAME)_test src/*.o src/curses/*.o test/*.o

distclean: clean
	rm -f src/*.d src/curses/*.d test/*.d
