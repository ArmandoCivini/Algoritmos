#include <stdlib.h>
#ifndef HEAP_H
#define HEAP_H


typedef struct heap heap_t;

typedef int (*cmp_func_t) (const void *a, const void *b);


//crea un heap vacio
//pre: se pasó función de  comparación
//post: devuelve un NULL si falló algo sino devuelve un heap
heap_t *heap_crear(cmp_func_t cmp);

//redimensiona el tamaño del arreglo del heap
//pre: el heap fue creado y el nuevo tamaño del arreglo
//post: devuelve true si se redimensionó y en caso contrario false
bool heap_redimensionar(heap_t* heap, size_t nuevo_tam);

//compara un dato con su padre y si es mayor los intercambia y se llama devuelta
//pre: el heap fue creado y la posicion donde se quiere hacer un upheap
void upheap(heap_t* heap, size_t pos);

//compara un dato con sus hijos y si alguno es menor se intercambia con el menor de ellos
//pre: el heap fue creado y la posicion donde se quiere hacer un upheap
void downheap(heap_t* heap, size_t pos);

//convierte un arreglo dentro de un heap en un heap de maximos
//pre: el heap fue creado y la posicion desde donde se quiere hacer un heap
void heapify(heap_t* heap, size_t pos);

//convierte un arreglo en un heap
//pre: un arreglo de void* su cantidad de datos y como se comparan sus datos
//post: devuelve un heap o NULL en caso de que algo haya salido mal
heap_t *heap_crear_arr(void *arreglo[], size_t n, cmp_func_t cmp);

//destruye el heap
//pre: el heap fue creado y se para como destruir los datos de ese heap
void heap_destruir(heap_t *heap, void destruir_elemento(void *e));

//cantidad de elementos del heap
//pre: el heap fue creado
//post: devuelve la cantidad de elementos del heap como size_t
size_t heap_cantidad(const heap_t *heap);

//determina si el heap esta vacio
//pre: el heap fue creado
//post: devuelve true si el heap esta vacio, false en caso contrario
bool heap_esta_vacio(const heap_t *heap);

//encola un elemento en el heap y lo vuelve a ordenar para que mantenga la condicion
//pre: el heap fue creado y se pasa un elemento para encolar
//post: devuelve true si se encoló el elemento, false en caso contrario
bool heap_encolar(heap_t *heap, void *elem);

//devuelve el mayor dato en el heap
//pre: el heap fue creado
//post: devuelve el mayor dato del heap
void *heap_ver_max(const heap_t *heap);

//desencola un elemento en el heap y lo vuelve a ordenar para que mantenga la condicion
//pre: el heap fue creado
//post: devuelve true si se desencoló el elemento, false en caso contrario
void *heap_desencolar(heap_t *heap);

//ordena un arreglo mediante el uso de un heap
//pre: se pasa el arreglo, la cantidad de datos y como se comparan estos
void heap_sort(void *elementos[], size_t cant, cmp_func_t cmp);


#endif //HEAP_H