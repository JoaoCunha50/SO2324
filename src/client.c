#include "../include/client.h"

int main(int argc, char *argv[])
{
    Comandos comando;
    strcpy(comando.command, argv[1]); // para ir buscar o comando para execução

    int fifo_servidor = open(SERVIDOR, O_WRONLY);
    if (fifo_servidor < 0)
    {
        write(2, "Error opening reading fifo.\n", 29);
        exit(EXIT_FAILURE);
    }

    if (argc < 2)
    {
        write(2, "Invalid number of arguments.\n", 30);
        exit(EXIT_FAILURE);
    }

    char buffer[124];

    if (strcmp(argv[1], "status") == 0)
    {
        write(fifo_servidor, &comando, sizeof(Comandos));

        int fifo_cliente = open(CLIENTE, O_RDONLY);

        char logs_read[4096];
        ssize_t bytes_lidos = read(fifo_cliente, logs_read, sizeof(logs_read));
        if (bytes_lidos <= 0)
        {
            perror("Erro ao ler do fifo\n");
            exit(EXIT_FAILURE);
        }
        logs_read[bytes_lidos] = '\0';

        ssize_t bytes_escritos = write(1, logs_read, strlen(logs_read));
        {
            if (bytes_escritos <= 0)
            {
                perror("Erro ao escrever no STDOUT\n");
                exit(EXIT_FAILURE);
            }
        }
        close(fifo_cliente);
    }
    else if (strcmp(argv[1], "execute") == 0)
    {
        comando.estimated_time = atoi(argv[2]);
        strcpy(comando.prog_name, argv[4]);
        strcpy(comando.flag, argv[3]);

        if (strcmp(argv[3], "-u") == 0 || strcmp(argv[3], "-p") == 0)
        {
            ssize_t bytes_escritos = write(fifo_servidor, &comando, sizeof(Comandos));
            if (bytes_escritos <= 0)
            {
                write(2, "Erro ao escrever no FIFO\n", 25);
                exit(EXIT_FAILURE);
            }
        }
        int fifo_cliente = open(CLIENTE, O_RDONLY);

        int id = 0;
        ssize_t bytes_lidos = read(fifo_cliente, &id, sizeof(int));
        if (bytes_lidos <= 0)
        {
            perror("Erro ao ler do fifo\n");
            exit(EXIT_FAILURE);
        }
        snprintf(buffer, sizeof(buffer), "Task %d received\n", id);
        write(1, buffer, strlen(buffer));
        close(fifo_cliente);

    }
    else if (strcmp(argv[1], "shutdown") == 0)
    {
        int bytes_escritos = write(fifo_servidor, &comando, sizeof(Comandos));
        if (bytes_escritos <= 0)
        {
            write(2, "Erro ao escrever no FIFO\n", 25);
            exit(EXIT_FAILURE);
        }
        write(1, "Server Terminado...\n", 20);
    }

    close(fifo_servidor);
    return 0;
}