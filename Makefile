NAME = minishell

#HELLO 	WORLD

CC = cc
CFLAGS = -Wall -Wextra -Werror -g

SRC = src/main.c	src/Handle_command.c	src/Handle_token.c	src/Helper.c  src/prompt.c	src/tools.c	src/meta_char.c	src/utils.c	src/excute.c  src/parse_herdoc.c	\
			src/garbage_collector.c src/manual_realloc.c	src/open_redirection.c	src/set_global_value.c	src/exit_command.c	src/dup_parent_and_child.c	src/lexer.c  src/enviroment.c src/cd.c src/export_unset.c\
			src/meta_char_utils.c	src/g_redir_err.c	src/lexer_split.c	src/lexer_parser.c	src/lexer_utils.c	src/token.c	src/lexer_helpers.c	src/garbage_collecter_utils.c	src/clean_up.c	src/herdoc_utils.c	src/parse_cmd_utils.c\
			src/parse_helper.c	src/excute_utils.c	src/main_execution.c	src/main_utils.c	src/main_helper.c	src/token_to_command.c	src/token_to_cmd_utils.c src/print_export_env.c src/export_helpers.c src/pwd_cmd.c \
			src/identifier_utils.c src/env_sort_utils.c src/env_var_utils.c src/export_without_value.c src/execution_helpers.c src/pipeline_execution.c src/child_execution_helpers.c src/pipeline_setup_utils.c \
			src/pipeline_utils_clean.c src/fd_cleanup.c	src/print_message.c   src/true_or_false.c \
			src/dup_parent_and_child_utils.c   src/pipeline_utils.c src/exported_vars.c	src/manage_cwd.c	src/open_files.c	src/prompt_utils.c\


LIBFT_SRC = my_libft/ft_isdigit.c my_libft/ft_isalnum.c my_libft/ft_isascii.c my_libft/ft_isprint.c my_libft/ft_strlen.c my_libft/ft_memset.c my_libft/ft_isalpha.c \
	my_libft/ft_bzero.c my_libft/ft_memcpy.c my_libft/ft_memmove.c my_libft/ft_strlcpy.c my_libft/ft_strlcat.c my_libft/ft_toupper.c my_libft/ft_tolower.c \
	my_libft/ft_strchr.c my_libft/ft_strrchr.c my_libft/ft_strncmp.c my_libft/ft_strcmp.c my_libft/ft_memchr.c my_libft/ft_memcmp.c my_libft/ft_strnstr.c my_libft/ft_atoi.c \
	my_libft/ft_calloc.c my_libft/ft_strdup.c my_libft/ft_substr.c my_libft/ft_strjoin.c my_libft/ft_strtrim.c my_libft/ft_split.c my_libft/ft_itoa.c \
	my_libft/ft_putchar_fd.c my_libft/ft_putendl_fd.c my_libft/ft_putnbr_fd.c my_libft/ft_putstr_fd.c my_libft/ft_strmapi.c  \
	my_libft/ft_lstnew_bonus.c my_libft/ft_lstadd_back_bonus.c my_libft/ft_lstadd_front_bonus.c my_libft/ft_lstsize_bonus.c \
	my_libft/ft_lstclear_bonus.c my_libft/ft_lstlast_bonus.c my_libft/ft_lstdelone_bonus.c my_libft/ft_lstiter_bonus.c

OBJ = $(SRC:.c=.o)
LIBFT_OBJ = $(LIBFT_SRC:.c=.o)

# Colors
GREEN = \033[0;32m
YELLOW = \033[1;33m
BLUE = \033[1;34m
RED = \033[0;31m
NC = \033[0m

# Spinner
SPINNER = spin() { \
	sp="/-\|"; \
	while kill -0 $$1 2>/dev/null; do \
		printf "\r[$(YELLOW)$$sp$(NC)] Building..."; \
		sp=$${sp#?}$${sp%???}; \
		sleep 0.1; \
	done; \
	printf "\r[$(GREEN)✔$(NC)] Build complete!         \n"; \
}

all: $(NAME)

$(NAME): $(OBJ) $(LIBFT_OBJ)
	@echo "$(BLUE)🔧 Compiling $(NAME)...$(NC)"
	@$(CC) $(CFLAGS) $(OBJ) $(LIBFT_OBJ) -o $(NAME) -lreadline
	@echo "$(GREEN)✅ $(NAME) built successfully!$(NC)"

clean:
	@rm -f $(OBJ) $(LIBFT_OBJ)
	@echo "$(YELLOW)🧹 Object files cleaned.$(NC)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(RED)🧨 Executable removed.$(NC)"

re: fclean all

# Valgrind leak testing targets
valgrind-quick:
	@echo "$(BLUE)🔍 Running quick valgrind leak test...$(NC)"
	@./quick_valgrind_test.sh

valgrind-full:
	@echo "$(BLUE)🔍 Running comprehensive valgrind leak test...$(NC)"
	@./test_valgrind_leaks.sh

valgrind-comprehensive:
	@echo "$(BLUE)🔍 Running enhanced comprehensive valgrind leak test...$(NC)"
	@./test_valgrind_comprehensive.sh

valgrind-check: $(NAME)
	@echo "$(BLUE)🔍 Quick valgrind check...$(NC)"
	@echo "echo 'test'; exit" | valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes --suppressions=readline.sup ./$(NAME)

valgrind-analyze:
	@echo "$(BLUE)📊 Analyzing valgrind results...$(NC)"
	@./simple_analyzer.sh

help-valgrind:
	@echo "$(YELLOW)Valgrind Testing Targets:$(NC)"
	@echo "  valgrind-quick         : Run quick leak tests (~30 tests)"
	@echo "  valgrind-full          : Run comprehensive leak tests (~56 tests)"
	@echo "  valgrind-comprehensive : Run enhanced comprehensive tests (~200+ tests)"
	@echo "  valgrind-check         : Single quick valgrind check"
	@echo "  valgrind-analyze       : Analyze previous test results"
	@echo "  help-valgrind          : Show this help"
	@echo ""
	@echo "$(BLUE)Test Coverage:$(NC)"
	@echo "  • Built-in commands (echo, pwd, cd, export, unset, env)"
	@echo "  • External commands (ls, cat, grep, wc, sort, etc.)"
	@echo "  • Simple and multiple pipes"
	@echo "  • Input/output redirections"
	@echo "  • Heredoc functionality"
	@echo "  • Variable expansion"
	@echo "  • Error handling and edge cases"
	@echo "  • Signal handling"
	@echo "  • Quote handling"
	@echo "  • Wildcard patterns"
	@echo ""
	@echo "$(BLUE)See VALGRIND_TESTING_README.md for detailed usage$(NC)"

.PHONY: all clean fclean re valgrind-quick valgrind-full valgrind-comprehensive valgrind-check valgrind-analyze help-valgrind
.INTERMEDIATE: $(LIBFT_OBJ) $(OBJ)