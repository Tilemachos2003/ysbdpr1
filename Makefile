join:
	@echo " Compile sort_main ...";
	gcc -g -I ./include/ -L ./lib/ -Wl,-rpath,./lib/ ./examples/join_main.c ./src/record.c ./src/join.c -lhp_file -lbf -o ./build/join_main -O2
