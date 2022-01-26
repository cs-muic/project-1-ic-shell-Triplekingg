TARGET=test

make:
	gcc -o test icsh.c
clean:
	rm $(TARGET)