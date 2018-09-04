NAME = part_sys
FLAGS = -std=c++11 -Wall -Wextra -Werror -g -O2 -std=gnu++0x


INCLUDE = -I includes \
		-I ${HOME}/.brew/Cellar/glfw/3.2.1/include \
		-I ${HOME}/.brew/Cellar/glew/2.1.0/include
SRCSPATH = srcs/

SRCS = 	main.cpp \
		Display.cpp \
		ParticleGenerator.cpp \
		Shader.cpp \
		Camera.cpp \
		glad.cpp \
		ClContext.cpp \

LIBS =  -L ${HOME}/.brew/Cellar/glfw/3.2.1/lib -lglfw  \
		-L ${HOME}/.brew/Cellar/glew/2.1.0/lib -lGLEW  \
		-framework OpenGL  \
		-framework OpenCl  \

OBJ= $(SRC:.cpp=.o) 

CC = clang++

SRC = $(addprefix $(SRCSPATH), $(SRCS))

%.o: %.cpp
	$(CC) $(INCLUDE) -c $(FLAGS)  $< -o $@ 

all: $(NAME)

$(NAME): $(OBJ)
	$(CC)  $(FLAGS) $(INCLUDE) -o $(NAME) $(LIBS) $(OBJ) 

clean:
	rm -f $(OBJ)

fclean:
	rm -f $(NAME) $(OBJ)

re: fclean all

.PHONY: all clean fclean re
