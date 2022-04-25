#include <stdio.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc,char * argv[]){

    if (argc !=4){
        printf("utilisation : %s ip_serveur port_serveur nom_du_fichier \n", argv[0]);
        exit(1);
    }
    uint16_t port;
    char ip[30];
    char msg[2000];
    char fichier[2000];
    sscanf(argv[1],"%s",ip);
    sscanf(argv[2],"%hd",&port);
    sscanf(argv[3],"%s",fichier);

    int sock=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in server,client;

    server.sin_addr.s_addr=inet_addr(ip);
    server.sin_family=AF_INET;
    server.sin_port=htons(port);

    connect(sock,(const struct sockaddr *)&server,(socklen_t) sizeof(server));

    printf("connecté \n");
    
    printf("fichier à envoyer : %s \n",fichier);

    send(sock,fichier,sizeof(fichier),0);
    
    
    struct stat sb;
    stat(fichier, &sb);
    off_t taille =sb.st_size;


    printf("taille : %lld bytes \n",taille);
    int number_to_send=(int)taille;
    write(sock,&number_to_send,sizeof(number_to_send));

    FILE* Fichier_to_send =fopen(fichier, "r");
    while(!feof(Fichier_to_send)){
        char c=fgetc(Fichier_to_send);
        send(sock,&c,sizeof(c),0);
    }

    

    close(sock);
    fclose(Fichier_to_send);


    return 0;
}