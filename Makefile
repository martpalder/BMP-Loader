build:
	gcc src/main.c -o bin/BMP-Loader

clean:
	del bin\BMP-Loader.exe

debug:
	gdb bin/BMP-Loader

run:
	bin/BMP-Loader