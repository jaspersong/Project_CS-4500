build_all:
	mkdir -p build
	cd ./build; cmake ..
	cd ./build; make
	cd ./build; cp ../src/dataframe/sorer .

test_all: build_all
	-cd ./build; ./test_suite_array
	-cd ./build; ./test_suite_map
	-cd ./build; ./test_suite_queue
	-cd ./build; ./test_suite_map
	-cd ./build; ./test_suite_dataframe
	-cd ./build; ./test_suite_messages
	-cd ./build; ./test_suite_network
	# TODO: Create Python unit test suite for the sorer tests
	-cd ./build; ./sorer -f "../data/default.sor" -from 0 -len 100print_col_type 0
	-cd ./build; ./sorer -f "../data/default.sor" -from 0 -len 100 -print_col_type 2
	-cd ./build; ./sorer -f "../data/default.sor" -from 0 -len 100 -is_missing_idx 2 0
	-cd ./build; ./sorer -f "../data/default.sor" -from 0 -len 100 -is_missing_idx 2 1
	-cd ./build; ./sorer -f "../data/default.sor" -from 0 -len 100 -is_missing_idx 2 2
	-cd ./build; ./sorer -f "../data/default.sor" -from 0 -len 100 -print_col_idx 2 0
	-cd ./build; ./sorer -f "../data/default.sor" -from 5 -len 100 -print_col_idx 1 0
	-cd ./build; ./sorer -f "../data/testfile.sor" -from 0 -len 100 -print_col_type 0
	-cd ./build; ./sorer -f "../data/testfile.sor" -from 0 -len 100 -print_col_type 2
	-cd ./build; ./sorer -f "../data/testfile.sor" -from 0 -len 100 -print_col_type 1
	-cd ./build; ./sorer -f "../data/testfile.sor" -from 0 -len 100 -print_col_type 3
	-cd ./build; ./sorer -f "../data/testfile.sor" -from 5 -len 100 -print_col_type 0
	-cd ./build; ./sorer -f "../data/testfile.sor" -from 0 -len 100 -print_col_idx 0 1
	-cd ./build; ./sorer -f "../data/testfile.sor" -from 0 -len 100 -print_col_idx 2 2
	-cd ./build; ./sorer -f "../data/testfile.sor" -from 0 -len 100 -print_col_idx 3 0
	-cd ./build; ./sorer -f "../data/testfile.sor" -from 0 -len 100 -print_col_idx 1 2
	-cd ./build; ./sorer -f "../data/testfile.sor" -from 5 -len 100 -print_col_idx 1 0
	-cd ./build; ./sorer -f "../data/testfile.sor" -from 0 -len 100 -is_missing_idx 3 1
	-cd ./build; ./sorer -f "../data/testfile.sor" -from 0 -len 100 -is_missing_idx 3 2
	-cd ./build; ./sorer -f "../data/testfile.sor" -from 0 -len 100 -is_missing_idx 1 1
	-cd ./build; ./sorer -f "../data/testfile.sor" -from 5 -len 100 -is_missing_idx 1 1
	-cd ./build; ./sorer -f "../data/testfile.sor" -from 0 -len 100 -print_col_idx 2 0
	-cd ./build; ./sorer -f "../data/testfile.sor" -from 0 -len 2 -print_col_idx 2 0
	-cd ./build; ./sorer -f "../data/testfile.sor" -from 19 -len 20 -print_col_idx 3 0
	-cd ./build; ./sorer -f "../data/changingschema.sor" -print_col_idx 1 500
	-cd ./build; ./sorer -f "../data/testfile.sor" -print_col_idx 3 0 -is_missing_idx 3 1
	-cd ./build; ./sorer -f "../data/testfile.sor" -print_col_idx 3 0 -is_missing_idx 3 1 -print_col_type 0
	-cd ./build; ./sorer -f "../data/changingschema.sor" -print_col_idx 1 500
	-cd ./build; ./sorer -f "../data/spacey.sor" -print_col_idx 3 0
	-cd ./build; ./sorer -f "../data/spacey.sor" -print_col_idx 2 0
	-cd ./build; ./sorer -f "../data/spacey.sor" -print_col_type 2
	-cd ./build; ./sorer -f "../data/spacey.sor" -print_col_type asdljfaklsdf
	-cd ./build; ./sorer -f "../data/spacey.sor" -print_col_idx 2
	-cd ./build; ./sorer -f "../data/spacey.sor" -print_col_idx asdfasdf asdfasd
	-cd ./build; ./sorer -f "../data/spacey.sor" -print_col_idx 2 asdfasd

demo_echo: build_all
	cd ./build; ./demo_echo_server & ./demo_echo_client

demo_distro_app: build_all
	cd ./build; ./demo_registrar & ./demo_node

clean:
	rm -rf ./build

docker_build_all:
	docker build -t snowy_song:0.1 .
	docker run -ti -v `pwd`:/test snowy_song:0.1 bash -c "cd /test; make build_all"

docker_test_all: docker_build_all
	docker run -ti -v `pwd`:/test snowy_song:0.1 bash -c "cd /test; make test_all"
