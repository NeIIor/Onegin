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

static bool onegin_str_compare_right (void* str1_im, void* str2_im);
static size_t onegin_f_num_str       (off_t size, char* text); 
static char* onegin_file_stat        (off_t* size_file);
static str_t* onegin_input           (size_t* n_str, char** text);
static bool onegin_str_compare_left  (void* str1_im, void* str2_im);
static void my_sort                  (void* arr, size_t size, size_t el_size, 
                                      bool (*str_compare) (void* a, void* b));
static void onegin_output            (str_t* arr_index, size_t n_str, FILE* oneg_w);
static void onegin_swap              (void* a, void* b, size_t el_size, void* buff);



int main() {
    FILE* oneg_w = fopen ("oneg_w.txt", "w");

    size_t n_str = 0;
    char* text = NULL;
    str_t* strings = onegin_input(&n_str, &text);
    printf("%p\t%u\n", strings, n_str);
    onegin_output (strings, n_str, oneg_w);
    my_sort (strings, n_str, sizeof(str_t), onegin_str_compare_left);
    onegin_output (strings, n_str, oneg_w);
    my_sort (strings, n_str, sizeof(str_t), onegin_str_compare_right);
    onegin_output (strings, n_str, oneg_w);
    free (strings);
    free (text); 
    fclose(oneg_w);

    return 0;
}

size_t onegin_f_num_str (off_t size, char* text) {
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

char* onegin_file_stat (off_t* size_file) {
    assert(size_file);

    FILE* oneg_r = fopen("oneg_r.txt", "rb"); 

    struct stat buf = {};
    stat ("oneg_r.txt", &buf);
    *size_file = buf.st_size;
    printf("size: %u\n", *size_file);

    char* text = (char*) calloc((size_t)(*size_file) + 1, sizeof(char));
    if (!text) {
        printf("Unluck with calloc text");
    }
    *size_file = fread(text, sizeof(char), (size_t) *size_file, oneg_r);
    printf("fread: %u\n", *size_file);

    text[*size_file] = '\0';
    fclose(oneg_r);
    return text;
}

str_t* onegin_input (size_t* n_str, char** text) { 
    assert(n_str);

    off_t size = 0;
    *text = onegin_file_stat(&size);
    printf("%u %p\n", size, *text);
    *n_str = onegin_f_num_str(size, *text);
    printf("n_str in input:%u\n", *n_str);

    str_t* arr_index = (str_t*) calloc(*n_str + 1, sizeof(str_t));
    if (!arr_index) {
        printf("Unluck with calloc arr_index");
    }
    
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

void onegin_output (str_t* arr_index, size_t n_str, FILE* oneg_w) {
    assert(arr_index);
    assert(oneg_w);

    for (size_t i = 0; i < n_str; i++) {
        fputs(arr_index[i].str, oneg_w);
    }
    fprintf(oneg_w, "\n"); 
}


void my_sort(void* arr, size_t size, size_t el_size, bool (*str_compare) (void* a, void* b)) {
    assert(arr);
    assert(str_compare);

    char* arr_ = (char*) arr;
    char* right = arr_ + (size - 1) * el_size;
    char* i = arr_;
    char* j = right;
    if ((size_t) arr_ > (size_t) right) return;
    void* tmp = (void*) calloc(el_size, 1);
    if (!tmp) {
        printf("Unluck with calloc buff");
    }
    char* mid = arr_ + ((size - 1) / 2) * el_size;
    
    while ((size_t) i <= (size_t) j) {
        while (true) {
            if ((size_t) i != (size_t) mid) {
                if (str_compare((void*) mid, (void*) i)) {
                    i = i + el_size;
                } else {
                    break;
                }
            } else {
                break;
            }
        }
        while (true) {
            if ((size_t) j != (size_t) mid) {
                if (str_compare((void*) j, (void*) mid)) {
                    j = j - el_size;
                } else {
                    break;
                }
            } else {
                break;
            }
        }
        if ((size_t) i <= (size_t) j) {
            onegin_swap((void*) i, (void*) j, el_size, tmp);
            if ((size_t) j == (size_t) mid) {
                mid = i;
            } else if ((size_t) i == (size_t) mid) {
                mid = j;
            }
            i = i + el_size;
            j = j - el_size;
        }   
    }
    free(tmp);
    my_sort (arr, (size_t) ((size_t) j - (size_t) arr) / el_size + 1, el_size, str_compare);
    my_sort ((void*) i, (size_t) ((size_t) right - (size_t) i) / el_size + 1, el_size, str_compare);
}

static void onegin_swap (void* a, void* b, size_t el_size, void* buff) { // some to do
    assert(a);
    assert(b);
    assert(buff);

    memcpy(buff, (void*) a, el_size);
    memcpy((void*) a, (void*) b, el_size);
    memcpy((void*) b, buff, el_size);
}

bool onegin_str_compare_right (void* str1_im, void* str2_im) {
    assert(str1_im);
    assert(str2_im);

    const str_t str1 = *(str_t*) str1_im;
    const str_t str2 = *(str_t*) str2_im;

    ssize_t i1 = str1.size - 1, i2 = str2.size - 1;
    
    while (i1 >= 0 && i2 >= 0) {
        if (!isalpha(str1.str[i1])) {
            i1--;
        } else if (!isalpha(str2.str[i2])) {
            i2--;
        } else if ((tolower(str1.str[i1]) - tolower(str2.str[i2])) > 0) {
            return true;
            break;
        } else if ((tolower(str1.str[i1]) - tolower(str2.str[i2])) < 0) {
            return false;
            break;
        } else {
            i1--;
            i2--;
        }
    }
    if (i2 == 0 && i1 != 0) {
        return true;
    } else {
        return false;
    }
}

bool onegin_str_compare_left (void* str1_im, void* str2_im) {
    assert(str1_im);
    assert(str2_im);

    const str_t str1 = *(str_t*) str1_im;
    const str_t str2 = *(str_t*) str2_im;

    size_t i1 = 0, i2 = 0;
    
    while (i1 < str1.size && i2 < str2.size) {
        if (!isalpha(str1.str[i1])) {
            i1++;
        } else if (!isalpha(str2.str[i2])) {
            i2++;
        } else if ((tolower(str1.str[i1]) - tolower(str2.str[i2])) > 0) {
            return true;
            break;
        } else if ((tolower(str1.str[i1]) - tolower(str2.str[i2])) < 0) {
            return false;
            break;
        } else {
            i1++;
            i2++;
        }
    }
    if (i2 == str2.size && i1 != str1.size) {
        return true;
    } else {
        return false;
    }
}