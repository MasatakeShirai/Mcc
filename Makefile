Mcc: Mcc.c


test: Mcc
	./Mcc -test
	./test.sh


clean:
	rm -f Mcc *.o* ~tmp*
