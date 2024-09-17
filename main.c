#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>

typedef struct texty {
    char* text;
    off_t size;
    size_t n_str;
    char** arr_index_r;
    char** arr_index_l;
} text_t;

void f_num_str           (text_t* onegin);
void input_text_stat   (text_t* onegin);
void input_arr         (text_t* onegin);
bool str_compare_left  (const char* const str1, const size_t, 
                        const char* const str2, const size_t len2);
bool str_compare_right (const char* const str1, const size_t len1, 
                        const char* const str2, const size_t len2);
void bubble_sort       (char** arr_str_l, char** arr_str_r, const size_t num_str);
void swap_str          (char** str1, char** str2);
void output_s_text     (char** const arr_index, const size_t num_str);



int main() { //\r до \n
    text_t onegin = {};
    input_text_stat (&onegin);
    input_arr(&onegin);
    f_num_str(&onegin);
    
    output_s_text (onegin.arr_index_l, onegin.n_str);
    printf("%p\t%p\t%d\t%d\n", onegin.text, onegin.arr_index_l, onegin.size, onegin.n_str);
    bubble_sort (onegin.arr_index_l, onegin.arr_index_r, onegin.n_str);
    output_s_text (onegin.arr_index_l, onegin.n_str);
    output_s_text (onegin.arr_index_r, onegin.n_str);
    free (onegin.arr_index_l);
    free (onegin.arr_index_r);
    printf("meow");
    return 0;
}

void f_num_str (text_t* onegin) {
    assert(onegin);

    for (off_t i = 0; i < onegin->size; i++) {
        if (onegin->text[i] == '\n') {
            (onegin->n_str)++;
        }
    }
    return;
}

void input_text_stat (text_t* onegin) {
    assert(onegin);

    FILE* oneg_r = fopen("onegin_read.txt", "rb");

    struct stat buf = {};
    stat ("onegin_read.txt", &buf);
    onegin->size = buf.st_size;

    onegin->text = (char*) calloc(onegin->size + 1, sizeof(char));
    onegin->size = fread(onegin->text, sizeof(char), (size_t) onegin->size, oneg_r);

    onegin->text[onegin->size] = '\0';
    fclose(oneg_r);
    return;
}

void input_arr (text_t* onegin) { 
    assert(onegin);

    onegin->arr_index_l = (char**) calloc(onegin->n_str, sizeof(char*));
    
    int count_str = 0;
    int j = 0;
    int count_alpha = 0;
    onegin->arr_index_l[0] = onegin->text;

    for (off_t i = 0; onegin->text[i] != '\0'; i++) {
        if (onegin->text[i] == '\n') {
            if (count_alpha == 0) {
                continue;
            }
            count_str++;
            onegin->arr_index_l[count_str] = &onegin->text[i+1];
            j = -1; 
            count_alpha = 0;
        } else if (isalpha(onegin->text[i])) {
            count_alpha++;
        } else if (onegin->text[i] == '\r') {
            (onegin->arr_index_l)[count_str][j] = '\0';
        } else {
            onegin->arr_index_l[count_str][j] = (onegin->text)[i];
        }
        j++;
    }
    onegin->arr_index_r = (char**) calloc(onegin->n_str, sizeof(char*));
    memcpy(onegin->arr_index_r, onegin->arr_index_l, onegin->n_str * sizeof(char*));
}


bool str_compare_left(const char* const str1, const size_t len1, 
                      const char* const str2, const size_t len2) {
    assert(str1);
    assert(str2);
 
    size_t i = 0, j = 0;
 
    while (i < len1 && j < len2) {
        if (!isalpha(str1[i])) {
            i++;
        } else if (!isalpha(str2[j])) {
            j++;
        } else if ((str1[i] - str2[j]) > 0) {
            return false;
        } else if ((str1[i] - str2[j]) < 0) {
            return true;
        } else {
            i++;
            j++;
        }
    }
}

bool str_compare_right (const char* const str1, const size_t len1, 
                        const char* const str2, const size_t len2) {
    assert(str1);
    assert(str2);
 
    size_t i = len1, j = len2;
 
    while (i > 0 && j > 0) {
        if (!isalpha(str1[i])) {
            i--;
        } else if (!isalpha(str2[j])) {
            j--;
        } else if ((str1[i] - str2[j]) > 0) {
            return false;
        } else if ((str1[i] - str2[j]) < 0) {
            return true;
        } else {
            i--;
            j--;
        }
    }
 return true;
}

void swap_str (char** str1, char** str2) {
    assert(str1);
    assert(str2);
 
    char* help_add = NULL;
    help_add = *str1;
    *str1 = *str2;
    *str2 = help_add; 
}

void bubble_sort (char** const arr_str_l, char** const arr_str_r,
                  const size_t num_str) {
    for (int i = 0; i < num_str; i++) {
        for (int j = 0; j < num_str - i - 1; j++) {
            if (!str_compare_left(arr_str_l[j], 
               ((size_t) arr_str_l[j+1] - (size_t) arr_str_l[j]), arr_str_l[j + 1], 
               ((size_t) arr_str_l[j+2] - (size_t) arr_str_l[j+1]))) {
                 swap_str (&arr_str_l[j], &arr_str_l[j + 1]);
            }
             if (!str_compare_right(arr_str_r[j],
                ((size_t) arr_str_r[j+1] - (size_t) arr_str_r[j]), arr_str_r[j + 1], 
                ((size_t) arr_str_l[j+2] - (size_t) arr_str_l[j+1]))) {
                  swap_str (&arr_str_r[j], &arr_str_r[j + 1]);
            }
        }
    }
}

void output_s_text (char** const arr_index, const size_t num_str) {
    assert(arr_index);

    FILE* oneg_w = fopen("onegin_output.txt", "w");
    assert(oneg_w);

    for (int i = 0; i < num_str; i++) {
        fputs(arr_index[i], oneg_w);
        fprintf(oneg_w, "\n");
    }
    fclose(oneg_w);
}

/*
problem:
use binary file (test trash)
save 1 version
fputs it after
change \r on ...
output?

struct $$
num_str like  $$
calloc <- for {realloc} $$
fputs for output not sorted text of onegin (do not delete \r n \n) $$
qsort

*/


