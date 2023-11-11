#include "pa2m.h"
#include "src/hash.h"

bool sumar_uno_a_los_pares(const char *clave, void *valor, void *aux)
{
	if (!valor || !aux) {
		return false;
	}

	int *value = (int *)valor;
	int *sumar = (int *)aux;

	if (*value % 2 == 0) {
		*value += *sumar;
	}

	return true;
}

bool buscar(const char *clave, void *valor, void *aux)
{
	if (!valor) {
		return true;
	}

	if (*(int *)valor == *(int *)aux) {
		return false;
	}

	return true;
}

void restar_uno_a_todos(void *valor)
{
	if (!valor) {
		return;
	}

	(*(int *)valor)--;
}

void pruebas_crear_hash()
{
	hash_t *hash1 = hash_crear(1);
	hash_t *hash2 = hash_crear(15);
	hash_t *hash3 = hash_crear(200);

	pa2m_afirmar(
		hash1 != NULL,
		"Se prueba crear el hash con una capacidad inicial menor a 3");
	pa2m_afirmar(hash2 != NULL,
		     "Se prueba crear el hash con una capacidad de 15");
	pa2m_afirmar(hash3 != NULL,
		     "Se prueba crear el hash con una capacidad de 200");
	pa2m_afirmar(hash_cantidad(hash1) == 0,
		     "La cantidad de elementos del hash recién creado es 0");

	hash_destruir(hash1);
	hash_destruir(hash2);
	hash_destruir(hash3);
}

void pruebas_insertar_hash()
{
	hash_t *hash = hash_crear(10);
	int vector[] = { 0, 1, 3, 7, 8, 5, 2 };
	void *anterior;
	int tam = sizeof(vector) / sizeof(*vector);
	char *strings[] = { "NULL", "Uno",   "Tres", "Siete",
			    "Ocho", "Cinco", "Dos" };
	pa2m_afirmar(hash_insertar(NULL, strings[0], &vector[0], &anterior) ==
			     NULL,
		     "Se prueba insertar en un hash nulo, devuelve NULL");
	pa2m_afirmar(
		hash_insertar(hash, NULL, &vector[0], &anterior) == NULL,
		"Se prueba insertar una clave nula en un hash válido, devuelve NULL");
	pa2m_afirmar(
		hash_insertar(hash, strings[0], NULL, &anterior) == hash,
		"Se prueba insertar un elemento nulo con clave válida en un hash válido, devuelve el hash");

	pa2m_afirmar(hash_cantidad(hash) == 1, "La cantidad de pares es 1");

	pa2m_afirmar(
		hash_insertar(hash, strings[1], &vector[1], &anterior) == hash,
		"Se prueba insertar un elemento con clave válida en un hash válido, devuelve el hash");
	pa2m_afirmar(hash_cantidad(hash) == 2, "La cantidad de pares es 2");
	bool insertado_correctamente = true;
	for (int i = 2; i < tam; i++) {
		if (hash_insertar(hash, strings[i], &vector[i], &anterior) ==
		    NULL) {
			insertado_correctamente = false;
		}
	}
	pa2m_afirmar(
		hash_insertar(hash, strings[4], &vector[5], NULL) == hash &&
			hash_obtener(hash, strings[4]) == &vector[5],
		"Se prueba actualizar el valor de una clave, con un anterior NULL. No se guarda el anterior y se actualiza de todas formas");
	pa2m_afirmar(insertado_correctamente == true,
		     "Se prueba insertar varios elementos, devuelve el hash");
	pa2m_afirmar(hash_cantidad(hash) == 7, "La cantidad de pares es 7");
	pa2m_afirmar(hash_insertar(hash, strings[1], &vector[0], &anterior) ==
			     hash,
		     "Se prueba actualizar un elemento, devuelve el hash");
	pa2m_afirmar(anterior == &vector[1],
		     "El elemento anterior retornado es el correcto");

	hash_destruir(hash);
}

void pruebas_rehash()
{
	hash_t *hash = hash_crear(5);
	int vector[] = { 0, 1, 3, 7, 8, 5, 2, 10, 11, 12, 13, 14, 15, 20 };
	void *anterior;
	int tam = sizeof(vector) / sizeof(*vector);
	char *strings[] = { "Cero",  "Uno",	"Tres",	  "Siete", "Ocho",
			    "Cinco", "Dos",	"Diez",	  "Once",  "Doce",
			    "Trece", "Catorce", "Quince", "Veinte" };

	bool insertado_correctamente = true;
	for (int i = 0; i < tam; i++) {
		if (hash_insertar(hash, strings[i], &vector[i], &anterior) !=
		    hash) {
			insertado_correctamente = false;
		}
	}
	pa2m_afirmar(
		insertado_correctamente == true,
		"Se prueba insertar varios elementos y causar un rehash, devuelve el hash");

	hash_destruir(hash);
}

