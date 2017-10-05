TEST_PROC_SRC := Process_Test/test_pr.c
TEST_PROC_EXE := Process_Test/test_pr.out

TEST_PTHREAD_SRC := Pthread_Test/test_pthread.c
TEST_PTHREAD_EXE := Pthread_Test/test_pthread.out

PTHREAD_LIB := -lpthread

#Target option for compiling and loading kernel module.
load: 
	cd scheduler && make load
#Target option for unloading and cleaning the generated kernel modules.
unload: 
	cd scheduler && make unload

#Target option for compiling the test_process program.
comp_pr_test:
	gcc $(TEST_PROC_SRC) -o $(TEST_PROC_EXE)
#Target option for running the test_process program.
pr_test: comp_pr_test 
	./$(TEST_PROC_EXE)


#Target option for compiling the test_pthread program.
comp_pthread_test:
	gcc $(TEST_PTHREAD_SRC) -o $(TEST_PTHREAD_EXE) $(PTHREAD_LIB)
#Target option for running the test_pthread program.
pthread_test: comp_pthread_test 
	./$(TEST_PTHREAD_EXE)



#Target option for cleaning the generated kernel modules.
clean_modules:
	cd scheduler && make clean
#Target option for cleaning the test_process program.
clean_pr_test:
	rm -f $(TEST_PROC_EXE)
#Target option for cleaning the test_process program.
clean_pthread_test:
	rm -f $(TEST_PTHREAD_EXE)
#Target option for cleaning the test_process program and the generated kernel modules
cleanall: clean_pr_test clean_pthread_test clean_modules
