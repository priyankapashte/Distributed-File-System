#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <netdb.h>
#include <openssl/md5.h>

#define CONFBUFF 200
#define MAXBUFFSIZE 2048
#define MAXCLIENTS 100
#define SIZE 50
#define SERVERCOUNT 4

int hextodec(char c);
void sendtoserver(char *part,int s1, int s2, int divsize, char *partnum);
void checkPart(char *partname, int k, char *p);
int rfile[4]={0,0,0,0};
char recvfile[2048];
 
int s[SERVERCOUNT];
char dfs1_ip[20], dfs2_ip[20], dfs3_ip[20], dfs4_ip[20], dfs1_port[10], dfs2_port[10],dfs3_port[10],dfs4_port[10], username[50], password[50],userpass[100];

void error(char *msg)    /* error - wrapper for perror */
{
    perror(msg);
      exit(1);
}

void List()
{
    char dirname[100]= ".././DFS1";
    char buffer[MAXBUFFSIZE];
    DIR*p;
    struct dirent *d;
    p=opendir(dirname);
    if(p==NULL)
        error("Cannot find directory");
    printf("Sending File names in the Server Directory...\n");
    d = readdir(p);
    while((d=readdir(p)))
    {
        if(d->d_name[0]!='.')
        {
            bzero(buffer, MAXBUFFSIZE);
            strcpy(buffer,d->d_name);
            printf("%s\n",buffer);
        }   
    }
}

void Put(char *filename)
{
    FILE *putfile, *p1, *p2, *p3, *p4;
    long fsize;
    int divsize;
    char part1[SIZE]=".", part2[SIZE]=".", part3[SIZE]=".", part4[SIZE]=".";

    putfile = fopen(filename,"r");
    if(putfile == NULL)
        error("File not found.\n");
    else
    {   
        int x=0;
        char buf[MAXBUFFSIZE];
        MD5_CTX c;
        ssize_t bytes;
        unsigned char out[MD5_DIGEST_LENGTH];

        MD5_Init(&c);
        while ((bytes = fread (buf,1,MAXBUFFSIZE,putfile)) != 0)
        MD5_Update (&c, buf, bytes);
        MD5_Final (out,&c);
        for(int n=0; n<MD5_DIGEST_LENGTH; n++)
                printf("%02x", out[n]);
        printf("\n");

        int decimal[32];
        for (int i=0; i < 32; i++)
        {   
            decimal[i] = hextodec(out[i]);
            x = (x * 16 + decimal[i])%4;
        }

        printf("x = %d\n",x);

        fseek(putfile,0,SEEK_SET);

        if ((fsize=fseek(putfile, 0, SEEK_END) == -1))
            error("The file was not seeked.\n");
        fsize = ftell(putfile);
        if (fsize == -1)
            error("The file size was not retrieved");
        fseek(putfile,0,SEEK_SET);
        printf("File size: %ld\n", fsize);
        divsize = fsize/4;

        char buffer[divsize];

        bzero(buffer,divsize);

        strcat(part1,filename);
        strcat(part2,filename);
        strcat(part3,filename);
        strcat(part4,filename);

        strcat(part1,".1");
        strcat(part2,".2");
        strcat(part3,".3");
        strcat(part4,".4");

        p1 = fopen(part1,"w");
        if(p1 == NULL)
            error("Part 1 could not be created.\n");
        fread(buffer,1,divsize,putfile);
        fwrite(buffer,divsize,1,p1);
        printf("Part 1 created.\n");

        bzero(buffer,divsize);

        p2 = fopen(part2,"w");
        if(p2 == NULL)
            error("Part 2 could not be created.\n");
        fread(buffer,1,divsize,putfile);
        fwrite(buffer,divsize,1,p2);
        printf("Part 2 created.\n");

        bzero(buffer,divsize);

        p3 = fopen(part3,"w");
        if(p3 == NULL)
            error("Part 3 could not be created.\n");
        fread(buffer,1,divsize,putfile);
        fwrite(buffer,divsize,1,p3);
        printf("Part 3 created.\n");

        bzero(buffer,divsize);

        p4 = fopen(part4,"w");
        if(p4 == NULL)
            error("Part 4 could not be created.\n");
        fread(buffer,1,divsize,putfile);
        fwrite(buffer,divsize,1,p4);
        printf("Part 4 created.\n");

        fclose(p1);
        fclose(p2);
        fclose(p3);
        fclose(p4);

        switch(x)
        {
            case 0:     //x=0 1-(1,2) 2-(2,3) 3-(3,4) 4-(4,1)
            
            //Part1      
            sendtoserver(part1,s[3],s[0],divsize,"1");  

            //Part2     
            sendtoserver(part2,s[0],s[1],divsize,"2");  

            //Part3     
            sendtoserver(part3,s[1],s[2],divsize,"3");  

            //Part4      
            sendtoserver(part4,s[2],s[3],divsize,"4");  

            break; 

            case 1:     //x=1 1-(4,1) 2-(1,2) 3-(2,3) 4-(3,4)
            
            //Part1      
            sendtoserver(part1,s[0],s[1],divsize,"1");  

            //Part2     
            sendtoserver(part2,s[1],s[2],divsize,"2");  

            //Part3     
            sendtoserver(part3,s[2],s[3],divsize,"3");  

            //Part4      
            sendtoserver(part4,s[3],s[0],divsize,"4");  

            break;   

            case 2:     //x=2 1-(3,4) 2-(4,1) 3-(1,2) 4-(2,3)
            
            //Part1      
            sendtoserver(part1,s[1],s[2],divsize,"1");  

            //Part2     
            sendtoserver(part2,s[2],s[3],divsize,"2");  

            //Part3     
            sendtoserver(part3,s[3],s[0],divsize,"3");  

            //Part4      
            sendtoserver(part4,s[0],s[1],divsize,"4");  

            break; 

            case 3:     //x=3 1-(2,3) 2-(3,4) 3-(4,1) 4-(1,2)
            
            //Part1      
            sendtoserver(part1,s[2],s[3],divsize,"1");  

            //Part2     
            sendtoserver(part2,s[3],s[0],divsize,"2");  

            //Part3     
            sendtoserver(part3,s[0],s[1],divsize,"3");  

            //Part4      
            sendtoserver(part4,s[1],s[2],divsize,"4");  

            break; 
        }  
    }
}

