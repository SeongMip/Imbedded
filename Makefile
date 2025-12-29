NAME        := rtos_sim
CC          := gcc

SRC_DIR     := src
INC_DIR     := include
BUILD_DIR   := build
TEST_DIR    := tests
TEST_BIN    := test_policy

MODE ?= debug

CFLAGS_COMMON := -I$(INC_DIR) -Wall -Wextra -Werror -std=c11 -D_POSIX_C_SOURCE=200809L
LDFLAGS       := -pthread

ifeq ($(MODE),release)
	CFLAGS := $(CFLAGS_COMMON) -O2 -DNDEBUG
else
	CFLAGS := $(CFLAGS_COMMON) -O0 -g3 -DDEBUG
endif

CFLAGS += -MMD -MP

SRCS := \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/system.c \
	$(SRC_DIR)/timebase.c \
	$(SRC_DIR)/task.c \
	$(SRC_DIR)/queue.c \
	$(SRC_DIR)/state.c \
	$(SRC_DIR)/policy.c \
	$(SRC_DIR)/error.c \
	$(SRC_DIR)/safety.c \
	$(SRC_DIR)/control.c \
	$(SRC_DIR)/time_delay.c \
	$(SRC_DIR)/task_input.c \
	$(SRC_DIR)/task_control.c \
	$(SRC_DIR)/task_output.c \
	$(SRC_DIR)/task_watchdog.c

OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

.PHONY: all clean fclean re run info test

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

run: $(NAME)
	./$(NAME)

test:
	$(CC) $(CFLAGS_COMMON) -O0 -g3 \
		$(TEST_DIR)/test_policy.c \
		$(SRC_DIR)/policy.c \
		$(SRC_DIR)/state.c \
		$(SRC_DIR)/error.c \
		-o $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -rf $(BUILD_DIR)

fclean: clean
	rm -f $(NAME) $(TEST_BIN)

re: fclean all

info:
	@echo "NAME:   $(NAME)"
	@echo "MODE:   $(MODE)"
	@echo "CFLAGS: $(CFLAGS)"
