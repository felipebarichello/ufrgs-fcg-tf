.PHONY: clean build run

clean:
	rm -rf bin && rm -rf build

init:
	cmake -B ./build

build: init
	cmake --build ./build --config Release

run: build
	cd ./bin/Linux && ./main