void sendtoserver(char *part, int s1, int s2, int divsize, char *partnum)
{
    printf("Partnum: %s\n",partnum);
    FILE *fpart;
    char k[1]={'z'},a[divsize+1],enc[divsize+1],ack[1];
    fpart = fopen(part,"r");
    if(fpart == NULL)
        error("File not found.\n");
    fseek(fpart,0,SEEK_SET);
    fread(a,1,divsize,fpart);
    printf("Sding: %s: \n",a );

    
    for(int j=0;j<divsize;j++)
    {
        enc[j] = a[j] ^ k[0];
    }
    strcat(enc,partnum);
   // printf("Sding: %s: \n", enc);
    if(s1>0)
    {
       if((send(s1,enc,divsize+1,0))<0)
            error("Error sending file \n");
        printf("Sent packet 1\n");
        if(recv(s1, ack, 1,0)<0)
            error("RECV ERROR"); 
    }   
    
    if(s2>0)
    {
        if((send(s2,enc,divsize+1,0))<0)
            error("Error sending file \n");
        printf("Sent packet 2\n");
        if(recv(s2, ack, 1,0)<0)
            error("RECV ERROR");
    }
    fclose(fpart);
}

void Get(char *filename)
{
    char s1part1[300],s1part2[300],s2part1[300],s2part2[300],s3part1[300],s3part2[300],s4part1[300],s4part2[300]; 
    char s1file1[300],s1file2[300],s2file1[300],s2file2[300],s3file1[300],s3file2[300],s4file1[300],s4file2[300];
    int nbytes;
    
    char ack[1];
    char k[1]={'z'};
    FILE *file;

    printf("In GET");
    printf("Receiving file...\n");

    if(s[0]>0)
    {
        if(send(s[0],"/DFS1",5,0)<0)
            error("SEND FAILED");
        if(recv(s[0], ack, 1,0)<0)
            error("RECV ERROR");

        if(send(s[0],"F",1,0)<0)
            error("SEND FAILED");
        if((nbytes=recv(s[0],s1file1,300,0))<0)
            error("RECV FAILED");
        s1file1[nbytes]='\0';
        printf("S1 file1 Received: %s\n",s1file1);
        if(send(s[0],"F",1,0)<0)
            error("SEND FAILED");
        if((nbytes=recv(s[0],s1part1,300,0))<0)
            error("RECV FAILED");
        s1part1[nbytes]='\0';
        printf("S1 Part1 Received: %s\n",s1part1);
        if(send(s[0],"F",1,0)<0)
            error("SEND FAILED");

        if(send(s[0],"F",1,0)<0)
            error("SEND FAILED");
        if((nbytes=recv(s[0],s1file2,300,0))<0)
            error("RECV FAILED");
        s1file2[nbytes]='\0';
        printf("S1 file2 Received: %s\n",s1file2);
        if(send(s[0],"F",1,0)<0)
            error("SEND FAILED");
        if((nbytes=recv(s[0],s1part2,300,0))<0)
            error("RECV FAILED");
        s1part2[nbytes]='\0';
        printf("S1 Part2 Received: %s\n",s1part2);
        if(send(s[0],"F",1,0)<0)
            error("SEND FAILED");
    }
 
    if(s[1]>0)
    {
        if(send(s[1],"/DFS2",5,0)<0)
            error("SEND FAILED");
        if(recv(s[1], ack, 1,0)<0)
            error("RECV ERROR");

        if(send(s[1],"F",1,0)<0)
            error("SEND FAILED");
        if((nbytes=recv(s[1],s2file1,300,0))<0)
            error("RECV FAILED");
        s2file1[nbytes]='\0';
        printf("S2 file1 Received: %s\n",s2file1);
        if(send(s[1],"F",1,0)<0)
            error("SEND FAILED");
        if((nbytes=recv(s[1],s2part1,300,0))<0)
            error("RECV FAILED");
        s2part1[nbytes]='\0';
        printf("S2 Part1 Received: %s\n",s2part1);
        if(send(s[1],"F",1,0)<0)
            error("SEND FAILED");

        if(send(s[1],"F",1,0)<0)
            error("SEND FAILED");
        if((nbytes=recv(s[1],s2file2,300,0))<0)
            error("RECV FAILED");
        s2file2[nbytes]='\0';
        printf("S1 file1 Received: %s\n",s2file2);
        if(send(s[1],"F",1,0)<0)
            error("SEND FAILED");
        if((nbytes=recv(s[1],s2part2,300,0))<0)
            error("RECV FAILED");
        s2part2[nbytes]='\0';
        printf("S2 Part2 Received: %s\n",s2part2);
        if(send(s[1],"F",1,0)<0)
            error("SEND FAILED");
    }

    if(s[2]>0)
    {
        if(send(s[2],"/DFS3",5,0)<0)
            error("SEND FAILED");
        if(recv(s[2], ack, 1,0)<0)
            error("RECV ERROR");

        if(send(s[2],"F",1,0)<0)
            error("SEND FAILED");
        if((nbytes=recv(s[2],s3file1,300,0))<0)
            error("RECV FAILED");
        s3file1[nbytes]='\0';
        printf("S1 file1 Received: %s\n",s3file1);
        if(send(s[2],"F",1,0)<0)
            error("SEND FAILED");
        if((nbytes=recv(s[2],s3part1,300,0))<0)
            error("RECV FAILED");
        s3part1[nbytes]='\0';
        printf("Part1 Received: %s\n",s3part1);
        if(send(s[2],"F",1,0)<0)
            error("SEND FAILED");

        if(send(s[2],"F",1,0)<0)
            error("SEND FAILED");
        if((nbytes=recv(s[2],s3file2,300,0))<0)
            error("RECV FAILED");
        s3file2[nbytes]='\0';
        printf("S1 file1 Received: %s\n",s3file2);
        if(send(s[2],"F",1,0)<0)
            error("SEND FAILED");
        if((nbytes=recv(s[2],s3part2,300,0))<0)
            error("RECV FAILED");
        s3part2[nbytes]='\0';
        printf("Part2 Received: %s\n",s3part2);
        if(send(s[2],"F",1,0)<0)
            error("SEND FAILED");
    }

    if(s[3]>0)
    {
        if(send(s[3],"/DFS4",5,0)<0)
            error("SEND FAILED");
        if(recv(s[3], ack, 1,0)<0)
            error("RECV ERROR");

        if(send(s[3],"F",1,0)<0)
            error("SEND FAILED");
        if((nbytes=recv(s[3],s4file1,300,0))<0)
            error("RECV FAILED");
        s4file1[nbytes]='\0';
        printf("S1 file1 Received: %s\n",s4file1);
        if(send(s[3],"F",1,0)<0)
            error("SEND FAILED");
        if((nbytes=recv(s[3],s4part1,300,0))<0)
            error("RECV FAILED");
        s4part1[nbytes]='\0';
        printf("Part1 Received: %s\n",s4part1);
        if(send(s[3],"F",1,0)<0)
            error("SEND FAILED");

        if(send(s[3],"F",1,0)<0)
            error("SEND FAILED");
        if((nbytes=recv(s[3],s4file2,300,0))<0)
            error("RECV FAILED");
        s4file2[nbytes]='\0';
        printf("S4 file2 Received: %s\n",s4file2);
        if(send(s[3],"F",1,0)<0)
            error("SEND FAILED");
        if((nbytes=recv(s[3],s4part2,300,0))<0)
            error("RECV FAILED");
        s4part2[nbytes]='\0';
        printf("Part2 Received: %s\n",s4part2);
        if(send(s[3],"F",1,0)<0)
            error("SEND FAILED");
    }

    char partname[300];
    
    bzero(recvfile,2048);

    file = fopen("GETFile","w");
    if(file == NULL)
        error("FILE NOT OPENED FOR WRITING");

    for(int k=1;k<=4;k++)
    {
        printf("k=%d\n",k);

        if(s[0]>0)
        {
            bzero(partname,300);
            strcpy(partname,s1file1);
            checkPart(partname,k,s1part1);
            
            bzero(partname,300);
            strcpy(partname,s1file2);
            checkPart(partname,k,s1part2);
        }
        
        if(s[1]>0)
        {
            bzero(partname,300);
            strcpy(partname,s2file1);
            checkPart(partname,k,s2part1);

            bzero(partname,300);
            strcpy(partname,s2file2);
            checkPart(partname,k,s2part2);
        }
        
        if(s[2]>0)
        {
            bzero(partname,300);
            strcpy(partname,s3file1);
            checkPart(partname,k,s3part1);

            bzero(partname,300);
            strcpy(partname,s3file2);
            checkPart(partname,k,s3part2);
        }
        
        if(s[3]>0)
        {
            bzero(partname,300);
            strcpy(partname,s4file1);
            checkPart(partname,k,s4part1);

            bzero(partname,300);
            strcpy(partname,s4file2);
            checkPart(partname,k,s4part2);
        }  
    }
    for(int m=0;m<4;m++)
    {
        printf("Rfile:%d m=%d\n",rfile[m],m);
        if(rfile[m]!=1)
        {
            printf("Rfile:%d m=%d\n",rfile[m],m);
            printf("File is incomplete\n");
            break;
        }
        else if(m==3)
        {
            char dec[strlen(recvfile)];
            for(int j=0;j<strlen(recvfile);j++)
            {
                dec[j] = recvfile[j] ^ k[0];
            }
            printf("Recvfile: %s\n",dec);
            fwrite(dec,strlen(recvfile),1,file);
            fclose(file);
        }
    }
}

