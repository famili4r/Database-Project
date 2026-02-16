TARGET = bin/app

SRC = $(wildcard src/*.c)

OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))



default: $(TARGET)

run: clean default
	./$(TARGET) -n -f mydb.db
	./$(TARGET) -f ./mydb.db -a "Test Tester,Teststreet 1,40"
	./$(TARGET) -f ./mydb.db -a "Hemuli,Muumilaakso,120"
	./$(TARGET) -f ./mydb.db -a "Kimmo Koodari,kotikatu 1,20"

clean:
	rm -f mydb.db
	rm -f obj/*.o
	rm -f bin/*



$(TARGET): $(OBJ)

	gcc -o $@ $?



obj/%.o : src/%.c

	gcc -c $< -o $@ -Iinclude