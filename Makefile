VERSION = 1.0
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -g -Os
LIBS = -pthread
ifeq ($(OS),Windows_NT)
    LIBS += -lws2_32
    SOCKET_SRC = core/socket/win.c
else
    SOCKET_SRC = core/socket/posix.c
endif

Q = $(if $(filter $(V), 1), ,@)

O = out
D = $(O)/.deps
DIRS = $(O)/core/socket $(O)/utils/logger $(D)/core/socket $(D)/utils/logger

SRC_COMMON = utils/logger/logger.c $(SOCKET_SRC)
CLIENT_SRC = client.c $(SRC_COMMON)
SERVER_SRC = server.c $(SRC_COMMON)

CLIENT_OBJ = $(CLIENT_SRC:%.c=$(O)/%.o)
SERVER_OBJ = $(SERVER_SRC:%.c=$(O)/%.o)
DEP = $(CLIENT_SRC:%.c=$(D)/%.d) $(SERVER_SRC:%.c=$(D)/%.d)

CLIENT = $(O)/client
SERVER = $(O)/server

.PHONY: all clean

all: $(CLIENT) $(SERVER)

$(O):
	$(Q)mkdir -p $@

$(DIRS):
	$(Q)mkdir -p $@

$(O)/%.o: %.c | $(DIRS)
	$(Q)echo " CC    $@"
	$(Q)$(CC) $(CFLAGS) -MMD -MP -MF $(D)/$*.d -c -o $@ $<

$(CLIENT): $(CLIENT_OBJ)
	$(Q)echo " LD    $@"
	$(Q)$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(SERVER): $(SERVER_OBJ)
	$(Q)echo " LD    $@"
	$(Q)$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	$(Q)echo " CLEAN $(O)"
	$(Q)rm -rf $(O)

-include $(DEP)