void checkPart(char *partname,int k, char *p)
{
    char *token;
    int part;
    token = strtok(partname,".");
    token = strtok(NULL,".");
    token = strtok(NULL,".");
    token = strtok(NULL,"\n");
    //printf("token: %s",token);
    part=atoi(token);
    printf("Part: %d\n",part);
    if((part == k) && (rfile[k-1] == 0))
    {
        rfile[k-1]=1;
        strcat(recvfile,p);
    }
}

void ReadConfigFile(char *conf)
{
    FILE *dfc;
    char line[CONFBUFF];
    char *token;
    dfc = fopen(conf,"r");
    if(dfc == NULL)
        error("Configuration File not found.\n");
    else
    {
        printf("Reading Config File\n");
        while(fgets(line, CONFBUFF, dfc) != NULL)
        {
            if(strstr(line,"Server"))
            {
                token = strtok(line," ");
                token = strtok(NULL," ");
                if(!strcmp(token,"DFS1"))
                {
                    token = strtok(NULL,":");
                    strcpy(dfs1_ip,token);
                    token = strtok(NULL,"\n");
                    strcpy(dfs1_port,token);
                    printf("DFS1 IP: %s\n", dfs1_ip);
                    printf("DFS1 Port: %s\n", dfs1_port);
                }
                else if(!strcmp(token,"DFS2"))
                {
                    token = strtok(NULL,":");
                    strcpy(dfs2_ip,token);
                    token = strtok(NULL,"\n");
                    strcpy(dfs2_port,token);
                    printf("DFS2 IP: %s\n", dfs2_ip);
                    printf("DFS2 Port: %s\n", dfs2_port);
                }
                else if(!strcmp(token,"DFS3"))
                {
                    token = strtok(NULL,":");
                    strcpy(dfs3_ip,token);
                    token = strtok(NULL,"\n");
                    strcpy(dfs3_port,token);
                    printf("DFS3 IP: %s\n", dfs3_ip);
                    printf("DFS3 Port: %s\n", dfs3_port);
                }
                else if(!strcmp(token,"DFS4"))
                {
                    token = strtok(NULL,":");
                    strcpy(dfs4_ip,token);
                    token = strtok(NULL,"\n");
                    strcpy(dfs4_port,token);
                    printf("DFS4 IP: %s\n", dfs4_ip);
                    printf("DFS4 Port: %s\n", dfs4_port);
                }
            }
            else if(strstr(line,"Username"))
            {
                token = strtok(line," ");
                token = strtok(NULL,"\n");
                strcpy(username,token);
                printf("Username: %s\n",username);
            }
            else if(strstr(line,"Password"))
            {
                token = strtok(line," ");
                token = strtok(NULL,"\n");
                strcpy(password,token);
                printf("Password: %s\n",password); 
            }
        }
    }
}

