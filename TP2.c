#define _GNU_SOURCE
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "strutil.h"
#include "abb.h"
#include "hash.h"
#include "pila.h"
#include "cola.h"
#include "heap.h"
#define N 5
#define RED 2

typedef struct vuelos{
	abb_t* arbol;
	hash_t* hash;
}vuelos_t;

typedef struct vuelo{
	char** datos_separados;
}vuelo_t;

vuelo_t* vuelo_crear(char** separados){
	vuelo_t* vuelo = malloc(sizeof(vuelo_t));
	vuelo->datos_separados = separados;
	return vuelo;
	
}

void vuelo_destruir(vuelo_t* vuelo){
	free_strv(vuelo->datos_separados);
	free(vuelo);
}

void borrar_del_abb(abb_t* abb, char** conjun){
	lista_t* lista = abb_obtener(abb, conjun[6]);
	lista_iter_t* iter = lista_iter_crear(lista);
	vuelo_t* actual = lista_iter_ver_actual(iter);
	while(strcmp(actual->datos_separados[0], conjun[0])!=0){
		lista_iter_avanzar(iter);
		actual = lista_iter_ver_actual(iter);
	}
	vuelo_t* anterior = lista_iter_borrar(iter);
	vuelo_destruir(anterior);
	lista_iter_destruir(iter);
}

bool agregar_archivo(const char* arch, vuelos_t* vuelos){
	//char* linea = NULL;
	size_t capacidad = 0;
	while(!feof(arch)){
		char* linea = NULL;
		//se resetea la posicion de memoria
		ssize_t largo = getline(&linea, &capacidad, archivo);
		char** conjun_aux = split(linea, ',');
		vuelo_t* vuelo = vuelo_crear(linea, conjun_aux);
		vuelo_t* resultado_h = hash_obtener(vuelos->hash, conjun_aux[0]);
		if(resultado_h){
			borrar_del_abb(vuelos->abb, conjun_aux)
		}
		if (!hash_guardar(vuelos->hash, conjun_aux[0], vuelo)) return false;
		lista_t* resultado_l = abb_obtener(vuelos->abb, conjun_aux[6]);
		if(resultado_l){
			lista_insertar_primero(resultado, linea);
		}else{
			lista_t* lista = lista_crear();
			lista_insertar_primero(lista, linea);
			if(!abb_guardar(vuelos->abb, conjun_aux[6], lista)){
				lista_destruir(lista, NULL);
				return false;
			}
		}
	}
	return true;
}

void print_error_o_OK(bool ok, char* comando_error){
	if (ok){
		printf("OK\n");
		return;
	}
	fprintf( stderr, "Error en comando %s\n", comando_error);
}

void print_error(char* comando_error){
	fprintf( stderr, "Error en comando %s\n", comando_error);
}

vuelos_t* crear_vuelos(){
	vuelos_t* vuelos = malloc(sizeof(vuelos));
	vuelos->arbol = abb_crear(strcmp, destruir_dato_abb);
	vuelos->hash = hash_crear(hash_destruir_dato_t destruir_dato_h);
	return vuelos;
}

int comp_numeros(int v1, int v2){
	if(v1 > v2){
		return 1;
	}
	if(v2 > v1){
		return -1;
	}
	return 0;
}

int comparador_de_n_de_vuelos(const void* v1, const void* v2){
	vuelo_t* vuelo1 = (vuelo_t*)v1;
	vuelo_t* vuelo2 = (vuelo_t*)v2;
	return comp_numeros(atoi(vuelo1->datos_separados[0]), atoi(vuelo1->datos_separados[0]));
}

int comparador_de_prior_de_vuelos(const void* v1, const void* v2){
	vuelo_t* vuelo1 = (vuelo_t*)v1;
	vuelo_t* vuelo2 = (vuelo_t*)v2;
	return comp_numeros(atoi(vuelo1->datos_separados[5]), atoi(vuelo1->datos_separados[5]));
}

