#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rand_malloc.h"
#include <stdbool.h>

char *readline(bool *fail)
{
    char *line = NULL;
    int c;
    size_t bufsize = 0;
    size_t size = 0;
    while ((c = getchar()) != EOF)
    {
        if(c == '\n')
        {
            break;
        }
        if (size >= bufsize)
        {
            char *newbuf;
            if (bufsize == 0)
                bufsize = 2;
            else if (bufsize <= ((size_t)-1) / 2)
                bufsize = 2 * size;
            else
            {
                free(line);
                *fail = true;
                return NULL;
            }
            newbuf = realloc(line, bufsize);
            if (!newbuf)
            {
                free(line);
                *fail = true;
                return NULL;
            }
            line = newbuf;
        }
        line[size++] = c;
        if (c == '\n')
            break;
    }
    if ((c == EOF) && (size == 0))
        return NULL;
    if (size >= bufsize)
    {
        char *newbuf;
        if (size < (size_t)-1)
            bufsize = size + 1;
        else
        {
            free(line);
            *fail = true;
            return NULL;
        }
        newbuf = realloc(line, bufsize);
        if (!newbuf)
        {
            free(line);
            *fail = true;
            return NULL;
        }
        line = newbuf;
    }
    line[size++] = '\0';
    return line;
}

void reverse_words(char* line) {
    char* start = line;
    char* end = strchr(line, '\0') - 1;
    while (start < end) {
        char tmp = *start;
        *start = *end;
        *end = tmp;
        start++;
        end--;
    }

    start = line;
    while (*start != '\0') {
        char* word_end = strchr(start, ' ');
        if (word_end == NULL) {
            word_end = strchr(start, '\n');
            if (word_end == NULL) {
                word_end = strchr(start, '\0');
            }
        }
        if (word_end != NULL) {
            end = word_end - 1;
            while (start < end) {
                char tmp = *start;
                *start = *end;
                *end = tmp;
                start++;
                end--;
            }
        }
        if (*word_end == ' ') {
            start = word_end + 1;
        } else {
            start = word_end;
        }
    }
}

int main()
{
    char **lines = NULL;
    bool is_failure = false;
    size_t nolines = 0;
    size_t nolinesmax = 0;
    char *line;
    size_t i;
    while ((line = readline(&is_failure)))
    {
        if (nolines >= nolinesmax)
        {
            char **newlines;
            if (nolines == 0)
                nolinesmax = 1;
            else if (nolines <= ((size_t)-1) / 2 / sizeof(char *))
            {
                nolinesmax = 2 * nolines;
            }
            else
                goto error;
            newlines = realloc(lines,
                               nolinesmax * sizeof(char *));
            if (newlines == NULL)
                goto error;
            lines = newlines;
        }
        reverse_words(line);
        lines[nolines++] = line;
    }
    if(is_failure){
        goto error;
    }
    printf("\n");
    for (i = nolines; i > 0; i--)
    {
        printf("%s", lines[i - 1]);
        free(lines[i - 1]);
        printf("\n");
    }
    free(lines);
    return 0;
error:
    for (i = nolines; i > 0; i--)
        free(lines[i - 1]);
    free(lines);
    free(line);
    exit(1);
}
