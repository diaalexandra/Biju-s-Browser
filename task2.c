#include "LibTema.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void getQueryWords(char** words, int* size);
int site_compare(const void* s1, const void* s2);
char* strip_html_tags(char* str);
char** get_words_from_string(char* str, int* size);
int tmpf(char** queryWords, char** siteWords, int queryWordsLen, int siteWordsLen);

int main()
{
	char* words[100];
	int size = 0;
	memset(words, 0, 100 * sizeof(char*));
	getQueryWords(words, &size);              //citeste cuvintele date
	SiteList* list = build_db();             //citeste site-urile
	SiteList* list2 = crete_site_list();    //creeaza lista de siteuri care satisfac cerinta (contin cuv din cele date)

	for (int i = 0; i < list->size; ++i)   //parcurge toate siteurile
	{
		Site* s = list->sites[i];

		char* str1 = strip_html_tags(s->content);
		int wrodsSize = 0;
		char** wordsInString = get_words_from_string(str1, &wrodsSize);   // calculeaza cuvintele siteului

		if(tmpf(words , wordsInString, size, wrodsSize))         //daca se gaseste unul cel putin un cuv din query
		{
			add_site_ex(list2, dup_site(s));                  // adauga siteul in lista
		}

		free(str1);                                       //eliberam memoria alocata
		for (int i = 0; i < wrodsSize; ++i)
		{
			free(wordsInString[i]);
		}
		free(wordsInString);
	}

	qsort(list2->sites, list2->size, sizeof(Site*), site_compare);   //sortam siteurile folosind comparatorul site_compare

	for (int i = 0; i < list2->size; ++i)    // printam url-ul siteurilor gasite
	{
		Site* s = list2->sites[i];
		printf("%s\n", s->url);
	}

	for (int i = 0; i < size; ++i) { free(words[i]); }
	delete_site_list(list);
	delete_site_list(list2);
	return EXIT_SUCCESS;          //return 0
}

void getQueryWords(char** words, int* size)  //functie care ne citeste cuvintele de cautare
{
	char line[255];
	fgets(line, 255, stdin);          //citeste linia de la tastatura
	line[strcspn(line, "\n")] = 0;   //elimina newline

	char delim[] = " ";
	char* tok = strtok(line, delim);   //returneaza primul cuv din string

	while (tok != NULL)
	{
		words[*size] = duplicate_string(tok);   //il salveaza in vectorul de cuvinte
		(*size)++;

		tok = strtok(NULL, delim);    //trece la urmatorul cuvant
	}
}

int site_compare(const void* s1, const void* s2)   //functie care compara 2 siteuri
{
	Site* site1 = *(Site**)s1;
	Site* site2 = *(Site**)s2;

	int res = strcmp(site1->content, site2->content);

	if (res == 0)                       //daca continutul e identic ordinea este data de nr de accesari
	{
		return site2->nrAccesseses - site1->nrAccesseses;    //siteul cu cele mai multe accesari apare primul in lista
	}
	return res;                       // altfel este ordine lexicografica a contentului
}

char* strip_html_tags(char* str)    // functie care face strip la html site
{
	int len = strlen(str);
	char* res = (char*)malloc(len * sizeof(char));    // aloca stringul pentru rezultat
	memset(res, 0, len * sizeof(char));

	char* chI = str;      //pornim de la primul caracter din input
	char* chO = res;     // pozitia in stringul final

	while (chI < str + len && *chI)    // cat timp nu am aj la ultimul caracter
	{
		while (chI < str + len && *chI && *chI != '<')     // copiem toate caract di input pana dam de <
		{
			*chO = *chI;
			chO++;
			chI++;
		}

		while (chI < str + len && *chI && *chI != '>')    //sarim peste toate caracterele pana dam de >
		{
			chI++;
		}
		chI++;           //dam skip si peste caract >
	}

	return res;
}

char** get_words_from_string(char* str, int* size)    // functie care returneaza cuvintele dintr-un string
{
	int len = strlen(str);
	*size = len /2;                                          //lungimea vect de cuv (pp ca nu are mai mult de 2 caract in general)
	char** words = (char**)malloc(*size * sizeof(char*));   //aloc spatiu pentru vectorul de cuvinte
	memset(words, 0, *size * sizeof(char*));

	char delim[] = "\n ";
	char* tok = strtok(str, delim);    //extragem primul cuvant
	*size = 0;

	while (tok != NULL)
	{
		words[*size] = duplicate_string(tok);   //se copiaza cuvantul in vectorul de cuvinte
		(*size)++;

		tok = strtok(NULL, delim);       // luam urmatorul cuvant
	}

	return words;
}

int tmpf(char** queryWords, char** siteWords, int queryWordsLen, int siteWordsLen) // functie care ne intoarce true daca s-a gasit cel putin
                                                                                  //un cuv din cele date in cuv unui site
{
	for (int i = 0; i < queryWordsLen; ++i)       //pentru fiecare cuvand dat
	{
		for (int j = 0; j < siteWordsLen; ++j)   //verific daca este prezent in cuvintele siteului
		{
			if (strcmp(queryWords[i], siteWords[j]) == 0)
			{
				return 1;
			}
		}
	}

	return 0;        //altfel false
}

