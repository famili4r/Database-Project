TARGET = bin/app

SRC = $(wildcard src/*.c)

OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))



default: $(TARGET)

run: clean default
	./bin/app -n -f mydb.db


clean:
	rm -f mydb.db
	rm -f obj/*.o
	rm -f bin/*



$(TARGET): $(OBJ)

	gcc -o $@ $?



obj/%.o : src/%.c

	gcc -c $< -o $@ -Iinclude