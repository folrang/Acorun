#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "md5.h"

#include "../Manager.h"

#ifndef FALSE
#define FALSE 0
#define TRUE (!(FALSE))
#endif

int
Bin2HexA(char *hex, size_t hex_size, const void *abins, size_t bins_size, int use_upper)
{
	size_t i, j;
	const char *UHEXMAP = "0123456789ABCDEF";
	const char *LHEXMAP = "0123456789abcdef";
	const unsigned char *bins = (const unsigned char *) abins;

	const char *HEXMAP = LHEXMAP;

	if(use_upper)
		HEXMAP = UHEXMAP;

	for(i=0, j=0; i<bins_size; ++i)
	{
		if(j+1 >= hex_size)
			return FALSE;

		hex[j] = HEXMAP[bins[i] >> 4];
		hex[j+1] = HEXMAP[bins[i] & 0xf];
		j += 2;
	}

	hex[j] = '\0';
	return TRUE;
}

int
IsValidValue(const char *s, const char *m, const char *v)
{
	char buffer[256];
	_snprintf(buffer, sizeof(buffer), "%s%s", s, m);

	md5_state_t st;
	md5_init(&st);

	md5_append(&st, (md5_byte_t *) buffer, (int)strlen(buffer));

	md5_byte_t md5[16];
	md5_finish(&st, md5);

	char hex[256];
	if(!Bin2HexA(hex, sizeof(hex), md5, sizeof(md5), FALSE))
		return FALSE;

	if(strcmp(v, hex))
		return FALSE;

	return TRUE;
}

char *
GetToken(char *s, const char *delim)
{
	char *ptr = strtok(s, delim); 
	if(!ptr)
		return NULL;

	return strdup(ptr);
}

int 
IsValidXigncodeCookieFast(const char *cookie)
{
	char tmp[256];
	memset(tmp, 0, sizeof(tmp));
	strncpy(tmp, cookie, sizeof(tmp) - 1);

	char *tok[3];
	int result = FALSE;

	tok[0] = GetToken(tmp, "_");
	tok[1] = GetToken(NULL, "_");
	tok[2] = GetToken(NULL, "_");

	if(tok[0] && tok[1] && tok[2]
		&& IsValidValue(tok[0], GameServer::Security::MANAGER()->GetServerCookieSeed(), tok[2]) )
	{
		for( int i=0; i<GameServer::Security::MANAGER()->GetApkSignatureCnt(); i++ )
		{
			const char * szApkSignature = GameServer::Security::MANAGER()->GetApkSignature(i);
			if( szApkSignature != NULL && IsValidValue(tok[0], szApkSignature, tok[1]) )
			{
				result = TRUE;
				break;
			}
		}
	}

	if(tok[0])
		free(tok[0]);

	if(tok[1])
		free(tok[1]);

	if(tok[2])
		free(tok[2]);

	return result;
}
