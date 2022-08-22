#ifndef JSON_H_
#define JSON_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define json_containerOf(ptr, type, member) \
	((type *)((char *)ptr - offsetof(type, member)))

/** Enumeration of codes of supported JSON properties types. */
typedef enum
{
	JSON_OBJ,
	JSON_ARRAY,
	JSON_TEXT,
	JSON_BOOLEAN,
	JSON_INTEGER,
	JSON_REAL,
	JSON_NULL
} jsonType_t;

/** Structure to handle JSON properties. */
typedef struct json_s
{

	struct json_s *sibling;
	char const *name;

	union
	{
		char const *value;
		struct
		{
			struct json_s *child;
			struct json_s *last_child;
		} c;
	} u;

	jsonType_t type;

} json_t;

/** Print the value os a json object or array.
  * @param json The handler of the json object or array. */
extern void dump(json_t const *json);

extern void readFile(const char *nameFile, char *buffer);

/** Parse a string to get a json.
  * @param str String pointer with a JSON object. It will be modified.
  * @param mem Array of json properties to allocate.
  * @param qty Number of elements of mem.
  * @retval Null pointer if any was wrong in the parse process.
  * @retval If the parser process was successfully a valid handler of a json.
  *         This property is always unnamed and its type is JSON_OBJ. */
json_t const *json_create(char *str, json_t mem[], unsigned int qty);

/** Get the name of a json property.
  * @param json A valid handler of a json property.
  * @retval Pointer to null-terminated if property has name.
  * @retval Null pointer if the property is unnamed. */
static inline char const *json_getName(json_t const *json)
{
	return json->name;
}

/** Get the value of a json property.
  * The type of property cannot be JSON_OBJ or JSON_ARRAY.
  * @param json A valid handler of a json property.
  * @return Pointer to null-terminated string with the value. */
static inline char const *json_getValue(json_t const *property)
{
	return property->u.value;
}

/** Get the type of a json property.
  * @param json A valid handler of a json property.
  * @return The code of type.*/
static inline jsonType_t json_getType(json_t const *json)
{
	return json->type;
}

/** Get the next sibling of a JSON property that is within a JSON object or array.
  * @param json A valid handler of a json property.
  * @retval The handler of the next sibling if found.
  * @retval Null pointer if the json property is the last one. */
static inline json_t const *json_getSibling(json_t const *json)
{
	return json->sibling;
}

/** Search a property by its name in a JSON object.
  * @param obj A valid handler of a json object. Its type must be JSON_OBJ.
  * @param property The name of property to get.
  * @retval The handler of the json property if found.
  * @retval Null pointer if not found. */
json_t const *json_getProperty(json_t const *obj, char const *property);

/** Search a property by its name in a JSON object and return its value.
  * @param obj A valid handler of a json object. Its type must be JSON_OBJ.
  * @param property The name of property to get.
  * @retval If found a pointer to null-terminated string with the value.
  * @retval Null pointer if not found or it is an array or an object. */
char const *json_getPropertyValue(json_t const *obj, char const *property);

/** Get the first property of a JSON object or array.
  * @param json A valid handler of a json property.
  *             Its type must be JSON_OBJ or JSON_ARRAY.
  * @retval The handler of the first property if there is.
  * @retval Null pointer if the json object has not properties. */
static inline json_t const *json_getChild(json_t const *json)
{
	return json->u.c.child;
}

/** Get the value of a json boolean property.
  * @param property A valid handler of a json object. Its type must be JSON_BOOLEAN.
  * @return The value stdbool. */
static inline bool json_getBoolean(json_t const *property)
{
	return *property->u.value == 't';
}

/** Get the value of a json integer property.
  * @param property A valid handler of a json object. Its type must be JSON_INTEGER.
  * @return The value stdint. */
static inline int64_t json_getInteger(json_t const *property)
{
	return atoll(property->u.value);
}

/** Get the value of a json real property.
  * @param property A valid handler of a json object. Its type must be JSON_REAL.
  * @return The value. */
// static inline double json_getReal(json_t const *property)
// {
// 	return atof(property->u.value);
// }
double json_getReal(json_t const *property);

/** Structure to handle a heap of JSON properties. */
typedef struct jsonPool_s jsonPool_t;
struct jsonPool_s
{
	json_t *(*init)(jsonPool_t *pool);
	json_t *(*alloc)(jsonPool_t *pool);
};

/** Parse a string to get a json.
  * @param str String pointer with a JSON object. It will be modified.
  * @param pool Custom json pool pointer.
  * @retval Null pointer if any was wrong in the parse process.
  * @retval If the parser process was successfully a valid handler of a json.
  *         This property is always unnamed and its type is JSON_OBJ. */
json_t const *json_createWithPool(char *str, jsonPool_t *pool);

// JSON OBJECT CREATE

//take care: https://github.com/gad1231/JSON_C

typedef enum NUMBER
{
	INTEGER,
	DOUBLE,
	STRING
} NUMBER_t;


// -------------------- JSON OUTPUT --------------------

