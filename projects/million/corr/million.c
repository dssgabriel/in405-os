#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "se_fichier.h"

int srv_create_tubes() {
    int rc;

    rc = mkfifo("tube_in", 0666);
    if (rc) {
        fprintf(stderr, "ERR: creating tube_in\n");
        return -1;
    }

    rc = mkfifo("tube_out", 0666);
    if (rc) {
        fprintf(stderr, "ERR: creating tube_out\n");
        unlink("tube_in");
        return -1;
    }

    return 0;
}

void srv_delete_tubes() {
    unlink("tube_in");
    unlink("tube_out");
}

int srv_open_tubes(int *fd_in, int *fd_out) {
    *fd_in = open("tube_in", O_RDONLY);
    if (*fd_in < 0) {
        fprintf(stderr, "ERR: opening tube_in\n");
        return -1;
    }

    *fd_out = open("tube_out", O_WRONLY);
    if (*fd_out < 0) {
        fprintf(stderr, "ERR: opening tube_out\n");
        close(*fd_in);
        return -1;
    }

    return 0;
}

void srv_close_tubes(int fd_in, int fd_out) {
    close(fd_in);
    close(fd_out);
}

bool srv_check_duplicates(int size_array, int *array) {
    int i, j;

    for (i = 0; i < size_array; ++i)
        for (j = i + 1; j < size_array; ++j)
            if (array[i] == array[j])
                return true;

    return false;
}

int srv_retrieve_config(const char *cfg_path, int *nb_numbers,
                        int **correct_numbers, int **gain) {
    SE_FICHIER sef;
    int cnt;
    int rc;
    int i;
    
    sef = SE_ouverture(cfg_path, O_RDONLY);
    if (sef.descripteur < 0) {
        fprintf(stderr, "ERR: opening cfg file\n");
        return -1;
    }

    cnt = SE_lectureEntier(sef, nb_numbers);
    if (cnt <= 0) {
        fprintf(stderr, "ERR: missing nb_numbers\n");
        goto err_cls;
    }

    *correct_numbers = malloc(*nb_numbers * sizeof(**correct_numbers));
    if (! *correct_numbers) {
        fprintf(stderr, "ERR: allocating numbers\n");
        goto err_cls;
    }

    *gain = calloc(*nb_numbers + 1, sizeof(**gain));
    if (! *gain) {
        fprintf(stderr, "ERR: allocating gain\n");
        goto err_rel_nb;
    }

    for (i = 0; i < *nb_numbers; ++i) {
        cnt = SE_lectureEntier(sef, (*correct_numbers) + i);
        if (cnt <= 0) {
            fprintf(stderr, "ERR: missing numbers\n");
            goto err_rel_gn;
        }
    }

    if (srv_check_duplicates(*nb_numbers, *correct_numbers)) {
        fprintf(stderr, "ERR: dupplicate numbers\n");
        goto err_rel_gn;
    }

    for (i = 0; i < *nb_numbers; ++i) {
        int idx;

        cnt = SE_lectureEntier(sef, &idx);
        if (cnt <= 0) {
            fprintf(stderr, "ERR: missing gain index\n");
            goto err_rel_gn;
        }

        if (idx > *nb_numbers || idx < 0) {
            fprintf(stderr, "ERR: incorrect gain index\n");
            goto err_rel_gn;
        }

        cnt = SE_lectureEntier(sef, (*gain) + idx);
        if (cnt <= 0) {
            fprintf(stderr, "ERR: missing gain value\n");
            goto err_rel_gn;
        }
    }

    SE_fermeture(sef);
    return 0;

err_rel_gn:
    free(*gain);

err_rel_nb:
    free(*correct_numbers);

err_cls:
    SE_fermeture(sef);
    return -1;
}

void srv_release_alloc(int *correct_numbers, int *gain) {
    free(correct_numbers);
    free(gain);
}

int srv_receive_numbers(int fd, int nb_numbers, int *client_numbers) {
    int nb_client_numbers;
    ssize_t cnt;
    int i;

    cnt = read(fd, &nb_client_numbers, sizeof(nb_client_numbers));
    if (cnt != sizeof(nb_client_numbers)) {
        fprintf(stderr, "ERR: reading tube_in\n");
        return -1;
    }

    if (nb_client_numbers != nb_numbers)
        fprintf(stderr, "ERR: incorrect number of client numbers\n");

    for (i = 0; i < nb_client_numbers; ++i) {
        if (nb_client_numbers != nb_numbers) {
            read(fd, client_numbers, sizeof(*client_numbers));
            continue;
        }

        cnt = read(fd, client_numbers + i, sizeof(*client_numbers));
        if (cnt != sizeof(*client_numbers)) {
            fprintf(stderr, "ERR: reading tube_in\n");
            return -1;
        }
    }

    return (nb_client_numbers != nb_numbers) ? 1 : 0;
}

int srv_send_error(int fd, int err) {
    ssize_t cnt;

    cnt = write(fd, &err, sizeof(err));
    if (cnt != sizeof(err)) {
        fprintf(stderr, "ERR: writing tube_out\n");
        return -1;
    }

    return 0;
}

