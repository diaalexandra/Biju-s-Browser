#pragma once

typedef struct   // structura pentru citirea tuturor elementelor unui site
{
	char url[50];
	int nrAccesseses;
	int checksum;
	char* title;
	char* content;
	char* html;
} Site;

typedef struct  // structura care contine vectorul de site-uri
{
	Site** sites;
	int size;
	int maxSize;
} SiteList;

Site* create_site(const char* siteName);
Site* dup_site(Site* s);
void delete_site(Site* site);

SiteList* crete_site_list();
void delete_site_list(SiteList* siteList);
void add_site(SiteList* siteList, const char* siteName);
void add_site_ex(SiteList* siteList, Site* s);

SiteList* build_db();
char* duplicate_string(const char* str);
