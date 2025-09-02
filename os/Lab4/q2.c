// author : bibhuti 
#include <stdio.h>
#include <unistd.h> 
#include <string.h>
#include <ctype.h>  
#include <stdlib.h>
#include <sys/wait.h> 

int main(int argc , char *argv[]){ 
	if( argc != 2) { 
		printf("Usage: %s <num_terms>\n", argv[0]) ; 
                return 1 ; 
        }
	int num = atoi(argv[1]) ; 
	if(num <= 0) { 
		printf("Error , the number entered cannt be negative") ; 
		return(1) ; 
	}
	if(fork() == 0 ) { 
		//child it is
		int n = num ;
		int dp[n+2] ;
		dp[0]= 0 ;  
		dp[1]= 1;
		dp[2]= 1 ; 
		for(int i = 3 ; i<= n ; i++ ) { 
			dp[i] = dp[i-1] + dp[i-2] ; 
		} 
		for(int i = 0; i< n ; i++ ) { 
			printf("%d ",dp[i]) ; 
		} 
		printf("\n") ; 
		return(0) ;  

	} 
	else{ 
		//parent : 
		wait(NULL) ;
		printf("Finally parents end after child finished") ; 
		return(0) ; 
		//waits for the child to finish
	}
} 