int srv_send_result(int fd, int nb_correct_numbers, int gain) {
    ssize_t cnt;

    cnt = write(fd, &nb_correct_numbers, sizeof(nb_correct_numbers));
    if (cnt != sizeof(nb_correct_numbers)) {
        fprintf(stderr, "ERR: writing tube_out\n");
        return -1;
    }

    cnt = write(fd, &gain, sizeof(gain));
    if (cnt != sizeof(gain)) {
        fprintf(stderr, "ERR: writing tube_out\n");
        return -1;
    }

    return 0;
}

int srv_count_correct_numbers(int nb_correct_numbers, int *client_numbers,
                              int *server_numbers) {
    int res = 0;
    int i, j;

    for (i = 0; i < nb_correct_numbers; ++i) {
        for (j = 0; j < nb_correct_numbers; ++j) {
            if (client_numbers[i] == server_numbers[j]) {
                ++res;
                break;
            }
        }
    }

    return res;
}

int server(int argc, char **argv) {
    int *client_numbers, *correct_numbers;
    int nb_correct_numbers, nb_numbers;
    int fd_in, fd_out;
    int *gain;
    int rc;

    rc = srv_create_tubes();
    if (rc)
        return rc;

    if (argc != 1) {
        fprintf(stderr, "ERR: usage - need the configuration path\n");
        goto err_del;
    }

    rc = srv_retrieve_config(argv[0], &nb_numbers, &correct_numbers, &gain);
    if (rc)
        goto err_del;

    while (1) {
        rc = srv_open_tubes(&fd_in, &fd_out);
        if (rc)
            goto err_del;

        rc = srv_receive_numbers(fd_in, nb_numbers, client_numbers);
        if (rc < 0)
            goto err_cls;

        if (rc == 1) {
            rc = srv_send_error(fd_out, -1);
            if (rc)
                goto err_cls;

            continue;
        }

        if (srv_check_duplicates(nb_numbers, client_numbers)) {
            rc = srv_send_error(fd_out, -2);
            if (rc)
                goto err_cls;

            continue;
        }

        nb_correct_numbers = srv_count_correct_numbers(nb_numbers,
                                                       client_numbers,
                                                       correct_numbers);

        rc = srv_send_result(fd_out, nb_correct_numbers,
                             gain[nb_correct_numbers]);
        if (rc)
            goto err_cls;

        if (nb_correct_numbers == nb_numbers)
            break;

        srv_close_tubes(fd_in, fd_out);
    }

err_cls:
    srv_close_tubes(fd_in, fd_out);

err_del:
    srv_delete_tubes();
    return rc;
}

int clt_send_numbers(int argc, char **argv) {
    ssize_t cnt;
    int fd;
    int i;

    fd = open("tube_in", O_WRONLY);
    if (fd < 0) {
        if (errno == ENOENT) {
            fprintf(stderr, "ERR: server is not online\n");
            return 2;
        }

        fprintf(stderr, "ERR: opening wr tube\n");
        return -1;
    }

    cnt = write(fd, &argc, sizeof(argc));
    if (cnt != sizeof(argc)) {
        fprintf(stderr, "ERR: writing value to tube\n");
        close(fd);
        return -1;
    }

    for (i = 0; i < argc; ++i) {
        int val = atoi(argv[i]);
        ssize_t cnt;
        
        cnt = write(fd, &val, sizeof(val));
        if (cnt != sizeof(val)) {
            fprintf(stderr, "ERR: writing value to tube\n");
            close(fd);
            return -1;
        }   
    }

    close(fd);
    return 0;
}

int clt_recv_result(int *nb_correct_numbers, int *gain) {
    ssize_t cnt;
    int fd;

    fd = open("tube_out", O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "ERR: opening rd tube\n");
        return -1;
    }

    cnt = read(fd, nb_correct_numbers, sizeof(*nb_correct_numbers));
    if (cnt != sizeof(*nb_correct_numbers)) {
        fprintf(stderr, "ERR: reading value from tube\n");
        close(fd);
        return -1;
    }

    if (*nb_correct_numbers == -1) {
        fprintf(stderr, "ERR: incorrect number of numbers\n");
        close(fd);
        return -1;
    }

    if (*nb_correct_numbers == -2) {
        fprintf(stderr, "ERR: duplicate numbers\n");
        close(fd);
        return -1;
    }

    cnt = read(fd, gain, sizeof(*gain));
    if (cnt != sizeof(*gain)) {
        fprintf(stderr, "ERR: reading value from tube\n");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

int client(int argc, char **argv) {
    int nb_correct_numbers;
    int gain;
    int rc;

    if (!argc) {
        fprintf(stderr, "ERR: usage - need numbers\n");
        return -1;
    }

    rc = clt_send_numbers(argc, argv);
    if (rc)
        return rc;

    rc = clt_recv_result(&nb_correct_numbers, &gain);
    if (rc)
        return rc;

    if (nb_correct_numbers == argc)
        printf("!!! JACKPOT !!!\n");

    printf("You just gained %d dollars (%d correct numbers)\n",
           gain, nb_correct_numbers);

    return (nb_correct_numbers == argc) ? 1 : 0;
}

int main(int argc, char **argv) {
    char cmd[16];
    int a;

    if (argc > 1 && !strcmp(argv[1], "server"))
        return server(argc - 2, argv + 2);

    if (argc > 1 && !strcmp(argv[1], "client"))
        return client(argc - 2, argv + 2);

    fprintf(stderr, "ERR: usage - need keyword {server|client}\n");
    return -1;
}
