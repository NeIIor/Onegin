#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>


typedef struct {
    char* str;
    size_t size;
} str_t;

static bool str_compare_right (void* str1_im, void* str2_im);
static size_t f_num_str       (off_t size, char* text); /*either header or static*/
static char* file_stat        (off_t* size_file);
static str_t* input_oneg      (size_t* n_str, char** text);
static bool str_compare_left  (void* str1_im, void* str2_im);
static void my_sort           (void* arr, size_t size, size_t el_size, bool (*str_compare) (void* a, void* b));
static void output_s_text     (str_t* arr_index, size_t n_str, FILE* oneg_w);



int main() {
    FILE* oneg_w = fopen ("oneg_w.txt", "w");

    size_t n_str;
    char* text;
    str_t* strings = input_oneg(&n_str, &text);
    printf("%p\t%u\n", strings, n_str);
    output_s_text (strings, n_str, oneg_w);
    my_sort (strings, n_str, sizeof(str_t), str_compare_left);
    output_s_text (strings, n_str, oneg_w);
    my_sort (strings, n_str, sizeof(str_t), str_compare_right);
    output_s_text (strings, n_str, oneg_w);
    free (strings);
    free (text); 
    fclose(oneg_w);

    return 0;
}

size_t f_num_str (off_t size, char* text) {
    assert(text);

    size_t n_str = 0;
    for (off_t i = 0; i < size; i++) {
        if (text[i] == '\n') {
            n_str++;
        }
    }
    printf("n_str: %u\n", n_str);
    return n_str;
}

char* file_stat (off_t* size_file) {
    assert(size_file);

    FILE* oneg_r = fopen("oneg_r.txt", "rb"); 

    struct stat buf = {};
    stat ("oneg_r.txt", &buf);
    *size_file = buf.st_size;
    printf("size: %u\n", *size_file);

    char* text = (char*) calloc((size_t)(*size_file) + 1, sizeof(char));
    *size_file = fread(text, sizeof(char), (size_t) *size_file, oneg_r);
    printf("fread: %u\n", *size_file);

    text[*size_file] = '\0';
    fclose(oneg_r);
    return text;
}

str_t* input_oneg (size_t* n_str, char** text) { 
    off_t size = 0;
    *text = file_stat(&size);
    printf("%u %p\n", size, *text);
    *n_str = f_num_str(size, *text);
    printf("n_str in input:%u\n", *n_str);

    str_t* arr_index = (str_t*) calloc(*n_str + 1, sizeof(str_t));
    
    size_t count_str = 0;
    int j = 0;
    int count_alpha = 0;
    printf("%d\t%d\n", (*text)[0], (*text)[1]);
    for (off_t i = 0; (*text)[i] != '\0'; i++) {
        if ((*text)[i] == '\n') {
            if (count_alpha == 0) {
                j = 1;
                continue;
            }
            arr_index[count_str].str = *text + i - j;
            arr_index[count_str].size = j;
            count_str++;
            j = 0; 
            count_alpha = 0;
        } else if (isalpha((*text)[i])) { 
            count_alpha++;
        } else if ((*text)[i] == '\r') {
            (*text)[i] = '\0';
        }
        j++;
    }
    *n_str = count_str;
    return arr_index;
}

void output_s_text (str_t* arr_index, size_t n_str, FILE* oneg_w) {
    for (size_t i = 0; i < n_str; i++) {
        fputs(arr_index[i].str, oneg_w);
    }
    fprintf(oneg_w, "\n"); 
}


void my_sort(void* arr, size_t size, size_t el_size, bool (*str_compare) (void* a, void* b)) {
    char* arr_ = (char*) arr;
    char* right = arr_ + (size - 1) * el_size;
    char* i = arr_;
    char* j = right;
    if ((size_t) arr_ > (size_t) right) return;
    void* tmp = calloc(el_size, 1);
    char* mid = arr_ + (size / 2) * el_size;
    while ((size_t) i <= (size_t) j) {
        while (str_compare((void*) mid, (void*) i)) 
            i = i + el_size;
        while (str_compare((void*) j,   (void*) mid)) 
            j = j - el_size;
        if ((size_t) i <= (size_t) j) {
            memcpy(tmp, (void*) i, el_size);
            memcpy((void*) i, (void*) j, el_size);
            memcpy((void*) j, tmp, el_size);
            i = i + el_size;
            j = j - el_size;
        }   
    }
    free(tmp);
    my_sort (arr, (size_t) ((size_t) j - (size_t) arr) / el_size + 1, el_size, str_compare);
    my_sort ((void*) i, (size_t) ((size_t) right - (size_t) i) / el_size + 1, el_size, str_compare);
}

bool str_compare_right (void* str1_im, void* str2_im) {
    assert(str1_im);
    assert(str2_im);

    const str_t str1 = *(str_t*) str1_im;
    const str_t str2 = *(str_t*) str2_im;
 
    int i = (int) str1.size - 1, j = (int) str2.size - 1;
    
    while (i >= 0 && j >= 0) {
        if (!isalpha(str1.str[i])) {
            i--;
        } else if (!isalpha(str2.str[j])) {
            j--;
        } else if ((tolower(str1.str[i]) - tolower(str2.str[j])) > 0) {
            return true;
            break;
        } else if ((tolower(str1.str[i]) - tolower(str2.str[j])) < 0) {
            return false;
            break;
        } else {
            i--;
            j--;
        }
    }
    return false;
}

bool str_compare_left (void* str1_im, void* str2_im) {
    assert(str1_im);
    assert(str2_im);

    const str_t str1 = *(str_t*) str1_im;
    const str_t str2 = *(str_t*) str2_im;
 
    size_t i = 0, j = 0;
    
    while (i < str1.size && j < str2.size) {
        if (!isalpha(str1.str[i])) {
            i++;
        } else if (!isalpha(str2.str[j])) {
            j++;
        } else if ((tolower(str1.str[i]) - tolower(str2.str[j])) > 0) {
            return true;
            break;
        } else if ((tolower(str1.str[i]) - tolower(str2.str[j])) < 0) {
            return false;
            break;
        } else {
            i++;
            j++;
        }
    }
    return false;
}