#include "LibTema.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	SiteList* list = build_db();              //citeste siteurile
	for (int i = 0; i < list->size; ++i)     //trece prin toate siteurile
	{
		Site* s = list->sites[i];
		printf("%s %d %s\n", s->url, s->nrAccesseses, s->title);
	}
	delete_site_list(list);

	return EXIT_SUCCESS;
}