void** ordenar_lista_por_n_de_vuelo(lista_t* lista){
	void** ordenar = malloc(sizeof(void*) * N);
	int tam = N;
	size_t cont = 0;
	ordenar[1] = NULL;
	lista_iter_t* iter = lista_iter_crear(lista);
	for(int i = 0; !lista_iter_al_final(iter); i++){
		if (i == tam){
			orndenar = realloc(ordenar, tam * RED * sizeof(void*));
			tam = tam * RED;
		}
		ordenar[i] = lista_iter_ver_actual(iter);
		lista_iter_avanzar(iter);
		cont++
	}
	lista_iter_destruir(iter);
	if (ordenar[1] == NULL) return ordenar;
	heapsort(ordernar, cont, comparador_de_n_de_vuelos);
	return ordenar;
}

int agregar_a_tda(void** datos, void* tda, char* condicion, int caso){
	int cont = 0;
	for (int i = 0; datos[i] != NULL; ++i){
		if (caso == 1){
			cola_encolar((cola_t*)tda, datos[i]);
		}
		else{
			pila_apilar((pila_t*)tda, datos[i]);
		}
		cont++;
	}
	return cont;
}

void** lista_de_vuelos_a_mostrar(void** listas, char* condicion, int n){
	size_t tam = N;
	void* tda;
	int caso = 0;
	if (strcmp(condicion, "asc") == 0){
		tda = (void*)cola_crear();
		caso = 1;
	}else{
		tda = (void*)pila_crear();
		caso = -1;
	}
	for (int i = 0; listas[i] != NULL && i < n; ++i){
		void** ordenados = ordenar_lista_por_n_de_vuelo(lista[i]);
		int cantidad = agregar_a_tda(odenados, tda, condicion, caso);
		free(ordenados);
	}
	void** list_a_mostr = malloc(sizeof(void*) * cantidad);
	if (caso == 1){
		cola_t* cola = (cola_t*)tda;
		for (int i = 0; !cola_esta_vacia(cola) && i < n; ++i){
			list_a_mostr[i] = cola_desencolar(cola);
		}
		cola_destruir(cola, NULL);
	}else{
		pila_t* pila = (pila_t*)tda;
		for (int i = 0; !pila_esta_vacia(pila) && i < n; ++i){
			list_a_mostr[i] = pila_desapilar(pila);
		}
		pila_destruir(pila);
	}
	return list_a_mostr;
}

bool char_isdigit_pos(const char* cad){
	for (int i = 0; cad[i] != '\0'; ++i){
		if(isdigit(cad[i])==0){
			return false;
		} 
	}
	return true;
}

bool ver_tablero(char* _cant, char* condicion, char* principo, char* fin, vuelos_t* vuelos){
	if (!char_isdigit_pos(_cant)) return false;
	int cant = atoi(_cant);
	if (cant == 0) return false;
	if (strcmp(condicion, "asc") == 0 || strcmp(condicion, "desc") == 0) return false;
	if (strcmp(principo, fin) > 0) return false;
	void** listas = abb_busc_iter_corte(vuelos->abb, principo, fin);
	if (listas == NULL){
		free(listas);
		return false;
	}
	void** list_a_mostr = lista_de_vuelos_a_mostrar(listas, condicion, cant);
	for(int i = 0, i < cant , i++){
		char** datos_vuelo = list_a_mostr[i]->datos_separados;
		printf("%s - %s \n", datos_vuelo[6], datos_vuelo[0]);
	}
	free(listas);
	free(list_a_mostr);
}

bool info_vuelo(const char* cod_vuelo, vuelos_t* vuelos){
	vuelo_t* vuelo = hash_obtener(const hash_t *hash, const char *clave);
	if (vuelo == NULL) return false;
	char* cad = join(vuelo->datos_separados, ' ');
	printf("%s\n", cad);
	return true;
}

