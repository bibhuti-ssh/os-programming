
// author : bibhuti 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>

int main(int argv , int argc[]) { 
	if(argv != 3 ) { 
		printf("not correct command \n") ;
		return(1) ; 
	}

	int N = stoiargc[1] ; 
	int M = argc[2] ; 

