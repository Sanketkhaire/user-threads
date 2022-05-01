try: libraryFunctionsTest.c ../mythread.c ../lock.c
	cc libraryFunctionsTest.c ../mythread.c ../lock.c -o try
answer: matrixMultiTest.c ../mythread.c ../lock.c
	cc matrixMultiTest.c ../mythread.c ../lock.c -o answer
sync: sync_test.c ../mythread.c ../lock.c 
	cc sync_test.c ../mythread.c ../lock.c -o sync
clean:
	rm try
	rm answer
	rm sync
