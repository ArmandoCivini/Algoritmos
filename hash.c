#include "hash.h"
#define CAPACIDAD_INICIAL 13
#define FACTOR_DE_RED 7
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

/******************************
	Estructuras del codigo		
******************************/

typedef struct campo{
	char* clave_h;
	void* valor;
}campo_t;


typedef struct nodo{
	campo_t* campo;
	struct nodo* prox;
} nodo_t;


typedef struct lista{
	nodo_t* primero;
	nodo_t* ultimo;
	size_t cantidad;
}lista_t;



struct hash{
	size_t cantidad;
	int capacidad;
	lista_t** tabla;
	hash_destruir_dato_t f_destruir;
};


typedef struct lista_iter{
	lista_t* lista;
	nodo_t* posicion;
	nodo_t* anterior;
}lista_iter_t;

struct hash_iter{
	const hash_t* hash;
	int posicion_tabla;
	nodo_t* nodo;
};

/*********************************
			FUNCIONES
*********************************/

int hashf(const char *word) {
    int hash = 0;
    int n;
    for (int i = 0; word[i] != '\0'; i++) {
        // alphabet case
        if (isalpha(word[i]))
            n = word[i] - 'a' + 1;
        else  // comma case
            n = 27;

        hash = ((hash << 3) + n);
    }
    return hash;
}


nodo_t* nodo_crear(void* dato){
	nodo_t* nodo = malloc(sizeof(nodo_t));
	if(nodo==NULL){
		return NULL;
	} 
	nodo->campo = dato;
	nodo->prox = NULL;
	return nodo;
}

hash_t* hash_crear(hash_destruir_dato_t destruir_dato){
	hash_t* hash = malloc(sizeof(hash_t));
	hash->tabla = malloc(sizeof(lista_t*) * CAPACIDAD_INICIAL);
	hash->capacidad = CAPACIDAD_INICIAL;
	for (int i = 0; i < CAPACIDAD_INICIAL; ++i)
	{
		hash->tabla[i] = NULL;
	}
	hash->cantidad = 0;
	hash->f_destruir = destruir_dato;
	return hash;
}

lista_t* lista_crear(){
	lista_t* lista = malloc(sizeof(lista_t));
	if(lista==NULL){
		return NULL;
	}
	lista->primero=NULL;
	lista->ultimo=NULL;
	lista->cantidad=0;
	return lista;
}

bool lista_esta_vacia(const lista_t* lista){
	return lista->cantidad==0;
}
void lista_destruir(lista_t *lista, void destruir_dato(void *)){
	nodo_t* prox;
	while (lista->primero != NULL){
		if(destruir_dato != NULL) destruir_dato(lista->primero->campo->valor);
		prox = lista->primero->prox;
		free(lista->primero->campo->clave_h);
		free(lista->primero->campo);
		free(lista->primero);
		lista->primero= prox;
	}
	free(lista);
}

campo_t* campo_crear(void* dato, char* clave){
	campo_t* campo = malloc(sizeof(campo_t));
	if (campo==NULL){
		return NULL;
	}
	campo->valor = dato;
	campo->clave_h = clave;
	return campo;
}	

bool lista_insertar_ultimo(lista_t* lista, void* dato, char* clave){
	campo_t* campo = campo_crear(dato, clave);
	if (campo==NULL){
		return NULL;
	}
	nodo_t* nodo = nodo_crear(campo);
	if (nodo==NULL)
	{
		return false;
	}
	if(lista_esta_vacia(lista)){
		lista->primero = nodo;
	}else{lista->ultimo->prox = nodo;}
	lista->ultimo = nodo;
	lista->cantidad++;
	return true;
}

