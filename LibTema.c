#include "LibTema.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* getContent(const char* str);
char* getTitle(const char* str);

Site* create_site(const char* siteName)  //creez o functie care imi citeste site-ul din fisier
{
	FILE* f = fopen(siteName, "r");

	if (f == NULL)      // daca nu am reusit sa-l deschid return null
	{
		return NULL;
	}

	Site* tmp = (Site*)malloc(sizeof(Site));  // aloc spatiu pentru structura site

	int htmlLength;
	fscanf(f, "%s %d %d %d", tmp->url, &htmlLength, &tmp->nrAccesseses, &tmp->checksum); //citesc din fisier elem de pe prima linie de care am nevoie

	char* html = (char*)malloc((htmlLength + 1) * sizeof(char));  // aloc spatiu pentru content
	fgetc(f);                                                    // elimin noua linie din prima linie a fisierului
	fread(html, sizeof(char), htmlLength, f);                   // citesc contentul
	html[htmlLength] = 0;                                      // ultimul element il setez cu 0

	tmp->title = getTitle(html);
	tmp->content = getContent(html);
    tmp->html = html;
	fclose(f);

	return tmp;
}

Site* dup_site(Site* s)  // functie care duplica un site
{
	Site* tmp = (Site*)malloc(sizeof(Site));
	strcpy(tmp->url, s->url);                         //creez o copie
	tmp->nrAccesseses = s->nrAccesseses;
	tmp->checksum = s->checksum;
	tmp->title = strdup(s->title);
	tmp->content = strdup(s->content);
    tmp->html = duplicate_string(s->html);

	return tmp;
}

void delete_site(Site* site)  //functie care sterge site-ul
{
	free(site->title);
	free(site->content);
    free(site->html);
	free(site);
}

char* getTitle(const char* str)  // creez o functie care scoate titlul dintre tagurile html
{
	const char* poz1 = strstr(str, "<title>");  //retin pozitia unde se gaseste "<title>"
	const char* poz2 = strstr(str, "</title>");  // retin pozitia unde se gaseste "</title>"
	if (poz1 && poz2)
	{
		poz1 += 7;                                               // adaug 7 pentru cele 7 caractere ale lui "<title>"
		int size = poz2 - poz1;                                 // si apoi il elimin prin scaderea din pozitia a 2-a
		char* res = (char*)malloc((size + 1) * sizeof(char));  // aloc spatiu pentru titlu
		strncpy(res, poz1, size);                             // il copiez
		res[size] = 0;                                       // caracterul cu care se termina stringul il setez cu 0
		return res;
	}
	else                                                  //altfel returnam un empty string (string care contine decat caract 0)
	{
		char* res = (char*)malloc(1 * sizeof(char));
		res[0] = 0;
		return res;
	}
}

char* getContent(const char* str)      // creez o functie care imi citeste continutul
{
	const char* poz1 = strstr(str, "</title>");   //continutul incepe dupa tagul de final de titlu asa ca retin pozitia acestuia
	const char* poz2 = strstr(str, "</html>");// si se termina la tagul </html> si retin si pozitia acestuia

	if (poz1 && poz2)        // daca cele doua pozitii exista
	{
		poz1 += 9;                                                // dam skip la title
		int size = poz2 - poz1;                                  // calculam lungimea stringului rezultat
		char* res = (char*)malloc((size + 1) * sizeof(char));   // aloc memorie pentru string
		strncpy(res, poz1, size);                              // il copiez
		res[size] = 0;
		return res;
	}
	else                                                    //altfel returnam un empty string (string care contine decat caract 0)
	{
		char* res = (char*)malloc(1 * sizeof(char));
		res[0] = 0;
		return res;
	}
}

SiteList* crete_site_list()     // creez o functie pentru crearea vectorului de site-uri gol de lungime maxima 3
{
	SiteList* tmp = (SiteList*)malloc(sizeof(SiteList));    //alocam spatiu structurii care contine vectorul
	tmp->maxSize = 3;
	tmp->size = 0;
	tmp->sites = (Site**)malloc(tmp->maxSize * sizeof(Site*));  // se aloca vectorul de stringuri
	memset(tmp->sites, 0, tmp->maxSize * sizeof(Site*));       //initializam cu 0 toate elem
	return tmp;
}

void delete_site_list(SiteList* siteList)  // creez o functie care sterge vectorul de siteuri odata ce l-am terminat de folosit
{
	for (int i = 0; i < siteList->size; ++i)   // sterge toate siteurile alocate
	{
		delete_site(siteList->sites[i]);
	}
    free(siteList->sites);  //sterge vectorul de site pointeri
	free(siteList);        // sterge structura care contine vectorul de siteuri
}

void add_site(SiteList* siteList, const char* siteName) //creez o functie care imi adauga fiecare site in lista dupa numele lui (adauga unul nou)
{
	Site* s = create_site(siteName);
	if (s == NULL)
		return;

	add_site_ex(siteList, s);
}

void add_site_ex(SiteList* siteList, Site* s)  // functie care adauga un site existent in lista (site existent)
{
	if (siteList->size + 1 > siteList->maxSize)     //daca nu avem spatiu sa-l adaugam
	{
		siteList->maxSize += 3;                    // dimensiunea max este incrementata cu 3

		Site** sites = (Site**)malloc(siteList->maxSize * sizeof(Site*));  // se aloca spatiu pentru noul vector de siteuri
		memset(sites, 0, siteList->maxSize * sizeof(Site*));

		for (int i = 0; i < siteList->size; ++i)    // copiaza din vectorul vechi in vectorul nou
		{
			sites[i] = siteList->sites[i];
		}
		free(siteList->sites);      //se sterge vectorul vechi
		siteList->sites = sites;   //structura va contine acum noul vector
	}

	siteList->sites[siteList->size] = s;  //pe ultima pozitie se adauga noul site
	siteList->size++;                    //dim vectorului creste cu 1
}

SiteList* build_db()   // functie care creaza un sitelist cu toate siteurile din master.txt
{
	SiteList* list = crete_site_list();

	FILE* f = fopen("master.txt", "r");  //deschide
	char name[255];
	while(fscanf(f,"%s", name) >=1)    //cat timp se citeste un nou site
	{
		add_site(list, name);        // il adauga in lista
	}
	fclose(f);
	return list;
}

char* duplicate_string(const char* str)  //functie care duplica un string
{
#if defined(_MSC_VER)           // daca folosim visual studio
	return _strdup(str);
#else
	return strdup(str);        // altfel suntem pe gcc
#endif
}

