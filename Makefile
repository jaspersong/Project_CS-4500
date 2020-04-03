build_all:
	mkdir -p build
	cd ./build; cmake ..
	cd ./build; make

test_all: build_all
	-cd ./build; ./test_suite_queue && echo "SUCCESS"
	-cd ./build; ./test_suite_dataframe && echo "SUCCESS"
	-cd ./build; ./test_suite_messages && echo "SUCCESS"
	-cd ./build; ./test_suite_network && echo "SUCCESS"
	-cd ./build; ./test_suite_sorer && echo "SUCCESS"
	-cd ./build; ./test_suite_kv_store && echo "SUCCESS"
	-cd ./build; ./demo_trivial_app_nonet && echo "SUCCESS"
	-cd ./build; ./demo_app_nonet && echo "SUCCESS"
	-cd ./build; ./demo_wordcount_nonet ../data/shakespeare.txt && echo "SUCCESS"

test_valgrind: build_all
	-cd ./build; valgrind --leak-check=yes ./test_suite_queue
	-cd ./build; valgrind --leak-check=yes ./test_suite_dataframe
	-cd ./build; valgrind --leak-check=yes ./test_suite_messages
	-cd ./build; valgrind --leak-check=yes ./test_suite_network
	-cd ./build; valgrind --leak-check=yes ./test_suite_sorer
	-cd ./build; valgrind --leak-check=yes ./test_suite_kv_store
	-cd ./build; valgrind --leak-check=yes ./demo_trivial_app_nonet
	-cd ./build; valgrind --leak-check=yes ./demo_app_nonet
	-cd ./build; valgrind --leak-check=yes ./demo_wordcount_nonet ../data/shakespeare.txt

demo_demo_net_app: build_all
	-cd ./build; ./demo_app_wnet

demo_wordcount_net_app: build_all
	-cd ./build; ./demo_wordcount_wnet ../data/shakespeare.txt

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
