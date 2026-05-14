#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

int to_digit(char c, int base)
{
	int digit;

	if (isdigit((unsigned char)c)) {
		digit = c - '0';
	}
	else if (isalpha((unsigned char)c)) {
		digit = tolower((unsigned char)c) - 'a' + 10;
	}
	else {
		return -1;
	}

	if (digit >= base) {
		return -1;
	}
	return digit;
}

int get_base(const char** s, int base)
{
	if ((base == 0 || base == 16) && **s == '0' &&
		((*s)[1] == 'x' || (*s)[1] == 'X') && (to_digit((*s)[2], 16) != -1)) {
		*s += 2;
		base = 16;
	}
	else if (base == 0) {
		if (**s == '0') {
			base = 8;
		}
		else {
			base = 10;
		}
	}
	return base;
}

int right_base(const char* nPtr, char** endPtr, int base)
{
	if (((base > 36) || (base < 2)) && base != 0) {
		if (endPtr)
			*endPtr = (char*)nPtr;
		errno = EINVAL;
		return 0;
	}
	return 1;
}

int get_sign(const char** s)
{
	int sign = 1;
	if (**s == '-') {
		sign = -1;
		(*s)++;
	}
	else if (**s == '+') {
		(*s)++;
	}
	return sign;
}

long get_positive_result(const char** s_ptr, int base, int sign,
						 long max)
{
	long result = 0;
	while (1) {
		int digit = to_digit(**s_ptr, base);
		if ((digit) == -1)
			break;

		if ((result > max / base) ||
			(result == max / base && digit > max % base)) {
			errno = ERANGE;
			while (1) {
				int d = to_digit(**s_ptr, base);
				if ((d) == -1)
					break;
				(*s_ptr)++;
			}
			break;
		}
		else 
			result = result * base + digit;
		(*s_ptr)++;
	}
	return result;
}

long get_negative_result(const char** s_ptr, int base, int sign,
						 long min)
{
	long result = 0;
	while (1) {
		int digit = to_digit(**s_ptr, base);
		if ((digit) == -1)
			break;
		int neg_digit = -digit;

		if ((result < min / base) ||
			(result == min / base && neg_digit < min % base)) {
			errno = ERANGE;
			while (1) {
				int d = to_digit(**s_ptr, base);
				if ((d) == -1)
					break;
				(*s_ptr)++;
			}
			break;
		}
		else 
			result = result * base + neg_digit;
		(*s_ptr)++;
	}
	return result;
}

long count(const char** s_ptr, int base, int sign)
{
	if (sign == 1){
		return get_positive_result(s_ptr, base, sign, LONG_MAX);
	}
	else{
		return get_negative_result(s_ptr, base, sign, LONG_MIN);
	}
}

void set_pointer(const char* nPtr, char** endPtr, const char* s,
				 const char* temp)
{
	if (endPtr) {
		if (s == temp) {
			*endPtr = (char*)nPtr;
		}
		else {
			*endPtr = (char*)s;
		}
	}
}

long ret_err(int sign)
{
	if (sign == 1)
		return LONG_MAX;
	else
		return LONG_MIN;
}

long strtol(const char* nPtr, char** endPtr, int base)
{
	if (!(right_base(nPtr, endPtr, base)))
		return 0;

	const char* s = nPtr;
	int sign;

	while (isspace((unsigned char)*s)) {
		s++;
	}

	sign = get_sign(&s);
	base = get_base(&s, base);

	const char* temp = s;
	long result = count(&s, base, sign);

	set_pointer(nPtr, endPtr, s, temp);
	if (errno == ERANGE){
		return ret_err(sign);
	}
	return result;
}