int Server(char *host, char *port)
{
    int sock, option = 1;
    struct sockaddr_in client;
    int sockaddr_len = sizeof(struct sockaddr_in);

    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        error("SOCKET FALIED");

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    
    client.sin_family = AF_INET;                                // Address Family
    client.sin_port = htons((unsigned int)atoi(port));          // Sets port to network byte order
    inet_aton(host, &client.sin_addr);
  //  client.sin_addr.s_addr = inet_addr(host);
    bzero(&client.sin_zero,0);                                  // Zero the struct
   

    if(connect(sock,(struct sockaddr *)&client, sizeof(client)) < 0)
    {
        printf("CONNECT FAILED");
        sock = -1;
    }

    return sock;
}

int hextodec(char c)
{

  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  else
    return -1;  

}

int main(int argc, char **argv)
{
    int nbytes;
    char command[SIZE],response[1],buff2[MAXBUFFSIZE],ack[1];
    char *token;
    struct timeval timeout;

    ReadConfigFile(argv[1]);
    s[0] = Server(dfs1_ip,dfs1_port);
    s[1] = Server(dfs2_ip,dfs2_port);
    s[2] = Server(dfs3_ip,dfs3_port);
    s[3] = Server(dfs4_ip,dfs4_port);

    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    for(int i=0; i<SERVERCOUNT; i++)
    {
        if(setsockopt(s[i], SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(struct timeval))<0)
            printf("No response from server %d",i+1);
    }

    while(1)
    {
        printf("Select an option:\n");
        printf("1. LIST\n2. PUT\n3. GET\n");
        fgets(command,SIZE,stdin);
        printf("Command Entered: %s\n",command);
        bzero(userpass,100);
        strcat(userpass,username);
        strcat(userpass," ");
        strcat(userpass,password);
        for(int i=0; i<SERVERCOUNT; i++)
        {
            printf("%d\n",s[i]);
            if(s[i]>0)
            {
                printf("Sending Username & Password %d %s\n",i,userpass);
                if(send(s[i],userpass,strlen(userpass), 0)<0)
                    printf("SENDING USERNAME & PASSWORD FAILED");
                if(recv(s[i],response,1,0)<0)
                    printf("RECV FAILED");
                if(response[0] == 'F')
                    printf("USERNAME AND PASSWORD DID NOT MATCH\n");
            }
        }
        token = strtok(command," ");
        for(int i=0; i<SERVERCOUNT; i++)
        {
            if(s[i]>0)
            {
                if((send(s[i],token,strlen(token),0))<0)
                    error("Error sending file \n");
            }
        }
        if(!strcmp(token,"LIST"))
        {
            List();
        }
        else if(!strcmp(token,"PUT"))
        {
            token = strtok(NULL,"\n");
            for(int i=0; i<SERVERCOUNT; i++)
            {
                if(s[i]>0)
                {
                    if(recv(s[i], ack, 1,0)<0)
                        error("RECV ERROR");
                    if((send(s[i],token,strlen(token),0))<0)
                        error("Error sending file \n");
                }
            }
            Put(token);
        }
        else if(!strcmp(token,"GET"))
        {
            token = strtok(NULL,"\n");
            printf("token: %s\n",token);
            for(int i=0; i<SERVERCOUNT; i++)
            {
                if(s[i]>0)
                {
                    if(recv(s[i], ack, 1,0)<0)
                        error("RECV ERROR");
                    if((send(s[i],token,strlen(token),0))<0)
                        error("Error sending file \n");
                }
            }
            Get(token);
        }
        else
        {
            printf("Enter a valid command.\n");
        }
    }     
    return 0;
}