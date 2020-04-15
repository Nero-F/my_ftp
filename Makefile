BLUE	=		\033[36m

WHITE	=		\033[0m

CC		=		gcc

RM		=		rm -rf

SRC		=		src/main.c			\
				src/get_next_line.c	\
				src/my_ftp.c	\
				src/server-pi/client_connexion.c	\
				src/server-pi/linked_list_ut.c	\
				src/server-pi/server_pi.c	\
				src/ftp-commands/auth.c	\
				src/ftp-commands/retr.c	\
				src/ftp-commands/stor.c	\
				src/ftp-commands/directory.c	\
				src/ftp-commands/data_transfert_init.c	\

OBJ		=		$(SRC:%.c=$(BUILD_DIR)/%.o)

BUILD_DIR	=	build

override CFLAGS		+=	-Wall -Wextra -Werror
override CPPFLAGS	+=	-I./include
override DEPFLGAS	+=	-MT $@ -MMD -MP -MF $(@:.o=.d)

NAME	=		myftp

$(NAME):	$(OBJ)
	$(CC) $^ $(CFLAGS) -o $@ 

all:	$(NAME) ## Build the binary and relinks if needed

clean: ## Delete the relocatable files
	$(RM) $(BUILD_DIR)

fclean: clean ## Delete the binary file and execute the above rule
	$(RM) $(NAME)

re: fclean all ## Executes an fclean and rebuild

.SECONDEXPANSION:

$(BUILD_DIR)/%.o:	%.c | $$(@D)/.
	$(CC) $(CPPFLAGS) $(DEPFLAGS) -c $< -o $@

$(BUILD_DIR)/.:
	mkdir $@

$(BUILD_DIR)%/.:
	mkdir -p $@

.PRECIOUS: $(BUILD_DIR)/. $(BUILD_DIR)%/.
-include $(OBJ:.o=.d)

help: ## Display this help
	@printf "USAGE:\n\n"
	@grep -E "^[a-zA-Z\\_]+:.*##.*" $(MAKEFILE_LIST) | awk -F ":.*##" '{printf "$(BLUE)%-25s$(WHITE)%s\n", $$1, $$2}'
.PHONY: all clean fclean re
