#include <string.h>
#include <stdlib.h>

#include "hash.h"

#define FACTOR_CARGA_MAXIMO 0.7

typedef struct nodo_hash {
	const char *clave;
	void *valor;
	struct nodo_hash *siguiente;
} nodo_hash_t;

struct hash {
	nodo_hash_t **tabla;
	size_t cantidad;
	size_t capacidad;
};

size_t funcion_hash(const char *clave)
{
	size_t num = 0;

	while (*clave) {
		num += (size_t) * (clave++);
	}

	return num;
}

bool hash_actualizar_valor(hash_t *hash, const char *clave, void *valor,
			   size_t pos, void **anterior)
{
	nodo_hash_t *actual = hash->tabla[pos];

	while (actual != NULL) {
		if (strcmp(actual->clave, clave) == 0) {
			if (anterior != NULL) {
				*anterior = actual->valor;
			}
			actual->valor = valor;
			return true;
		}
		actual = actual->siguiente;
	}

	if (anterior != NULL) {
		*anterior = NULL;
	}
	return false;
}

char *hash_duplicar_clave(const char *palabra)
{
	char *copia = malloc(strlen(palabra) + 1);
	if (!copia) {
		return NULL;
	}

	strcpy(copia, palabra);

	return copia;
}

nodo_hash_t *crear_nodo(const char *clave, void *valor)
{
	nodo_hash_t *nuevo = calloc(1, sizeof(nodo_hash_t));

	if (nuevo == NULL) {
		return NULL;
	}

	nuevo->clave = hash_duplicar_clave(clave);
	nuevo->valor = valor;

	return nuevo;
}

void insertar_nodo(hash_t *hash, nodo_hash_t *nodo, size_t pos)
{
	nodo_hash_t *actual = hash->tabla[pos];

	if (actual == NULL) {
		hash->tabla[pos] = nodo;
		return;
	}

	while (actual->siguiente != NULL) {
		actual = actual->siguiente;
	}

	actual->siguiente = nodo;
}

hash_t *rehashear(hash_t *hash, size_t nueva_capacidad)
{
	hash_t nuevo_hash;

	nuevo_hash.tabla = calloc(nueva_capacidad, sizeof(nodo_hash_t *));
	if (nuevo_hash.tabla == NULL) {
		return NULL;
	}
	nuevo_hash.capacidad = nueva_capacidad;
	nuevo_hash.cantidad = 0;

	for (size_t i = 0; i < hash->capacidad; i++) {
		nodo_hash_t *actual = hash->tabla[i];
		while (actual != NULL) {
			nodo_hash_t *siguiente = actual->siguiente;
			size_t nueva_posicion =
				funcion_hash(actual->clave) % nueva_capacidad;
			actual->siguiente = nuevo_hash.tabla[nueva_posicion];
			nuevo_hash.tabla[nueva_posicion] = actual;
			actual = siguiente;
			nuevo_hash.cantidad++;
		}
	}

	free(hash->tabla);
	hash->tabla = nuevo_hash.tabla;
	hash->capacidad = nuevo_hash.capacidad;
	hash->cantidad = nuevo_hash.cantidad;
	return hash;
}

void *quitar_nodo(hash_t *hash, const char *clave, size_t pos)
{
	nodo_hash_t *actual = hash->tabla[pos];
	nodo_hash_t *anterior = NULL;
	void *valor_a_retornar = NULL;

	while (actual != NULL) {
		if (strcmp(actual->clave, clave) == 0) {
			if (anterior == NULL) {
				hash->tabla[pos] = actual->siguiente;
			} else {
				anterior->siguiente = actual->siguiente;
			}
			valor_a_retornar = actual->valor;
			free((void *)actual->clave);
			free(actual);
			hash->cantidad--;
			return valor_a_retornar;
		}
		anterior = actual;
		actual = actual->siguiente;
	}

	return valor_a_retornar;
}

