#include "rand_malloc.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char* getLine();
void clean(char** numbers, size_t count, char* sum_string);
int is_valid_octal(char* str);
void reverse_string(char* str, int len);
char** read_valid_numbers(size_t* out_count);
char* calculate_total_sum(char** numbers, size_t count);
char* add_octal(char* s1, char* s2);
void* handle_allocation_error(char* line, char** numbers, size_t count);
char* trim_whitespace(char* line);


int main()
{
    size_t count = 0;
    char** numbers = read_valid_numbers(&count);   
    if (numbers == NULL) {
        return 1; 
    }
    char* total_sum = calculate_total_sum(numbers, count);
    if (total_sum == NULL) {
        clean(numbers, count, NULL);
        printf("Error: Couldn't allocate memory\n");
        return 1; 
    }
    printf("Sum:\n%s\n\n", total_sum);
    printf("Input numbers:\n");
    for (size_t i = 0; i < count; i++) {
        printf("%s\n", numbers[i]);
    }
    clean(numbers, count, total_sum);
    return 0;
}


char* trim_whitespace(char* line) {
    char* start = line;
    while (isspace((unsigned char)*start)) {
        start++;
    }
    if (*start != '\n' && *start != '\0') {
        char* end = start + strlen(start) - 1;
        while (end > start && isspace((unsigned char)*end)) {
            end--;
        }
        *(end + 1) = '\0';
    }
    return start;
}

char** read_valid_numbers(size_t* out_count) {
    size_t capacity = 1, count = 0;
    char **numbers = malloc(sizeof(char*)), *line, *start;
    if (!numbers) return handle_allocation_error(NULL, NULL, 0);
    while ((line = getLine()) != NULL) {
        start = trim_whitespace(line);        
        if (*start == '\0') {
            free(line);
            continue;
        }      
        if (!is_valid_octal(start)) {
            return handle_allocation_error(line, numbers, count);
        }
        if (count >= capacity) {
            capacity *= 2;
            char** temp = realloc(numbers, capacity * sizeof(char*));
            if (!temp) return handle_allocation_error(line, numbers, count);
            numbers = temp;
        }
        numbers[count] = malloc(strlen(start) + 1);
        if (!numbers[count]) return handle_allocation_error(line, numbers, count);        
        strcpy(numbers[count++], start);
        free(line);
    }
    if (!feof(stdin)) return handle_allocation_error(NULL, numbers, count); 
    *out_count = count;
    return numbers;
}

void* handle_allocation_error(char* line, char** numbers, size_t count){
	printf("Error: Couldn't allocate memory\n");
	free(line);
	clean(numbers, count, NULL);
	return NULL;
}

char* calculate_total_sum(char** numbers, size_t count)
{
    char* total_sum = malloc(2);
    if (!total_sum) return NULL;
    strcpy(total_sum, "0");
    for (size_t i = 0; i < count; i++) {
        char* next_sum = add_octal(total_sum, numbers[i]);
        if (next_sum == NULL) {
            free(total_sum);
            return NULL;
        }
        free(total_sum);
        total_sum = next_sum;
    }
    return total_sum;
}

void reverse_string(char* str, int len)
{
    for (int start = 0, end = len - 1; start < end; start++, end--) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
    }
}

char* getLine()
{
	size_t capacity = 16;
	size_t lenght = 0;

	char* buffer = malloc(capacity);
	if (buffer == NULL) {
		return NULL;
	}
	int ch;
	while ((ch = getchar()) != EOF && ch != '\n') {
		if (lenght + 1 >= capacity) {
			capacity = capacity * 2;
			char* temp = realloc(buffer, capacity);
			if (temp == NULL) {
				free(buffer);
				return NULL;
			}
			buffer = temp;
		}
		buffer[lenght++] = (char)ch;
	}
	if (ch == EOF && lenght == 0) {
		free(buffer);
		return NULL;
	}
	buffer[lenght] = '\0';
	return buffer;
}

void clean(char** numbers, size_t count, char* sum_string)
{
	if (numbers != NULL) {
		for (size_t i = 0; i < count; i++) {
			free(numbers[i]);
		}
		free(numbers);
	}
	free(sum_string);
}

int is_valid_octal(char* str)
{
	if (str == NULL || *str == '\0') {
		return 0;
	}
	while (*str) {
		if (*str < '0' || *str > '7') {
			return 0;
		}
		str++;
	}
	return 1;
}

char* add_octal(char* s1, char* s2)
{
    int len1 = strlen(s1), len2 = strlen(s2), max_len = len1;
    if (len2 > len1) max_len = len2;
    char* result = malloc(max_len + 2);
    if (result == NULL) {
        return NULL;
    }
    int i = len1 - 1, j = len2 - 1, k = 0, carry = 0;
	while (i >= 0 || j >= 0 || carry) {
        int sum = carry;
        if (i >= 0) {
            sum = sum + (s1[i] - '0');
            i--;
        }       
        if (j >= 0) {
            sum = sum + (s2[j] - '0');
            j--;
        }
        if (sum >= 8) {
            result[k] = (sum - 8) + '0';
            carry = 1;
        } else {
            result[k] = sum + '0';
            carry = 0;
        }
        k++;
    }   
    result[k] = '\0';
    reverse_string(result, k);  
    return result;
}
