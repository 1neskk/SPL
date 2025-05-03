.PHONY: build
build:
	mkdir -p build
	echo "Building project in Release mode..."
	cd build && \
	cmake -DCMAKE_BUILD_TYPE=release .. && \
	make

.PHONY: debug
debug:
	mkdir -p build
	echo "Building project in Debug mode..."
	cd build && \
	cmake .. -DCMAKE_BUILD_TYPE=Debug && \
	make

.PHONY: clean
clean:
	rm -rf build

.PHONY: format
format:
	clang-format src/* src/* -i
