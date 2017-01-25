#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#define MAXBUFFSIZE 2048
#define MAXCLIENTS 100
#define CONFBUFF 200

int ServerConfig(char *port);
void ReadConfigFile(char *conf);
void Authenticate(int asock);
void UploadFile(int asock, char *file, char *folder);
void DownloadFile(int asock, char *file, char *folder);

char userpass1[50],userpass2[50],user[50];
char root[100]="/Users/avneendrakanva/Desktop/PA3";
int rec[4];

void error(char *msg)    /* error - wrapper for perror */
{
	perror(msg);
  	exit(1);
}

int main(int argc, char **argv)
{
	struct sockaddr_in client;
	int sock1,sock2,sock3,sock4,asock1,asock2,asock3,asock4,nbytes;
	char *server, *port;
	char command[10],fname[50];
	ReadConfigFile("dfs.conf");
	socklen_t sockaddr_len = sizeof(struct sockaddr_in);
	if(!strcmp(argv[1],"/DFS1"))
	{
		sock1 = ServerConfig(argv[2]);	
		if((asock1 = accept(sock1, (struct sockaddr *)&client, &sockaddr_len)) == -1)
			error("ACCEPT1 FAILED");
		while(1)
		{
			Authenticate(asock1);
			if((nbytes=recv(asock1, command, sizeof(command),0))<0)
				error("COMMAND NOT RECEIVED");
			printf("Command Received: %s\n",command);
			
			if(!strcmp(command,"PUT"))
			{
				if(send(asock1,"F",1,0)<0)
					error("SEND FAILED");
				if((nbytes=recv(asock1, fname, sizeof(fname),0))<0)
					error("COMMAND NOT RECEIVED");
				printf("File name: %s\n",fname);
				UploadFile(asock1,fname,"/DFS1");
			}	
			else if(!strcmp(command,"GET"))
			{
				if(send(asock1,"F",1,0)<0)
					error("SEND FAILED");
				if((nbytes=recv(asock1, fname, sizeof(fname),0))<0)
					error("COMMAND NOT RECEIVED");
				DownloadFile(asock1,fname,"/DFS1");
			}
		}
	}
	else if(!strcmp(argv[1],"/DFS2"))
	{
		sock2 = ServerConfig(argv[2]);
		if((asock2 = accept(sock2, (struct sockaddr *)&client, &sockaddr_len)) == -1)
			error("ACCEPT2 FAILED");
		while(1)
		{
			Authenticate(asock2);
			if((nbytes=recv(asock2, command, sizeof(command),0))<0)
				error("COMMAND NOT RECEIVED");
			printf("Command Received: %s\n",command);
			
			if(!strcmp(command,"PUT"))
			{
				if(send(asock2,"F",1,0)<0)
					error("SEND FAILED");
				if((nbytes=recv(asock2, fname, sizeof(fname),0))<0)
					error("COMMAND NOT RECEIVED");
				printf("File name: %s\n",fname);
				UploadFile(asock2,fname,"/DFS2");
			}
			else if(!strcmp(command,"GET"))
			{
				if(send(asock2,"F",1,0)<0)
					error("SEND FAILED");
				if((nbytes=recv(asock2, fname, sizeof(fname),0))<0)
					error("COMMAND NOT RECEIVED");
				DownloadFile(asock2,fname,"/DFS2");
			}
		}
	}
	else if(!strcmp(argv[1],"/DFS3"))
	{
		sock3 = ServerConfig(argv[2]);
		if((asock3 = accept(sock3, (struct sockaddr *)&client, &sockaddr_len)) == -1)
			error("ACCEPT3 FAILED");
		while(1)
		{
			Authenticate(asock3);
			if((nbytes=recv(asock3, command, sizeof(command),0))<0)
				error("COMMAND NOT RECEIVED");
			printf("Command Received: %s\n",command);
						
			if(!strcmp(command,"PUT"))
			{
				if(send(asock3,"F",1,0)<0)
					error("SEND FAILED");
				if((nbytes=recv(asock3, fname, sizeof(fname),0))<0)
					error("COMMAND NOT RECEIVED");
				printf("File name: %s\n",fname);
				UploadFile(asock3,fname,"/DFS3");
			}
			else if(!strcmp(command,"GET"))
			{
				if(send(asock3,"F",1,0)<0)
					error("SEND FAILED");
				if((nbytes=recv(asock3, fname, sizeof(fname),0))<0)
					error("COMMAND NOT RECEIVED");
				DownloadFile(asock3,fname,"/DFS3");
			}
		}
	}	
	else if(!strcmp(argv[1],"/DFS4"))
	{
		sock4 = ServerConfig(argv[2]);
		if((asock4 = accept(sock4, (struct sockaddr *)&client, &sockaddr_len)) == -1)
			error("ACCEPT4 FAILED");
		while(1)
		{
			Authenticate(asock4);
			if((nbytes=recv(asock4, command, sizeof(command),0))<0)
				error("COMMAND NOT RECEIVED");
			printf("command Received: %s\n",command);
			
			if(!strcmp(command,"PUT"))
			{
				if(send(asock4,"F",1,0)<0)
					error("SEND FAILED");
				if((nbytes=recv(asock4, fname, sizeof(fname),0))<0)
					error("COMMAND NOT RECEIVED");
				printf("File name: %s\n",fname);
				UploadFile(asock4,fname,"/DFS4");
			}
			else if(!strcmp(command,"GET"))
			{
				if(send(asock4,"F",1,0)<0)
					error("SEND FAILED");
				if((nbytes=recv(asock4, fname, sizeof(fname),0))<0)
					error("COMMAND NOT RECEIVED");
				DownloadFile(asock4,fname,"/DFS4");
			}
		}
	}	
	else
		printf("Invalid Argument.\n");

	return 0;
}

