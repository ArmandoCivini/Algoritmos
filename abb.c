#include <stdlib.h>
#include <string.h>
#include "abb.h"
#include "pila.h"
#include <stdio.h>
#define N 100

typedef struct nodo{
	char* clave;
	void* dato;
	struct nodo* izq;
	struct nodo* der;
}nodo_t;

struct abb {
    nodo_t* raiz;
    size_t cant;
    abb_comparar_clave_t comparador;
    abb_destruir_dato_t d_dato;
};


nodo_t* nodo_crear(const char* clave, void* dato){
	nodo_t* nodo = malloc(sizeof(nodo_t));
	char* clave_aux = malloc(sizeof(char*)*(strlen(clave)+1));
	if (nodo == NULL) return NULL;
	clave_aux = strcpy(clave_aux, clave);
	nodo->clave = clave_aux;
	nodo->dato = dato;
	nodo->izq = NULL;
	nodo->der = NULL;
	return nodo;
}

abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato){
	abb_t* abb = malloc(sizeof(abb_t));
	abb->raiz = NULL;
	abb->cant = 0;
	abb->comparador = cmp;
	abb->d_dato = destruir_dato;
	return abb;

}

bool abb_guardar(abb_t* arbol, const char* clave, void* dato){
	nodo_t* abb = arbol->raiz;
	int caso = 0;
	nodo_t* padre = NULL;
	while (abb != NULL){
		if(arbol->comparador(abb->clave, clave) == 0){
			void* viejo = abb->dato;
			abb->dato = dato;
			if(arbol->d_dato){
				(arbol->d_dato)(viejo);
			}
			return true;
		}
		if(arbol->comparador(abb->clave, clave) > 0){
			padre = abb;
			abb = abb->izq;
			caso = 1;
		}else{
			padre = abb;
			abb = abb->der;
			caso = -1;
		}
	}
	abb = nodo_crear(clave, dato);
	if (caso == 0){
		arbol->raiz = abb;
	}
	if(caso == 1){
		padre->izq = abb;
	}
	if (caso == -1){
		padre->der = abb;
	}
	if (abb == NULL) return false;
	arbol->cant++;
	return true;
}

void* abb_obtener(const abb_t* arbol, const char* clave){
	nodo_t* abb = arbol->raiz;
	if(!abb){return NULL;}
	while (abb && arbol->comparador(abb->clave, clave) != 0){
		if(arbol->comparador(abb->clave, clave) > 0){
			abb = abb->izq;
		}else{
			abb = abb->der;
		}
	}
	if (abb == NULL) return NULL;
	return abb->dato;
}

bool abb_pertenece(const abb_t* arbol, const char* clave){
	nodo_t* abb = arbol->raiz;
	while (abb != NULL){
		if (arbol->comparador(abb->clave, clave) == 0) break;
		if(arbol->comparador(abb->clave, clave) > 0){
			abb = abb->izq;
		}else{
			abb = abb->der;
		}
	}
	if (abb == NULL) return false;
	return true;
}

void* abb_borrar(abb_t* arbol, const char* clave){
	nodo_t* abb = arbol->raiz;
	nodo_t* abb_padre = NULL;
	int ult_giro = 0;
	while (abb != NULL){
		if (arbol->comparador(abb->clave, clave) == 0){
			break;
		}
		abb_padre = abb;
		if (arbol->comparador(abb->clave, clave) > 0){
			abb = abb->izq;
			//el que hay que borrar es una hijo izq
			ult_giro = 1;
		}else{
			abb = abb->der;
			//el que hay que borrar es un hijo derecho
			ult_giro = -1;
		}

	}
	if (abb == NULL){
		return NULL;}
	nodo_t* abb_remplazo = abb->der;
	if (abb_remplazo == NULL){
		if (ult_giro == 1){
			abb_padre->izq = abb->izq;
		}else if(ult_giro == -1){
			abb_padre->der = abb->izq;
		}else{
			arbol->raiz = abb->izq;	
		}
		void* dato = abb->dato;
		free(abb->clave);
		free(abb);
		arbol->cant--;
		return dato;
	}
	nodo_t* abb_remplazo_padre = abb;
	while(abb_remplazo->izq != NULL){
		abb_remplazo_padre = abb_remplazo;
		abb_remplazo = abb_remplazo->izq;
	}
	abb_remplazo->izq = abb->izq;
	if (ult_giro == 1){
		abb_padre->izq = abb_remplazo;
	}
	if (ult_giro == -1){
		abb_padre->der = abb_remplazo;
	}
	if(ult_giro == 0) {
		arbol->raiz = abb_remplazo;
	}
	abb_remplazo_padre->izq = abb_remplazo->der;
	if (abb_remplazo_padre != abb) abb_remplazo->der = abb->der;
	void* dato = abb->dato;
	free(abb->clave);
	free(abb);
	arbol->cant--;
	return dato;
}