bool prioridad_vuelos(int cant, vuelos_t* vuelos){
	heap_t* heap = heap_crear(comparador_de_prior_de_vuelos);
	if (heap == NULL) return false;
	hash_iter_t* iter = hash_iter_crear(vuelos->hash);
	while(!hash_iter_al_final(iter)){
		const char* clave = hash_iter_ver_actual(const hash_iter_t *iter);
		bool salio = heap_encolar(heap, (void*)hash_obtener(vuelos->hash, clave));
		if (!salio){
			heap_destruir(heap, NULL);
			hash_iter_destruir(iter);
			return false;
		}
		hash_iter_avanzar(hash_iter_t *iter);
	}
	vuelo_t* actual = (vuelo_t*)heap_desencolar(heap_t *heap);
	heap_t* heap_n_vuelos = heap_crear(comparador_de_prior_de_vuelos);
	heap_encolar(heap_n_vuelos, actual);
	for (int i = 1; i < cant && !heap_esta_vacio(heap); ++i){
		vuelo_t* vuelo = (vuelo_t*)heap_desencolar(heap_t *heap);
		if(atoi(vuelos->datos_separados[5]) == atoi(actual->datos_separados[5])){
			heap_encolar(heap_n_vuelos, vuelo);
		}
		else{
			while(!heap_esta_vacio(heap_n_vuelos)){
				actual = heap_desencolar(heap_n_vuelos);
				printf("%s - %s", actual->datos_separados[5], actual->datos_separados[0]);
			}
			actual = vuelo;
			heap_encolar(heap_n_vuelos, actual);
		}
	}
	heap_destruir(heap);
	heap_destruir(heap_n_vuelos);
	return true;
}

bool vuelos_borrar(const char* desde, const char* hasta, vuelos_t* vuelos){
	if (strcmp(desde, hasta) > 0) return false;
	void** listas = abb_busc_iter_corte(vuelos->abb, desde, hasta);
	if (listas == NULL){
		free(listas);
		return false;
	}
	for (int i = 0; listas[i] != NULL ; ++i){
		lista_iter_t* iter = lista_iter_crear(listas[i]);
		if (iter == NULL) continue;
		vuelo_t* fly = lista_ver_primero(lista);
		abb_borrar(vuelos->abb, fly->datos_separados[6]);
		while(!lista_iter_al_final(iter)){
			vuelo_t* vuelo = (vuelo_t*)lista_iter_ver_actual(iter);
			hash_borrar(vuelos->hash, vuelo->datos_separados[0]);
			char* vuelo_p = join(vuelo->datos_separados, ' ');
			printf("%s\n", vuelo_p);
			vuelo_destruir(vuelo);
			lista_iter_avanzar(iter);
		}
		lista_iter_destruir(iter);
		lista_destruir(listas[i], NULL);
	}
	free(listas);
	return true;
}

void main(){
	char* linea = malloc(sizeof(char) * MAX);
	size_t buff = MAX;
	size_t* buff_pnt = &buff;
	vuelos_t* vuelos = crear_vuelos();
	while(getline(&linea, buff_pnt, stdin) != -1){
		char** comando = split(linea, ' ');
		if (strcmp(comando[0], "agregar_archivo") == 0){
			if (comando[1] == NULL || comando[2] != NULL){
				print_error("agregar_archivo");
				continue;
			}
			bool salio = agregar_archivo(comando[1], vuelos);
			print_error_o_OK(salio, "agregar_archivo");
			continue;
		}
		if (strcmp(comando[0], "ver_tablero") == 0){
			for (int i = 0; i < 5; ++i){
				if (comando[i] == NULL){
					print_error("ver_tablero");
					continue;
				}
			}
			if (comando[5] != NULL){
				print_error("ver_tablero");
				continue;
			}
			bool salio = ver_tablero(comando[1],comando[2], comando[3], comando[4], vuelos);
			print_error_o_OK(salio, "ver_tablero");
			continue;

		}
		if (strcmp(comando[0], "info_vuelo") == 0){
			if (comando[1] == NULL || comando[2] != NULL){
				print_error("info_vuelo");
				continue;
			}
			bool salio = info_vuelo(comando[1], vuelos);
			print_error_o_OK(salio, "info_vuelo");
			continue;
		}
		if (strcmp(comando[0], "prioridad_vuelos") == 0){
			if (comando[1] == NULL || comando[2] != NULL){
				print_error("prioridad_vuelos");
				continue;
			}
			bool salio = prioridad_vuelos(comando[1], vuelos);
			print_error_o_OK(salio, "prioridad_vuelos");
			continue;
		}
		if (strcmp(comando[0], "borrar") == 0){
			if (comando[1] == NULL || comando[2] == NULL || comando[3] != NULL){
				print_error("borrar");
				continue;
			}
			bool salio = vuelos_borrar(comando[1], comando[2], vuelos);
			print_error_o_OK(salio, "borrar");
			continue;
		}
	}
}