int ServerConfig(char *port)
{
	int sock,option=1;
	struct sockaddr_in server;
	int sockaddr_len = sizeof(struct sockaddr_in);

	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		error("SOCKET FALIED");
     
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

	server.sin_family = AF_INET;			// Address Family
	server.sin_port = htons((unsigned short)atoi(port));			// Sets port to network byte order
	server.sin_addr.s_addr = INADDR_ANY;	// Sets remote IP address
	bzero(&server.sin_zero,0);				// Zero the struct

	if((bind(sock, (struct sockaddr *)&server, sockaddr_len)) == -1)
		error("BIND FAILED");

	if((listen(sock, MAXCLIENTS)) == -1)
		error("LISTEN FAILED");

	return sock;
}

void ReadConfigFile(char *conf)
{
	FILE *dfs;
    char line[CONFBUFF],ch;
    int nlines=2;
    char *token;
    dfs = fopen(conf,"r");
    if(dfs == NULL)
        error("Configuration File not found.\n");
    else
    {
        printf("Reading Config File\n");
    /*    while(fgets(line, CONFBUFF, dfs) != NULL)
        	nlines++;*/
        while(fgets(line, CONFBUFF, dfs) != NULL)
        {
        	if(nlines == 2)
        	{
	        	token = strtok(line,"\n");
	        	strcpy(userpass1,token);
	        	nlines--;
        	}
        	else if(nlines == 1)
        	{
        		token = strtok(line,"\n");
	        	strcpy(userpass2,token);
	        	nlines--;
        	}
        }
        printf("userpass1: %s\n", userpass1);
        printf("userpass2: %s\n", userpass2);
     //	printf("Number of lines in File: %d\n",nlines);
    }
}

void Authenticate(int asock)
{
	char buff[50];
	int nbytes;
	printf("Autheticating Username and Password...\n");

	bzero(buff,50);

	if((nbytes=recv(asock, buff, sizeof(buff),0))<0)
		error("RECV ERROR");
	buff[nbytes]='\0';
	printf("%s\n",buff);


	if(!strcmp(userpass1,buff))
	{
		printf("Username1 & Password1 Matched\n");
		strcpy(user,userpass1);
	}	
	else if(!strcmp(userpass2,buff))
	{
		printf("Username2 & Password2 Matched\n");
		strcpy(user,userpass2);
	}
	else
	{
		if(send(asock,"F",1,0)<0)
			error("SEND FAILED");
		error("USERNAME AND PASSWORD DID NOT MATCH\n");
	}
		
	if(send(asock,"X",1,0)<0)
		error("SEND FAILED");

}

