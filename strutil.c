#include "strutil.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


size_t* largo_de_arreglo_de_char(char** arr){
	int n = 0;
	for (int i = 0; arr[i] != NULL; ++i){
		n++;
	}
	size_t* num = malloc(sizeof(size_t) * (n+2));
	if(num == NULL) return NULL;
	size_t largo_total = 0;
	size_t cont = 0;
	for (int i = 0; arr[i] != NULL; ++i){
		size_t arr_len = strlen(arr[i]);
		largo_total += arr_len + 1;
		num[i+2] = arr_len;
		cont++;
	}
	num[0] = cont;
	num[1] = largo_total;
	return num;
}

char *substr(const char *str, size_t n){
	char* sub = malloc((n+1) * sizeof(char));
	if (sub == NULL) return NULL;
	strncpy(sub, str, n);
	sub[n] = '\0';
	return sub;
}

char *join(char **strv, char sep){
	if (strv == NULL){
		char* cad = malloc(sizeof(char));
		if (cad == NULL){
			return NULL;
		}
		cad = "";
		return cad;
	}
	size_t* larg_char = largo_de_arreglo_de_char(strv);
	int cant_sep = (int)larg_char[0];
	int largo_total = (int)larg_char[1];
	char sep2[1];
	sep2[0] = sep;
	char* str_join = malloc((largo_total + 1) * sizeof(char));
	if (str_join == NULL) return NULL;
	if (cant_sep == 0){
		free(str_join);
		free(larg_char);
		return calloc(1, 1);
	}
	int suma_acum = 0;
	int i = 0;
	larg_char[1] = 0;
	int sep_especial = 0;
	if(sep == '\0') sep_especial = 1;
	while (i < cant_sep-1){
		strncpy(&str_join[suma_acum], strv[i], larg_char[i+2]);
		suma_acum += (int)larg_char[i+2] + 1 - sep_especial;
		strncpy(&str_join[suma_acum-1 + sep_especial], sep2, 1);
		i++;
	}
	strncpy(&str_join[suma_acum], strv[i], larg_char[i+2]);
	suma_acum += (int)larg_char[i+2] + 1 - sep_especial;
	str_join[suma_acum-1 + sep_especial] = '\0';
	free(larg_char);
	return str_join;
}


char** split(const char* str, char sep){
	int largo_split = 0;
	int tam_str = 0;
	for (int i = 0; str[i] != '\0'; ++i){
		if (str[i] == sep) largo_split++;
		tam_str++;
	}
	char** strsplit = malloc(sizeof(char*) * (largo_split+2));
	if (strsplit == NULL) return NULL;
	int minimo = 0;
	size_t guardados = 0;
	for(int i = 0; i < tam_str; i++){
		if(str[i] == sep){
			char* sub =  substr(&str[minimo], (size_t)(i-minimo));
			if (sub == NULL){
				free(strsplit);
				return NULL;
			}
			strsplit[guardados] = sub;
			guardados++;
			minimo=i+1;
		}
	}
	char* sub =  substr(&str[minimo], (size_t)(tam_str-minimo));
	if (sub == NULL){
		free(strsplit);
		return NULL;
	}
	strsplit[guardados] = sub;
	strsplit[guardados+1] = NULL;
	return strsplit;	
}

void free_strv(char *strv[]){
	for (int i = 0; strv[i] != NULL; ++i){
			free(strv[i]);
	}
	free(strv);
}