hash_t *hash_crear(size_t capacidad)
{
	hash_t *hash = calloc(1, sizeof(hash_t));

	if (hash == NULL) {
		return NULL;
	}

	if (capacidad < 3) {
		capacidad = 3;
	}

	hash->tabla = calloc(capacidad, sizeof(nodo_hash_t *));

	if (hash->tabla == NULL) {
		free(hash);
		return NULL;
	}
	hash->capacidad = capacidad;

	return hash;
}

hash_t *hash_insertar(hash_t *hash, const char *clave, void *elemento,
		      void **anterior)
{
	if (!hash || !clave) {
		return NULL;
	}

	size_t posicion = funcion_hash(clave) % hash->capacidad;

	if (hash_actualizar_valor(hash, clave, elemento, posicion, anterior) ==
	    true) {
		return hash;
	}

	if (hash->cantidad / hash->capacidad >= FACTOR_CARGA_MAXIMO) {
		if (rehashear(hash, hash->capacidad * 2) == NULL) {
			return NULL;
		}
		posicion = funcion_hash(clave) % hash->capacidad;
	}

	nodo_hash_t *nodo_a_insertar = crear_nodo(clave, elemento);

	if (!nodo_a_insertar) {
		return NULL;
	}

	insertar_nodo(hash, nodo_a_insertar, posicion);

	hash->cantidad++;

	return hash;
}

void *hash_quitar(hash_t *hash, const char *clave)
{
	if (!hash || !clave || hash_cantidad(hash) == 0)
		return NULL;

	size_t posicion = funcion_hash(clave) % hash->capacidad;

	return quitar_nodo(hash, clave, posicion);
}

void *hash_obtener(hash_t *hash, const char *clave)
{
	if (!hash || !clave || hash->cantidad == 0)
		return NULL;

	size_t pos = funcion_hash(clave) % hash->capacidad;

	nodo_hash_t *actual = hash->tabla[pos];
	while (actual != NULL) {
		if (strcmp(actual->clave, clave) == 0) {
			return actual->valor;
		}
		actual = actual->siguiente;
	}

	return NULL;
}

bool hash_contiene(hash_t *hash, const char *clave)
{
	if (!hash || hash->cantidad == 0 || !clave)
		return false;

	size_t pos = funcion_hash(clave) % hash->capacidad;

	nodo_hash_t *actual = hash->tabla[pos];

	while (actual != NULL) {
		if (strcmp(actual->clave, clave) == 0) {
			return true;
		}
		actual = actual->siguiente;
	}

	return false;
}

size_t hash_cantidad(hash_t *hash)
{
	if (!hash)
		return 0;

	return hash->cantidad;
}

void hash_destruir(hash_t *hash)
{
	if (!hash) {
		return;
	}
	if (hash->cantidad != 0) {
		for (size_t i = 0; i < hash->capacidad; i++) {
			nodo_hash_t *actual = hash->tabla[i];
			while (actual != NULL) {
				nodo_hash_t *siguiente = actual->siguiente;
				free((void *)actual->clave);
				free(actual);
				actual = siguiente;
			}
		}
	}
	free(hash->tabla);
	free(hash);
}

void hash_destruir_todo(hash_t *hash, void (*destructor)(void *))
{
	if (!hash) {
		return;
	}

	if (destructor == NULL) {
		hash_destruir(hash);
		return;
	}

	if (hash->cantidad != 0) {
		for (int i = 0; i < hash->capacidad; i++) {
			nodo_hash_t *actual = hash->tabla[i];
			while (actual != NULL) {
				nodo_hash_t *siguiente = actual->siguiente;
				destructor(actual->valor);
				free((void *)actual->clave);
				free(actual);
				actual = siguiente;
			}
		}
	}
	free(hash->tabla);
	free(hash);
}

size_t hash_con_cada_clave(hash_t *hash,
			   bool (*f)(const char *clave, void *valor, void *aux),
			   void *aux)
{
	if (!hash || !f)
		return 0;

	size_t n = 0;

	for (size_t i = 0; i < hash->capacidad; i++) {
		nodo_hash_t *actual = hash->tabla[i];
		while (actual != NULL) {
			if (f(actual->clave, actual->valor, aux)) {
				n++;
			} else {
				n++;
				return n;
			}
			actual = actual->siguiente;
		}
	}

	return n;
}