void UploadFile(int asock, char *file, char *folder)
{
	char part1[50],part2[50],partnum1[2]=" ",partnum2[2]=" ",f1[300],f2[300];
	int nbytes;
	FILE *p1, *p2;
	char *token;
	token = strtok(user," ");

	//printf("Folder: %s\n",token);
	//printf("Folder Main: %s\n",folder);
	if((nbytes=recv(asock, part1, sizeof(part1),0))<0)
		error("RECV ERROR");

	partnum1[0] = part1[nbytes-1];
	partnum1[1] ='\0';
	part1[nbytes-1]='\0';
	printf("Part1: %s\n",part1);
	if(send(asock,"F",1,0)<0)
		error("SEND FAILED");
	if((nbytes=recv(asock, part2, sizeof(part2),0))<0)
		error("RECV ERROR");

	partnum2[0] = part2[nbytes-1];
	partnum2[1] ='\0';
	part2[nbytes-1]='\0';
	printf("Part2: %s\n",part2);
	if(send(asock,"F",1,0)<0)
		error("SEND FAILED");
	
	printf("Partnum 1: %c\n",partnum1[0]);
	printf("Partnum 2: %c\n",partnum2[0]);
	
	bzero(f1,50);
	strcat(f1,root);
	strcat(f1,folder);
	strcat(f1,"/");
	strcat(f1,user);
	strcat(f1,"/");
	struct stat st = {0};
	if (stat(f1, &st) == -1) 
	{
		mkdir(f1, 0700);
	}
	strcat(f1,".");
	strcat(f1,file);
	strcat(f1,".");
	strcat(f1,partnum1);
	printf("F1: %s\n",f1);

	bzero(f2,50);
	strcat(f2,root);
	strcat(f2,folder);
	strcat(f2,"/");
	strcat(f2,user);
	strcat(f2,"/");
	strcat(f2,".");
	strcat(f2,file);
	strcat(f2,".");
	strcat(f2,partnum2);
	printf("F2: %s\n",f2);

	p1 = fopen(f1,"w");
	if(p1 == NULL)
		error("File not opened");
	else
	{
		fwrite(part1,nbytes-1,1,p1);
	}
	p2 = fopen(f2,"w");
	if(p2 == NULL)
		error("File not opened");
	else
	{
		fwrite(part2,nbytes-1,1,p2);
	}
	fclose(p1);
	fclose(p2);
}