void pruebas_eliminacion()
{
	hash_t *hash = hash_crear(25);
	void *anterior;

	int vector[] = { 0, 1, 3, 7, 8, 5, 2, 10, 11, 12, 13, 14, 15, 20 };
	char *claves[] = { "Cero",  "Uno",     "Tres",	 "Siete", "Ocho",
			   "Cinco", "Dos",     "Diez",	 "Once",  "Doce",
			   "Trece", "Catorce", "Quince", "Veinte" };
	int tam = sizeof(vector) / sizeof(*vector);

	bool insertado_correctamente = true;
	for (int i = 1; i < tam; i++) {
		if (hash_insertar(hash, claves[i], &vector[i], &anterior) !=
		    hash) {
			insertado_correctamente = false;
		}
	}
	pa2m_afirmar(insertado_correctamente == true,
		     "Se insertan 14 elementos");
	pa2m_afirmar(hash_quitar(NULL, claves[0]) == NULL,
		     "Se prueba quitar de un hash nulo, devuelve NULL");
	pa2m_afirmar(hash_quitar(hash, NULL) == NULL,
		     "Se prueba quitar una clave nula, devuelve NULL");
	hash_t *hash_vacio = hash_crear(5);
	pa2m_afirmar(
		hash_quitar(hash_vacio, claves[0]) == NULL,
		"Se prueba quitar un elemento de un hash vacío, devuelve NULL");
	hash_destruir(hash_vacio);
	pa2m_afirmar(
		hash_quitar(hash, claves[0]) == NULL,
		"Se prueba quitar una clave que no existe en el hash, devuelve NULL");
	pa2m_afirmar(
		hash_quitar(hash, claves[9]) == (void *)&vector[9],
		"Se prueba quitar un elemento que sí existía, devuelve el elemento");
	pa2m_afirmar(
		hash_quitar(hash, claves[9]) == NULL,
		"Se prueba quitar un elemento que sí existía, y luego quitarlo de nuevo, devuelve NULL");

	hash_destruir(hash);
}

void pruebas_obtencion()
{
	hash_t *hash = hash_crear(25);
	void *anterior;
	int vector[] = { 0, 1, 3, 7, 8, 5, 2, 10, 11, 12, 13, 14, 15, 20 };
	char *claves[] = { "Cero",  "Uno",     "Tres",	 "Siete", "Ocho",
			   "Cinco", "Dos",     "Diez",	 "Once",  "Doce",
			   "Trece", "Catorce", "Quince", "Veinte" };
	int tam = sizeof(vector) / sizeof(*vector);

	bool insertado_correctamente = true;
	for (int i = 1; i < tam; i++) {
		if (hash_insertar(hash, claves[i], &vector[i], &anterior) !=
		    hash) {
			insertado_correctamente = false;
		}
	}
	pa2m_afirmar(insertado_correctamente == true,
		     "Se insertan 14 elementos");
	pa2m_afirmar(hash_obtener(NULL, claves[1]) == NULL,
		     "Se prueba obtener de un hash nulo, devuelve NULL");
	pa2m_afirmar(hash_obtener(hash, NULL) == NULL,
		     "Se prueba obtener una clave nula, devuelve NULL");
	pa2m_afirmar(
		hash_obtener(hash, claves[0]) == NULL,
		"Se prueba obtener una clave que no se encuentra en el hash, devuelve NULL");
	pa2m_afirmar(
		hash_obtener(hash, claves[11]) == (void *)&vector[11],
		"Se prueba obtener una clave que sí se encuentra en el hash, devuelve el elemento");
	hash_quitar(hash, claves[11]);
	pa2m_afirmar(
		hash_obtener(hash, claves[11]) == NULL,
		"Se prueba obtener una calve que sí se encuentra en el hash, quitarla, y luego verificar que ya no se encuentra");

	hash_destruir(hash);
}

