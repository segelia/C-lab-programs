#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>

long int strtol(const char *nptr, char **endptr, int base)
{
	const char *p = nptr;

	// 0. Sprawdzanie czy p nie jest nullem i czy pozostałe argumenty są poprawne.
	if (p == NULL || base < 0 || base > 36 || base == 1)
	{
		if(endptr != NULL) *endptr = (char*)nptr;
		errno = EINVAL;
		return 0;
	}

	// 1. Usuwanie białych znaków.
	while (isspace(*p))
	{
		p++;
	}
	
	// 2. Sprawdzanie czy nie mamy znaku +-
	char sign = 1;
	if (p[0] == '+')
	{
		p++;
	}
	else if (p[0] == '-')
	{
		sign = -1;
		p++;
	}

	// 3. Sprawdzanie czy nie ma prefiksu 0 albo 0X lub 0x.
	// Jeśli base = 0 to sprawdzamy prefix i go skipujemy.
	// Jeśli base != 0 to prefix skipujemy o ile base ma odpowiednią wartość.
	
	// 3a. Samo rozpoznanie systemu jeśli taka jest potrzeba.
	if(base == 0)
	{
		base = 10;	// Zakładamy system dziesiętny.
		if(p[0] == '0')	// System może być oct albo hex.
		{
			// System ma base=10 jeśli po 0 nie ma nic.
			// System ma base=8 jeśli po 0 jest poprawna cyfra ósemkowa
			// System ma base=16 jeśli po 0 jest znak x i poprawna cyfra szesnastkowa.
			base = 8;
			if(toupper(p[1]) == 'X')
			{
				char c = toupper(p[2]);
				if((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'))
				{
					base = 16;
					p += 2;
				}
			}
		}
	}
	else if(base == 16)
	{
		if(p[0] == '0' && toupper(p[1]) == 'X')
		{
			char c = toupper(p[2]);
			if((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'))
				p += 2;
		}
	}
	
	// 4. Przeliczanie znaku na cyfre i sprawdzanie overflowa
	long int value = 0;
	int isValid = 0;	// 0 oznacza że nie przeczytaliśmy poprawnej liczby. Aby przeczytać poprawną liczbę musimy przeczytać choć jedną poprawną cyfrę.
	while (*p != '\0')
	{
		char c = toupper(*p);
		
		if (c >= '0' && c <= '9')
			c = c - '0';
		else if (c >= 'A' && c <= 'Z')
			c = c - 'A' + 10;
		else
			c = -1;	// Każdy niepoprawny znak, który nie może być cyfrą w żadnym dozwolonym systemie da nam wartość -1.
		
		// Sprawdzamy czy c jest poprawną liczbą w wybranym systemie liczbowym.
		// Przerywamy bo cyfra jest niepoprawna.
		if(c < 0 || c >= base)
			break;
		
		isValid = 1;	// Udało się coś przeczytać, więc liczba jest poprawna.
		
		long int new_value = value * base + sign * c;
		
		if (sign == 1)
		{
			if(new_value < value) // Overflow
			{
				errno = ERANGE;
				value = LONG_MAX;
			}	
			else
				value = new_value;
		}
		else
		{
			if(new_value > value) // Overflow
			{
				errno = ERANGE;
				value = LONG_MIN;
			}
			else
				value = new_value;
		}
		
		p++;
	}

	if(isValid == 1)
	{
		if(endptr != NULL)
			*endptr = (char*)p;
	}
	else
	{
		if(endptr != NULL)
			*endptr = (char*)nptr;
	}

	return value;
}
