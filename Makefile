SRCS	:= udpserver.cpp Info.cpp\

OBJS	=	${SRCS:.cpp=.o}


FLAGS	:=	-g -fsanitize=address -Wall -Wextra -Werror #-std=c++98

NAME	:=	kalman

.SUFFIXES: .cpp .o


.cpp.o:
		c++ ${FLAGS} -c $< -o ${<:.cpp=.o}

all: ${NAME}

clean:
		rm -rf ${OBJS}
fclean:		clean
		rm -f ${NAME}

${NAME}: ${OBJS}
		c++ ${FLAGS} -o ${NAME} ${OBJS}

bonus: ${OBJB}


re:		fclean all

run: all
	./kalman

.PHONY:		all clean fclean re