void pruebas_contiene()
{
	hash_t *hash = hash_crear(25);
	void *anterior;
	int vector[] = { 0, 1, 3, 7, 8, 5, 2, 10, 11, 12, 13, 14, 15, 20 };
	char *claves[] = { "Cero",  "Uno",     "Tres",	 "Siete", "Ocho",
			   "Cinco", "Dos",     "Diez",	 "Once",  "Doce",
			   "Trece", "Catorce", "Quince", "Veinte" };
	int tam = sizeof(vector) / sizeof(*vector);

	pa2m_afirmar(
		hash_contiene(hash, claves[9]) == false,
		"Se comprueba si el hash contiene un elemento inexistente, devuelve FALSE");
	pa2m_afirmar(
		hash_contiene(NULL, claves[1]) == false,
		"Se comprueba si un hash nulo contiene un elemento, devuelve FALSE");
	pa2m_afirmar(
		hash_contiene(hash, NULL) == false,
		"Se comprueba si un hash contiene un elemento con una clave nula, devuelve FALSE");

	for (int i = 1; i < tam; i++)
		hash_insertar(hash, claves[i], &vector[i], &anterior);

	pa2m_afirmar(
		hash_contiene(hash, claves[12]) == true,
		"Se comprueba si un hash válido contiene un elemento con clave válida, devuelve TRUE");

	hash_destruir(hash);
}

void pruebas_con_funcion_destructora()
{
	hash_t *hash = hash_crear(25);
	void *anterior;
	int vector[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
	char *claves[] = { "Cero",  "Uno",  "Dos",   "Tres", "Cuatro",
			   "Cinco", "Seis", "Siete", "Ocho", "Nueve",
			   "Diez",  "Once", "Doce",  "Trece" };
	int tam = sizeof(vector) / sizeof(*vector);

	for (int i = 0; i < tam; i++) {
		hash_insertar(hash, claves[i], &vector[i], &anterior);
	}

	hash_destruir_todo(hash, restar_uno_a_todos);
	bool funcion_aplicada_correctamente = true;
	for (int i = 0; i < tam; i++) {
		if (vector[i] != i - 1) {
			funcion_aplicada_correctamente = false;
		}
	}

	pa2m_afirmar(
		funcion_aplicada_correctamente == true,
		"Se prueba invocar al función destructora con otra funcion auxiliar, la función es aplicada correctamente");
}

void pruebas_iterador_interno()
{
	hash_t *hash = hash_crear(25);
	void *anterior;
	int vector[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
	char *claves[] = { "Cero",  "Uno",  "Dos",   "Tres", "Cuatro",
			   "Cinco", "Seis", "Siete", "Ocho", "Nueve",
			   "Diez",  "Once", "Doce",  "Trece" };
	int tam = sizeof(vector) / sizeof(*vector);

	for (int i = 0; i < tam; i++) {
		hash_insertar(hash, claves[i], &vector[i], &anterior);
	}

	pa2m_afirmar(hash_con_cada_clave(NULL, sumar_uno_a_los_pares,
					 &vector[1]) == 0,
		     "Se prueba iterar un hash nulo, devuelve 0");
	pa2m_afirmar(
		hash_con_cada_clave(hash, NULL, &vector[1]) == 0,
		"Se prueba iterar un hash válido con una función nula, devuelve 0");
	pa2m_afirmar(
		hash_con_cada_clave(hash, sumar_uno_a_los_pares, NULL) == 1,
		"Se prueba iterar en su totalidad un hash con un auxiliar nulo, devuelve la cantidad de elementos iterados");
	size_t cant =
		hash_con_cada_clave(hash, sumar_uno_a_los_pares, &vector[1]);

	bool hash_iterador_correctamente = true;
	for (int i = 0; i < tam; i = i + 2) {
		if (vector[i] != i + 1) {
			hash_iterador_correctamente = false;
		}
	}

	pa2m_afirmar(
		hash_iterador_correctamente == true &&
			cant == hash_cantidad(hash),
		"Se prueba iterar un hash en su totalidad, devuelve la cantidad de elementos");
	pa2m_afirmar(
		hash_con_cada_clave(hash, buscar, &vector[9]) == 7,
		"Se prueba iterar un hash deteniéndose antes de terminar de visitar todos los elementos");

	hash_destruir(hash);
}

int main()
{
	pa2m_nuevo_grupo(
		"\n======================== PRUEBAS TDA HASH - PADRÓN: 110703 ========================");
	pa2m_nuevo_grupo("PRUEBAS CREAR");
	pruebas_crear_hash();
	pa2m_nuevo_grupo("PRUEBAS INSERTAR");
	pruebas_insertar_hash();
	pruebas_rehash();
	pa2m_nuevo_grupo("PRUEBAS QUITAR");
	pruebas_eliminacion();
	pa2m_nuevo_grupo("PRUEBAS OBTENER");
	pruebas_obtencion();
	pa2m_nuevo_grupo("PRUEBAS CONTIENE");
	pruebas_contiene();
	pa2m_nuevo_grupo("PRUEBAS CON FUNCIÓN DESTRUCTORA");
	pruebas_con_funcion_destructora();
	pa2m_nuevo_grupo("PRUEBAS DE ITERADOR INTERNO");
	pruebas_iterador_interno();

	return pa2m_mostrar_reporte();
}