void** buscar_nodo_borrar(const hash_t* hash, int posicion, const char* clave){
	void** verctor_nodos = malloc(2 * sizeof(nodo_t*));
	nodo_t* nodo_ant = NULL;
	if(hash->tabla[posicion] == NULL){
		return false;
	}
	nodo_t* nodo = hash->tabla[posicion]->primero;
	while(nodo != NULL && strcmp(nodo->campo->clave_h, clave)!=0){
		nodo_ant = nodo;
		nodo = nodo->prox; 
	}
	verctor_nodos[0] = nodo_ant;
	verctor_nodos[1] = nodo;
	return verctor_nodos;
}

nodo_t* buscar_nodo(const hash_t* hash, int posicion, const char* clave){
	if(hash->tabla[posicion] == NULL){
		return false;
	}
	nodo_t* nodo = hash->tabla[posicion]->primero;
	while(nodo != NULL && strcmp(nodo->campo->clave_h, clave)!=0){
		nodo = nodo->prox; 
	}
	return nodo;
}


bool hash_pertenece(const hash_t *hash, const char *clave){
	int posicion = hashf(clave);
	posicion = abs(posicion % hash->capacidad);
	if(strlen(clave)== 0){
		posicion = 0;
	}
	nodo_t* nodo = buscar_nodo(hash, posicion, clave);
	if(nodo==NULL){
		return false;
	}
	return true;
}

bool hash_redimensionar(hash_t* hash){
    lista_t** nueva_tabla = malloc(sizeof(lista_t*) * (size_t)(hash->capacidad * FACTOR_DE_RED));
    if (nueva_tabla == NULL) return false;
    for (int i = 0; i < hash->capacidad * FACTOR_DE_RED; ++i)
    {
        nueva_tabla[i] = NULL;
    }
    int cap_ant = hash->capacidad;
    lista_t** tabla_ant = hash->tabla;
    hash->tabla = nueva_tabla;
    hash->capacidad = (hash->capacidad * FACTOR_DE_RED);
    bool no_falla = true;
    for (int i = 0; i < cap_ant; ++i){
        lista_t* lista_act = tabla_ant[i];
        if(lista_act==NULL){
            continue;
        }
        nodo_t* nodo_act = lista_act->primero;
        while (nodo_act != NULL){
            hash->cantidad--;
            no_falla = hash_guardar(hash, nodo_act->campo->clave_h, nodo_act->campo->valor);
            free(nodo_act->campo->clave_h);
            nodo_t* nodo_v = nodo_act;
            nodo_act = nodo_act->prox;
            free(nodo_v->campo);
            free(nodo_v);
        }   
    }
    if (!no_falla){
        hash->tabla = tabla_ant;
        hash->capacidad = cap_ant;
        return false;
    }
    for(int i=0; i< cap_ant; i++){
        if(tabla_ant[i] != NULL) free(tabla_ant[i]);
    }
    free(tabla_ant);
    return true;
}       

bool hash_guardar(hash_t* hash, const char* clave, void* dato){
	if (hash->cantidad > (hash->capacidad)* 0.7){
		if(!hash_redimensionar(hash)) return false;
	}
	int posicion = hashf(clave);
	posicion =  abs(posicion % hash->capacidad);
	if(strlen(clave)== 0){
		posicion = 0;
	}
	if(hash->tabla[posicion] == NULL){
		lista_t* lista = lista_crear();
		if(!lista){
			return false;
		}
		hash->tabla[posicion] = lista;
	}
	nodo_t* nodo = buscar_nodo(hash, posicion, clave);
	if(nodo){
		void* a_borrar = nodo->campo->valor;
		nodo->campo->valor = dato;
		hash_destruir_dato_t liberar =hash->f_destruir;
		if(hash->f_destruir != NULL){
			liberar(a_borrar);
		}
		
		return true;
	}
	char* clave2 = malloc(sizeof(char) * (strlen(clave)+ 1));
	strcpy(clave2, clave);
	bool resultado = lista_insertar_ultimo(hash->tabla[posicion], dato, clave2);
	if(resultado==false){
		return resultado;
	}
	hash->cantidad++;
	return true;
}


