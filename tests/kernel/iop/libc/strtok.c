#include <common-iop.h>
#include <string.h>

const char *getStringRep(const char *str) {
	return str ? str : "null";
}

void doStrtok(char *s, const char *delim)
{
	printf("strtok('%s', '%s') -> ", getStringRep(s), getStringRep(delim));
	char *result = strtok(s, delim);
	printf("'%s'\n", getStringRep(result));
}

int _start(int argc, char **argv) {
	printf("-- TEST BEGIN\n");
	
	{
		char inputString[] = "val1, val2,   val3,val4";
		static const char delim[] = ", ";
		
		doStrtok(inputString, delim);
		doStrtok(NULL, delim);
		doStrtok(NULL, delim);
		doStrtok(NULL, delim);
		doStrtok(NULL, delim);
	}
	
	{
		char inputString[] = "val1,val2,val3,val4";
		doStrtok(inputString, NULL);
		doStrtok(NULL, NULL);
	}
	
	doStrtok(NULL, NULL);
	
	printf("-- TEST END\n");
	return 0;
}
