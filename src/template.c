#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <fcntl.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <unistd.h>
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
    #include <fstream>
//    #define O_RDONLY 0    // Hack because C++ doesn't seem to like <fcntl.h>

    size_t filesize(const char* filename)
    {
        std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
        return (size_t)in.tellg();
    }
#else
    #include <fcntl.h>

    size_t filesize(const char* filename)
    {
        struct stat fileStat;

        if (stat(filename, &fileStat) < 0) {
            fprintf(stderr, "Failed to stat %s\n", filename);
            exit(1);
        }

        return fileStat.st_size;
    }
#endif

static void usage(const char * program)
{
    fprintf(stderr, "usage: %s <numkeys> (sequential|random|delete|sequentialstring|randomstring|deletestring|kjvmark)\n", program);
    exit(1);
}

double get_time(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (tv.tv_usec / 1000000.0);
}

char * new_string_from_integer(int num)
{
    int ndigits = num == 0 ? 1 : (int)log10(num) + 1;
    char * str = (char *)malloc(ndigits + 1);
    sprintf(str, "%d", num);
    return str;
}

int main(int argc, char ** argv)
{
    int num_keys = atoi(argv[1]);
    int i, value = 0;

    if (argc <= 2)
        usage(argv[0]);

    SETUP

    double before = get_time();

    if(!strcmp(argv[2], "sequential"))
    {
        for(i = 0; i < num_keys; i++)
            INSERT_INT_INTO_HASH(i, value);
    }

    else if(!strcmp(argv[2], "random"))
    {
        srandom(1); // for a fair/deterministic comparison
        for(i = 0; i < num_keys; i++)
            INSERT_INT_INTO_HASH((int)random(), value);
    }

    else if(!strcmp(argv[2], "delete"))
    {
        for(i = 0; i < num_keys; i++)
            INSERT_INT_INTO_HASH(i, value);
        before = get_time();
        for(i = 0; i < num_keys; i++)
            DELETE_INT_FROM_HASH(i);
    }

    else if(!strcmp(argv[2], "sequentialstring"))
    {
        for(i = 0; i < num_keys; i++)
            INSERT_STR_INTO_HASH(new_string_from_integer(i), value);
    }

    else if(!strcmp(argv[2], "randomstring"))
    {
        srandom(1); // for a fair/deterministic comparison
        for(i = 0; i < num_keys; i++)
            INSERT_STR_INTO_HASH(new_string_from_integer((int)random()), value);
    }

    else if(!strcmp(argv[2], "deletestring"))
    {
        for(i = 0; i < num_keys; i++)
            INSERT_STR_INTO_HASH(new_string_from_integer(i), value);
        before = get_time();
        for(i = 0; i < num_keys; i++)
            DELETE_STR_FROM_HASH(new_string_from_integer(i));
    }

    /* This benchmark counts the number of distinct words (strings on nonspace characters) in the king james bible.
     */
    else if (strcmp(argv[2], "kjvmark") == 0)
    {
        const char * file = "data/king-james-bible.txt";

        if (argc >= 5 && strcmp(argv[3], "-f") == 0)
            file = argv[4];

        size_t size = filesize(file);
        char * text = (char *)malloc(size + 1);    // + 1 allows '\0' termination of last word
        int    fd;

        if ((fd = open(file, O_RDONLY)) < 0) {
            fprintf(stderr, "%s: Failed to open %s\n", argv[0], file);
            exit(1);
        }

        if (read(fd, text, size) != size) {
            fprintf(stderr, "%s: Failed to read %lu bytes from %s\n", argv[0], size, file);
            exit(1);
        }

        before = get_time();    // Reset start time now that preparations are done

        size_t start = 0;

        for (;;) {
            while (start < size && isspace(text[start]))
                start++;

            if (start == size)
                break;

            size_t endWord = start + 1;

            while (endWord < size && !isspace(text[endWord]))
                endWord++;

            text[endWord] = '\0';    // '\0' terminate the "word" (actually a space delimitted sequence)
            INSERT_STR_INTO_HASH(&text[start], value);
            start = endWord + 1;
        }

        if (STR_HASH_GET_SIZE() != 34027) {
            fprintf(stderr, "%s: Incorrect number of pseudo words were found in the KJV text (expected 34027, got %lu)\n", argv[0],
                    STR_HASH_GET_SIZE());
            exit(1);
        }
    }

    else
        usage(argv[0]);

    double after = get_time();
    printf("%f\n", after-before);
    fflush(stdout);
    sleep(1000000);
}
