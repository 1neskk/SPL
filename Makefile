.PHONY: build debug clean format

build:
	mkdir -p build
	echo "Building project in Release mode..."
	cd build && \
	cmake -DCMAKE_BUILD_TYPE=release .. && \
	make

debug:
	mkdir -p build
	echo "Building project in Debug mode..."
	cd build && \
	cmake .. -DCMAKE_BUILD_TYPE=Debug && \
	make

clean:
	rm -rf build

format:
	clang-format anvil/src/*.c anvil/include/*.h axiom/src/*.c axiom/include/*.h -i --style=file
