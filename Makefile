all:
	gcc ./src/main.c ./src/js.c ./src/vec.c ./src/sdl_c.c ./src/font.c ./src/buffer.c ./src/buffer_color.c -Wall -Wextra -Wshadow -pedantic -lfreetype -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lfreetype -lpthread -o padd

prod:
	gcc ./src/main.c ./src/js.c ./src/vec.c ./src/sdl_c.c ./src/font.c ./src/buffer.c ./src/buffer_color.c -Wall -Wextra -Wshadow -pedantic -lfreetype -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lfreetype -lpthread -o padd -mwindows