void DownloadFile(int asock, char *file, char *folder)
{
	FILE *part1, *part2, *part3, *part4;
	char path[300]="/Users/avneendrakanva/Desktop/PA3",temp[300],ack[1],start[5];
	printf("Sending file parts...");
	char *token;
	int p1_size, p2_size, p3_size, p4_size;
	int sent[4]={0,0,0,0};

	printf("Waiting for DFS command....\n");
	if(recv(asock, start, 5,0)<0)
		error("RECV ERROR");
	printf("Start: %s\n",start);
	if(!strcmp(start,folder))
	{
		if(send(asock,"F",1,0)<0)
			error("SEND FAILED");
		token = strtok(user," ");
		//strcat(path,root);
		printf("FIle: %s\n",file);
		int flen = strlen(file);
		strcat(path,folder);
		strcat(path,"/");
		strcat(path,user);
		strcat(path,"/");
		strcat(path,file);

		int n = strlen(path);

		bzero(temp,300);
		strncpy(temp,path,n);
		strcat(temp,".1");
		printf("temp: %s\n",temp);
		part1 = fopen(temp,"r");
		if(part1 == NULL)
		{
			printf("Part 1 not present on Server\n");
		}
		else
		{
	
				fseek(part1,0,SEEK_SET);
		        if ((p1_size=fseek(part1, 0, SEEK_END) == -1))
		            error("The file was not seeked.\n");
		        p1_size = ftell(part1);
		        if (p1_size == -1)
		            error("The file size was not retrieved");
		        fseek(part1,0,SEEK_SET);
		        printf("File size: %d\n", p1_size);
		        char buff1[p1_size];
		        fread(buff1,1,p1_size,part1);
		        printf("%s\n",buff1);
	        
	        	printf("Sending file....\n");
	        	if(recv(asock, ack, 1,0)<0)
			       error("RECV ERROR");
			    printf("Received ack1\n");
			    if(send(asock,temp,strlen(temp),0)<0)
			    	error("SEND FAILED");
			    if(recv(asock, ack, 1,0)<0)
			       error("RECV ERROR");
			    if(send(asock,buff1,p1_size,0)<0)
			    	error("SEND FAILED");
			    if(recv(asock, ack, 1,0)<0)
			       error("RECV ERROR");
			   	   
		}

		bzero(temp,300);
		strncpy(temp,path,n);
		strcat(temp,".2");
		printf("temp: %s\n",temp);
		part2 = fopen(temp,"r");
		if(part2 == NULL)
		{
			printf("Part 2 not present on Server\n");
		}
		else
		{
			
				fseek(part2,0,SEEK_SET);
		        if ((p2_size=fseek(part2, 0, SEEK_END) == -1))
		            error("The file was not seeked.\n");
		        p2_size = ftell(part2);
		        if (p2_size == -1)
		            error("The file size was not retrieved");
		        fseek(part2,0,SEEK_SET);
		        printf("File size: %d\n", p2_size);
		        char buff2[p2_size];
		        fread(buff2,1,p2_size,part2);
		        printf("%s\n",buff2);
	    
	        	printf("Sending file....\n");
	        	if(recv(asock, ack, 1,0)<0)
			       error("RECV ERROR");
			    printf("Received ack2\n");
			    if(send(asock,temp,strlen(temp),0)<0)
			    	error("SEND FAILED");
			    if(recv(asock, ack, 1,0)<0)
			       error("RECV ERROR");
			    if(send(asock,buff2,p2_size,0)<0)
			    	error("SEND FAILED");
			    if(recv(asock, ack, 1,0)<0)
			       error("RECV ERROR");
			   	sent[1]=1; 
	        
		}

		bzero(temp,300);
		strncpy(temp,path,n);
		strcat(temp,".3");
		printf("temp: %s\n",temp);
		part3 = fopen(temp,"r");
		if(part3 == NULL)
		{
			printf("Part 3 not present on Server\n");
		}
		else
		{
		    
				fseek(part3,0,SEEK_SET);
		        if ((p3_size=fseek(part3, 0, SEEK_END) == -1))
		            error("The file was not seeked.\n");
		        p3_size = ftell(part3);
		        if (p3_size == -1)
		            error("The file size was not retrieved");
		        fseek(part3,0,SEEK_SET);
		        printf("File size: %d\n", p3_size);
		        char buff3[p3_size];
		        fread(buff3,1,p3_size,part3);
		        printf("%s\n",buff3);
	    
	        	printf("Sending file....\n");
	        	if(recv(asock, ack, 1,0)<0)
			       error("RECV ERROR");
			    printf("Received ack3\n");
			    if(send(asock,temp,strlen(temp),0)<0)
			    	error("SEND FAILED");
			    if(recv(asock, ack, 1,0)<0)
			       error("RECV ERROR");
			    if(send(asock,buff3,p3_size,0)<0)
			    	error("SEND FAILED");
			    printf("Sent..\n");
			    if(recv(asock, ack, 1,0)<0)
			       error("RECV ERROR");
			   	sent[2]=1; 
	         
		}

		bzero(temp,300);
		strncpy(temp,path,n);
		strcat(temp,".4");
		printf("temp: %s\n",temp);
		part4 = fopen(temp,"r");
		if(part4 == NULL)
		{
			printf("Part 4 not present on Server\n");
		}
		else
		{
		    
				fseek(part4,0,SEEK_SET);
		        if ((p4_size=fseek(part4, 0, SEEK_END) == -1))
		            error("The file was not seeked.\n");
		        p4_size = ftell(part4);
		        if (p4_size == -1)
		            error("The file size was not retrieved");
		        fseek(part4,0,SEEK_SET);
		        printf("File size: %d\n", p4_size);
		        char buff4[p4_size];
		        fread(buff4,1,p4_size,part4);
		        printf("%s\n",buff4);
	    
	        	printf("Sending file....\n");
	        	if(recv(asock, ack, 1,0)<0)
			       error("RECV ERROR");
			    printf("Received ack4\n");
			    if(send(asock,temp,strlen(temp),0)<0)
			    	error("SEND FAILED");
			    if(recv(asock, ack, 1,0)<0)
			       error("RECV ERROR");
			    if(send(asock,buff4,p4_size,0)<0)
			    	error("SEND FAILED");
			    if(recv(asock, ack, 1,0)<0)
			       error("RECV ERROR");
			   	sent[3]=1; 
	        
		}
	}


}