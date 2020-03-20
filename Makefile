build_all:
	mkdir -p build
	cd ./build; cmake ..
	cd ./build; make

test_all: build_all
	-cd ./build; ./test_suite_queue
	-cd ./build; ./test_suite_map
	-cd ./build; ./test_suite_dataframe
	-cd ./build; ./test_suite_messages
	-cd ./build; ./test_suite_network
	-cd ./build; ./test_suite_sorer

test_valgrind: build_all
	-cd ./build; valgrind ./test_suite_map
	-cd ./build; valgrind ./test_suite_queue
	-cd ./build; valgrind ./test_suite_dataframe
	-cd ./build; valgrind ./test_suite_messages
	-cd ./build; valgrind ./test_suite_network
	-cd ./build; valgrind ./test_suite_sorer

demo_distro_app: build_all
	cd ./build; ./demo_registrar & ./demo_node hello world

clean:
	rm -rf ./build

docker_build_all:
	docker build -t snowy_song:0.1 .
	docker run -ti -v `pwd`:/test snowy_song:0.1 bash -c "cd /test; make build_all"

docker_test_all: docker_build_all
	docker run -ti -v `pwd`:/test snowy_song:0.1 bash -c "cd /test; make test_all"

docker_test_valgrind: docker_build_all
	docker run -ti -v `pwd`:/test snowy_song:0.1 bash -c "cd /test; make test_valgrind"
