#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

/* These functions compute the size of a file in either C++ or C.
 */
#ifdef __cplusplus
    #include <fstream>

    static size_t filesize(const char* filename)
    {
        std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
        return (size_t)in.tellg();
    }
#else
    static size_t filesize(const char* filename)
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
    fprintf(stderr,
            "usage: %s (sequential|random|delete|sequentialstring|randomstring|deletestring|kjvmark) <maxkeys> <increment>\n",
            program);
    exit(1);
}

/* This is a non portable Linux function that returns the amount of memory in Kbytes used for data by the process
 */
static unsigned long get_data_mem(void)
{
    unsigned long data;
    unsigned long discard;
    FILE *        file = fopen("/proc/self/statm", "r");

    if (file == NULL) {
        fprintf(stderr, "Failed to open /proc/self/statm\n");
        exit(1);
    }

    char line[256];
    fgets(line, sizeof(line), file);

    if (sscanf(line, "%lu %lu %lu %lu %lu %lu %lu", &discard, &discard, &discard, &discard, &discard, &data, &discard) != 7) {
        fprintf(stderr, "Failed to scan 7 integers from /proc/self/statm\n");
        exit(1);
    }

    fclose(file);
    return data * getpagesize();
}

static double get_time(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (tv.tv_usec / 1000000.0);
}

static char * new_string_from_integer(int num)
{
    int ndigits = num == 0 ? 1 : (int)log10(num) + 1;
    char * str = (char *)malloc(ndigits + 1);
    sprintf(str, "%d", num);
    return str;
}

static int           interval;
static double        startTime;
static unsigned long startData;

static void printPoint(int i) {
    if ((i + 1) % interval != 0)
        return;

    double        after = get_time();
    unsigned long data  = get_data_mem();
    printf("%f %lu\n", after - startTime, data - startData);
    fflush(stdout);
}

int main(int argc, char ** argv)
{
    if (argc <= 3)
        usage(argv[0]);

    char * benchmark = argv[1];
    int    num_keys  = atoi(argv[2]);
    int    value     = 0;
    int    i;

    SETUP

    interval  = atoi(argv[3]);
    startTime = get_time();
    startData = get_data_mem();

    if (!strcmp(benchmark, "sequential"))
    {
        for(i = 0; i < num_keys; i++) {
            INSERT_INT_INTO_HASH(i, value);
            printPoint(i);
        }
    }

    else if (!strcmp(benchmark, "random"))
    {
        srandom(1); // for a fair/deterministic comparison

        for(i = 0; i < num_keys; i++) {
            INSERT_INT_INTO_HASH((int)random(), value);
            printPoint(i);
        }
    }

    else if (!strcmp(benchmark, "delete"))
    {
        for(i = 0; i < num_keys; i++)
            INSERT_INT_INTO_HASH(i, value);

        startTime = get_time();

        for(i = 0; i < num_keys; i++) {
            DELETE_INT_FROM_HASH(i);
            printPoint(i);
        }
    }

    else if (!strcmp(benchmark, "sequentialstring"))
    {
        for(i = 0; i < num_keys; i++) {
            INSERT_STR_INTO_HASH(new_string_from_integer(i), value);
            printPoint(i);
        }
    }

    else if (!strcmp(benchmark, "randomstring"))
    {
        srandom(1); // for a fair/deterministic comparison

        for(i = 0; i < num_keys; i++) {
            INSERT_STR_INTO_HASH(new_string_from_integer((int)random()), value);
            printPoint(i);
        }
    }

    else if (!strcmp(benchmark, "deletestring"))
    {
        for(i = 0; i < num_keys; i++)
            INSERT_STR_INTO_HASH(new_string_from_integer(i), value);

        startTime = get_time();

        for(i = 0; i < num_keys; i++) {
            DELETE_STR_FROM_HASH(new_string_from_integer(i));
            printPoint(i);
        }
    }

    /* This benchmark counts the number of distinct words (strings on nonspace characters) in the king james bible.
     */
    else if (!strcmp(benchmark, "kjvmark"))
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

        // Reset start time and memory now that preparations are done
        startTime = get_time();
        startData = get_data_mem();

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

        printPoint(-1);    // -1 forces printing
    }

    else {
        fprintf(stderr, "%s: No such benchmark '%s'", argv[0], benchmark);
        usage(argv[0]);
    }
}
