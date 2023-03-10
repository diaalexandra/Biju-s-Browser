#include "LibTema.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int rotr(char x, int k); // roteste la dreapta bitii lui x cu k pozitii
int rotl(char x, int k); // roteste la stanga bitii lui x cu k pozitii
int compute_checksum(const char* str);  //calculeaza checksumu-ul unui string

int main()
{
	SiteList* list = build_db();   //citeste site-urile

	char url[255];
	memset(url, 0, 255 * sizeof(char));
#if defined(_MSC_VER)
	while (gets_s(url, 255) != NULL && strlen(url) > 0)  //cat timp citesc un url
#else
	while (gets(url) != NULL && strlen(url) > 0)
#endif
	{
		Site* s = NULL;
		for (int i = 0; i < list->size; ++i)   // cauta siteul care are adresa = cu cea din variabila url
		{
			if (strcmp(url, list->sites[i]->url) == 0)
			{
				s = list->sites[i];
				break;
			}
		}

		if (s == NULL)
		{
			printf("Website not found!\n");      //daca nu s-a gasit printeaza mesajul corespunzator
		}
		else
		{
			int key = compute_checksum(s->html);     //calculeaza checksum
			if (key == s->checksum)
			{
				printf("Website safe!\n");        // daca checksumul citit = cu cel calculcat atunci printeaza mesajul corespunzator
			}
			else
			{
				printf("Malicious website! Official key: %d. Found key: %d\n", s->checksum, key);   //altfel printeaza mesajul corespunzator
			}
		}
	}

	delete_site_list(list);
	return EXIT_SUCCESS;
}

int rotr(char x, int k)
{
	k %= 8;
	int tmp = x;
	int res = (tmp >> k) | (tmp << (8 - k));
	return 255 & res;
}

int rotl(char x, int k)
{
	k %= 8;
	int tmp = x;
	int res = (tmp << k) | (tmp >> (8 - k));
	return 255 & res;
}

int compute_checksum(const char* str)
{
	int len = strlen(str);
	int acc = str[0];    //primul caracter este neschimbat

	for (int i = 1; i < len; ++i)  //trece prin caracterele stringului
	{
		if (i % 2 == 0)
		{
			acc = acc ^ rotl(str[i], i);  //roteste la stanga pentru pozitiile pare
		}
		else
		{
			acc = acc ^ rotr(str[i], i);  // roteste la dreapta pentru pozitiile impare si facem xor cu rezultatul partial
		}
	}

	return acc;
}
