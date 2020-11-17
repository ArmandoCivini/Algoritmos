#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#define TAM_INICIAL   30
#define FACTOR_DE_REDIMENSION 2

typedef int (*cmp_func_t) (const void *a, const void *b);

typedef struct heap {
    void** datos;
    size_t cant;
    size_t cap;
    cmp_func_t comparador;
}heap_t;

heap_t *heap_crear(cmp_func_t cmp){
	heap_t* heap = malloc(sizeof(heap_t));
	if (heap == NULL){
        return NULL;
    }
    heap->datos = malloc(TAM_INICIAL * sizeof(void*));
    if (heap->datos == NULL){
        free(heap);
        return NULL;
    }
    heap->cap = TAM_INICIAL;
    heap->cant = 0;
    heap->comparador = cmp;
    return heap;
}

bool heap_redimensionar(heap_t* heap, size_t nuevo_tam){
	void** nuevo_datos;
	nuevo_datos = realloc(heap->datos, (nuevo_tam) * sizeof(void*));
	if(nuevo_datos == NULL) return false;
	heap->datos = nuevo_datos;
	heap->cap = nuevo_tam;
	return true;
}

void swap(void** arr, size_t x, size_t y){
	void* cambio = arr[x];
	arr[x] = arr[y];
	arr[y] = cambio;
}

void upheap(heap_t* heap, size_t pos){
	if (pos == 0) return;
	size_t padre;
	if(pos % 2 == 0){
		padre = (pos-2)/2;
	}else{
		padre = (pos-1)/2;
	}
	if (heap->comparador(heap->datos[padre], heap->datos[pos]) < 0){
		swap(heap->datos, padre, pos);
		upheap(heap, padre);
	}
}

void downheap(heap_t* heap, size_t pos){
	if (pos >= heap->cant) return;
	size_t min = pos;
	size_t izq = (pos * 2) + 1;
	size_t der = (pos * 2) + 2;
	if (izq < heap->cant && heap->comparador(heap->datos[min], heap->datos[izq]) < 0) min = izq;
	if (der < heap->cant && heap->comparador(heap->datos[min], heap->datos[der]) < 0) min = der;
	if (min != pos){
		swap(heap->datos, pos, min);
		downheap(heap, min);
	}
}

void heapify(heap_t* heap, size_t pos){
	if (pos > heap->cant) return;
	heapify(heap, (pos * 2) + 1);
	heapify(heap, (pos * 2) + 2);
	downheap(heap, pos);
}



void heap_destruir(heap_t *heap, void destruir_elemento(void *e)){
	void** arr = heap->datos;
	for (int i = 0; i < heap->cant; ++i){
		if (destruir_elemento != NULL) destruir_elemento(heap->datos[i]);
	}
	free(arr);
	free(heap);
}

size_t heap_cantidad(const heap_t *heap){
	return heap->cant;
}

bool heap_esta_vacio(const heap_t *heap){
	return (heap->cant == 0);
}

bool heap_encolar(heap_t *heap, void *elem){
	if (heap->cant == heap->cap){
		bool redim = heap_redimensionar(heap, heap->cap * FACTOR_DE_REDIMENSION);
		if (!redim) return false;
	}
	heap->datos[heap->cant] = elem;
	heap->cant++;
	upheap(heap, heap->cant - 1);
	return true;
}

heap_t *heap_crear_arr(void *arreglo[], size_t n, cmp_func_t cmp){
	if (arreglo == NULL || n == 0){
		return NULL;
	}
	heap_t* heap = heap_crear(cmp);
	if (heap->cap < n){
		heap->datos = realloc(heap->datos, sizeof(void*) * n);
		heap->cap = n;
	}
	if (heap == NULL) return NULL;
	for (int i = 0; i < n; ++i){
		heap->datos[i] = arreglo[i];
	}
	heap->cant = n;
	heapify(heap, 0);
	return heap;
}

void *heap_ver_max(const heap_t *heap){
	if (heap->cant == 0) return NULL;
	return heap->datos[0];
}

void *heap_desencolar(heap_t *heap){
	if ((heap->cant * 4) < heap->cap){
		bool redim = heap_redimensionar(heap, heap->cap / FACTOR_DE_REDIMENSION);
		if (!redim) return NULL;
	}
	if (heap->cant == 0) return NULL;
	void* prim = heap->datos[0];
	heap->cant--;
	heap->datos[0] = heap->datos[heap->cant];
	heap->datos[heap->cant] = NULL;
	downheap(heap, 0);
	return prim;
}

void heap_sort(void *elementos[], size_t cant, cmp_func_t cmp){
	heap_t* heap = heap_crear_arr(elementos, cant, cmp);
	if(!heap){
		return;
	}
	while (heap->cant != 0){
		swap(heap->datos, 0, heap->cant - 1);
		heap->cant--;
		downheap(heap, 0);
	}
	for (int i = 0; i < cant; ++i)
	{
		elementos[i] = heap->datos[i];
	}
	heap->cant = cant;
	heap_destruir(heap, NULL);
}