void *hash_obtener(const hash_t *hash, const char *clave){
	int posicion = hashf(clave);
	posicion = abs(posicion % hash->capacidad);
	if(strlen(clave)== 0){
		posicion = 0;
	}
	if(hash->tabla[posicion] == NULL){
		return NULL;
	}
	nodo_t* nodo = buscar_nodo(hash, posicion, clave);
	if(nodo==NULL){
		return NULL;
	}
	return nodo->campo->valor;
}

void* hash_borrar(hash_t* hash, const char* clave){
	int posicion = hashf(clave);
	posicion = abs(posicion % hash->capacidad);
	if(strlen(clave)== 0){
		posicion = 0;
	}
	if(hash->tabla[posicion] == NULL){
		return NULL;
	}
	void** vector_nodo = buscar_nodo_borrar(hash, posicion, clave);
	nodo_t* nodo_ant = (nodo_t*)vector_nodo[0]
	nodo_t* nodo = (nodo_t*)vector_nodo[1]
	if(nodo==NULL){
		return NULL;
	}
	if(nodo->prox == NULL){
		hash->tabla[posicion]->ultimo = nodo_ant;
	}
	if(nodo_ant == NULL){
		hash->tabla[posicion]->primero = nodo->prox;
	}else{
		nodo_ant->prox = nodo->prox;
		
	}
	void* dato = nodo->campo->valor;
	free(nodo->campo->clave_h);
	free(nodo->campo);
	free(nodo);
	free(vector_nodo);
	hash->cantidad--;
	hash->tabla[posicion]->cantidad--;
	if(hash->tabla[posicion]->cantidad == 0){
		lista_t* a_borrar = hash->tabla[posicion];
		hash->tabla[posicion] = NULL;
		free(a_borrar);
	}
	return dato;
}	
	
size_t hash_cantidad(const hash_t *hash){
	return hash->cantidad;
}

void hash_destruir(hash_t *hash){
	for(int i=0; i< hash->capacidad; i++){
		if(hash->tabla[i] != NULL) lista_destruir(hash->tabla[i], hash->f_destruir);
	}

	free(hash->tabla);
	free(hash);
}
	
	
	
/*********************************
	ITERADOR DEL HASH
*********************************/
hash_iter_t *hash_iter_crear(const hash_t *hash){
	hash_iter_t* hash_iter = malloc(sizeof(hash_iter_t));
	hash_iter->hash = hash;
	hash_iter->posicion_tabla = 0;
	while(hash_iter->hash->tabla[hash_iter->posicion_tabla]==NULL){
		hash_iter->posicion_tabla++;
		if(hash_iter->posicion_tabla == hash_iter->hash->capacidad-1){
			hash_iter->nodo = NULL;
			return hash_iter;
		}
	}
	hash_iter->nodo = hash_iter->hash->tabla[hash_iter->posicion_tabla]->primero;
	return hash_iter;
}
bool hash_iter_avanzar(hash_iter_t *iter){
	while(iter->hash->tabla[iter->posicion_tabla]==NULL || iter->nodo == NULL){
		if(iter->posicion_tabla == iter->hash->capacidad-1 && iter->nodo == NULL){
			return false;
		}
		iter->posicion_tabla++;
		if(iter->hash->tabla[iter->posicion_tabla]!=NULL){
			iter->nodo = iter->hash->tabla[iter->posicion_tabla]->primero;
			return true;
		}	
	}
	iter->nodo = iter->nodo->prox;
	if(iter->nodo == NULL){
		return hash_iter_avanzar(iter);
	}
	return true;	
}

bool hash_iter_al_final(const hash_iter_t *iter){
	return ((iter->nodo == NULL && iter->posicion_tabla == iter->hash->capacidad - 1)||iter->hash->cantidad==0);
}

const char *hash_iter_ver_actual(const hash_iter_t *iter){
	if(iter->hash->cantidad==0 || hash_iter_al_final(iter)||iter->nodo == NULL){
		return NULL;
	}
	return iter->nodo->campo->clave_h;
}	
	


void hash_iter_destruir(hash_iter_t* iter){
	free(iter);
}