size_t abb_cantidad(abb_t* arbol){
	return arbol->cant;
}

void nodo_destruir(nodo_t* nodo, abb_destruir_dato_t destruir_dato){
	if (!nodo){
		return;
	}
	nodo_destruir(nodo->izq, destruir_dato);
	nodo_destruir(nodo->der, destruir_dato);
	if(destruir_dato != NULL){
		destruir_dato(nodo->dato);
	}
	free(nodo->clave);
	free(nodo);
}

void abb_destruir(abb_t* arbol){
	nodo_destruir(arbol->raiz, arbol->d_dato);
	free(arbol);
}

void abb_destruir(abb_t* arbol){
	nodo_destruir(arbol->raiz, arbol->d_dato);
	free(arbol);
}

void** abb_busc_iter_corte(abb_t* arbol, const char* clave_busc, const char* clave_corte){
	nodo_t* abb = arbol->raiz;
	if (!abb) return NULL;
	pila_t* pila_izq = pila_crear();
	while (abb && arbol->comparador(abb->clave, clave_busc) != 0){
		if(arbol->comparador(abb->clave, clave_busc) > 0){
			pila_apilar(pila_izq, abb);
			abb = abb->izq;
		}else{
			abb = abb->der;
		}
	}
	if (abb == NULL) return NULL;
	abb_iter_t* iter = malloc(sizeof(abb_iter_t));
	if (iter == NULL) return NULL;
	iter->act = abb;
	iter->pila_padres = pila_izq;
	void** nodos_recorridos = malloc(sizeof(void*) * N);
	int i = 0;
	int tam_arr = N;
	while (!abb_iter_in_al_final(iter) && arbol->comparador(abb_iter_in_ver_actual(iter), clave_corte) < 0){
		nodos_recorridos[i] = iter->act->dato;
		i++;
		abb_iter_in_avanzar(iter);
		if ( i == tam_arr){
			nodos_recorridos = realloc(nodos_recorridos, tam_arr * 2 * sizeof(void*));
			tam_arr = tam_arr * 2;
		}
	}
	nodos_recorridos[i] = iter->act->dato;
	i++;
	if ( i == tam_arr){
			nodos_recorridos = realloc(nodos_recorridos, (tam_arr + 2) * sizeof(void*));
			tam_arr = tam_arr +2;
		}
	nodos_recorridos[i] = NULL;
	free(iter);
	pila_destruir(pila_izq);
	return nodos_recorridos;
}

struct abb_iter {   
    nodo_t* act;
    pila_t* pila_padres;
};

nodo_t* mas_zurdo_iter(nodo_t* act, pila_t* pila_padres){
	while(act->izq != NULL){
		pila_apilar(pila_padres, act);
		act = act->izq;
	}
	return act;
}

abb_iter_t *abb_iter_in_crear(const abb_t *arbol){
	abb_iter_t* iter = malloc(sizeof(abb_iter_t));
	if (iter == NULL) return NULL;
	iter->act = arbol->raiz;
	iter->pila_padres = pila_crear();
	if(iter->act){
	iter->act = mas_zurdo_iter(iter->act, iter->pila_padres);}
	return iter;
}

bool abb_iter_in_avanzar(abb_iter_t *iter){
	if (iter->act == NULL) return false;
	if (iter->act->der){
		iter->act = iter->act->der;
		iter->act = mas_zurdo_iter(iter->act, iter->pila_padres);
		return true;
	}
	if (pila_esta_vacia(iter->pila_padres)){
		iter->act = NULL;
		return true;
	}
	iter->act = pila_desapilar(iter->pila_padres);
	return true;
}

bool abb_iter_in_al_final(const abb_iter_t *iter){
	return (iter->act == NULL);
}

const char *abb_iter_in_ver_actual(const abb_iter_t *iter){
	if (abb_iter_in_al_final(iter)) return NULL;
	return iter->act->clave;
}

void abb_iter_in_destruir(abb_iter_t* iter){
	pila_destruir(iter->pila_padres);
	free(iter);
}

bool iterador_interno(nodo_t* abb, bool visitar(const char*, void*, void*), void* extra){
	if(!abb){
		return true;
	}
	bool izquierda = iterador_interno(abb->izq, visitar, extra);
	if(izquierda == false){
		return false;
	}
	if(visitar(abb->clave, abb->dato, extra) == false){
		return false;
	}
	bool derecha = iterador_interno(abb->der, visitar, extra);
	if(derecha == false){
		return false;
	}
	return true;
}

void abb_in_order(abb_t* abb, bool visitar(const char*, void*, void*), void* extra){
	iterador_interno(abb->raiz, visitar, extra);
}
