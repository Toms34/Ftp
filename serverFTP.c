#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>

struct Thread_params{
    int client_socket;
    int Id;
};

void *Thread_Function(void * args)
{
    struct Thread_params * params = (struct Thread_params *) args;
    printf("Thread N° %d \n",params->Id);
    char nomFichier[2000];
    recv(params->client_socket, &nomFichier, sizeof(nomFichier), 0); // Recevoir le nom du fichier
    // printf("nom du fichier : %s \n",nomFichier);

    int taille = 0;
    read(params->client_socket, &taille, sizeof(taille)); // Recevoir la taille du fichier
    // printf("taille : %i bytes\n",taille);

    char *fichier_avec_chemin;

    fichier_avec_chemin = malloc(strlen(nomFichier) + sizeof("reception/")); // creation du char avec le chemin vers le fichier
    strcpy(fichier_avec_chemin, "reception/");                               // on y met reception/
    strcat(fichier_avec_chemin, nomFichier);                                 // on rajoute le nom du fichier final

    FILE *Fichier = fopen(fichier_avec_chemin, "w");

    char *data = malloc(taille);
    int e = 0;
    while (e < taille)
    {
        recv(params->client_socket, &data[e], sizeof(data[e]), 0);
        if (&data[e] == NULL)
            break;
        fwrite(&data[e], 1, 1, Fichier);
        e++;
    }

    // printf("dans le fichier %s il doit y avoir %s \n",nomFichier,data);

    fclose(Fichier);
    free(fichier_avec_chemin);
    close(params->client_socket);

    return NULL;
}

int main(int argc, char *argv[])
{
    
    if (argc != 2)
    {
        printf("Usage : %s port\n", argv[0]);
        exit(1);
    }
    uint16_t port;
    sscanf(argv[1], "%hd", &port);

    int sock_server = socket(AF_INET, SOCK_STREAM, 0);
    printf("socket creer \n");

    struct sockaddr_in server;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    bind(sock_server, (const struct sockaddr *)&server, sizeof(server));
    printf("bind done\n");

    char nomFichier[4000];
    listen(sock_server, 5);
    printf("listen \n");

    struct sockaddr_in client[10];
    socklen_t n = sizeof(&client);

    int parent = getpid();

    int i = 0;
    int client_sock[10];
    pthread_t Thread[10];
    while (1)
    {
        client_sock[i] = accept(sock_server, (struct sockaddr *)&client[i], &n);
        printf("accept \n");
        struct Thread_params param;
        param.client_socket=client_sock[i];
        param.Id=i;
        pthread_create(&Thread[i],NULL,Thread_Function,&param);
        pthread_join(Thread[i],NULL);

        i++;
    }



    close(sock_server);
    printf("fin \n");

    return 0;
}