extern void jW_Start_obj(char *JS, int *JS_start);
extern void jW_End_obj(char *JS, int *JS_len);
extern void jW_String(char *key, char *value, char *JS, int *JS_idx);
extern void jW_Object(char *key, char *value, char *JS, int *JS_idx);
extern void jW_Num(char *key, double value, NUMBER_t IntegerOrDouble, char *JS, int *JS_idx);
extern void jW_Array(char *key, char *value, char *JS, int *JS_idx);


#define JSON_START_OBJ(JSON_OBJ_FATHER, INDEX) jW_Start_obj(JSON_OBJ_FATHER, INDEX)
#define JSON_END_OBJ(JSON_OBJ_FATHER, INDEX) jW_End_obj(JSON_OBJ_FATHER, INDEX)

#define JSON_ADD_OBJ(KEY, VALUE, JSON_OBJ_FATHER, INDEX) jW_Object(KEY, VALUE, JSON_OBJ_FATHER, INDEX)
#define JSON_ADD_ARRAY(KEY, VALUE, JSON_OBJ_FATHER, INDEX) jW_Array(KEY, VALUE, JSON_OBJ_FATHER, INDEX)

#define JSON_ADD_STRING(KEY, VALUE, JSON_OBJ_FATHER, INDEX) jW_String(KEY, VALUE, JSON_OBJ_FATHER, INDEX)
#define JSON_ADD_INT(KEY, VALUE, JSON_OBJ_FATHER, INDEX) jW_Num(KEY, VALUE, INTEGER, JSON_OBJ_FATHER, INDEX)
#define JSON_ADD_DOUBLE(KEY, VALUE, JSON_OBJ_FATHER, INDEX) jW_Num(KEY, VALUE, DOUBLE, JSON_OBJ_FATHER, INDEX)


// Utils to use with jw_Array
extern int getIntArrayAsString(int *array, int size, char *string, int stringMaxSize);
extern int getDoubleArrayAsString(double *array, int size, char *string, int stringMaxSize);
extern int getDoubleMatrixAsString(double *matrix, int sizeX, int sizeY, char *string, int stringMaxSize);

extern void jW_Start_array(char *JS, int *JS_start);
extern void jW_End_array(char *JS, int *JS_len);
extern void jW_Object_add_to_array(char *value, char *JS, int *JS_idx);


#define JSON_CREATE_INT_ARRAY(ARRAY, ARRAY_SIZE, JSON_NEW_ARRAY, JSON_ARRAY_MAX_SIZE)\
  getIntArrayAsString(ARRAY, ARRAY_SIZE, JSON_NEW_ARRAY, JSON_ARRAY_MAX_SIZE)

#define JSON_CREATE_DOUBLE_ARRAY(ARRAY, ARRAY_SIZE, JSON_NEW_ARRAY, JSON_ARRAY_MAX_SIZE)\
  getDoubleArrayAsString(ARRAY, ARRAY_SIZE, JSON_NEW_ARRAY, JSON_ARRAY_MAX_SIZE)

#define JSON_CREATE_DOUBLE_MATRIX(MATRIX, MATRIX_X_SIZE, MATRIX_Y_SIZE, JSON_NEW_MATRIX, JSON_MATRIX_MAX_SIZE)\
  getDoubleMatrixAsString(MATRIX, MATRIX_X_SIZE, MATRIX_Y_SIZE, JSON_NEW_MATRIX, JSON_MATRIX_MAX_SIZE)

#define JSON_START_ARRAY(ARRAY, INDEX) jW_Start_array(ARRAY, INDEX)
#define JSON_END_ARRAY(ARRAY, INDEX) jW_End_array(ARRAY, INDEX)
#define JSON_ADD_OBJ_TO_ARRAY(OBJ, ARRAY, INDEX) jW_Object_add_to_array(OBJ, ARRAY, INDEX)

// -------------------- JSON INPUT --------------------

#define JSON_PARSE(STRING, JSON_STRUCT, SIZE) json_create(STRING, JSON_STRUCT, SIZE)

#define JSON_GET_PROPERTY_NAME(JSON_STRUCT) json_getName(JSON_STRUCT)
#define JSON_GET_PROPERTY_VALUE(JSON_STRUCT) json_getValue(JSON_STRUCT)
#define JSON_GET_PROPERTY_TYPE(JSON_STRUCT) json_getType(JSON_STRUCT)

#define JSON_GET_PROPERTY_VALUE_AS_BOOLEAN(JSON_STRUCT) json_getBoolean(JSON_STRUCT)
#define JSON_GET_PROPERTY_VALUE_AS_INT(JSON_STRUCT) json_getInteger(JSON_STRUCT)
#define JSON_GET_PROPERTY_VALUE_AS_DOUBLE(JSON_STRUCT) json_getReal(JSON_STRUCT)

#define JSON_GET_SIBLING(JSON_STRUCT) json_getSibling(JSON_STRUCT)
#define JSON_GET_CHILD(JSON_STRUCT) json_getChild(JSON_STRUCT)

#define JSON_GET_PROPERTY(JSON_STRUCT, PROPERTY) json_getProperty(JSON_STRUCT, PROPERTY)
#define JSON_GET_PROPERTY_VALUE_BY_NAME(JSON_STRUCT, PROPERTY) json_getPropertyValue(JSON_STRUCT, PROPERTY)

#endif /* JSON_H_ */
