#include <ctype.h>

#include "json.h"
#include "log.h"

/** Structure to handle a heap of JSON properties. */
typedef struct jsonStaticPool_s
{
	json_t *mem;		   /**< Pointer to array of json properties.      */
	unsigned int qty;	   /**< Length of the array of json properties.   */
	unsigned int nextFree; /**< The index of the next free json property. */
	jsonPool_t pool;
} jsonStaticPool_t;

/* Search a property by its name in a JSON object. */
json_t const *json_getProperty(json_t const *obj, char const *property)
{
	json_t const *sibling;

	for (sibling = obj->u.c.child; sibling; sibling = sibling->sibling)
		if (sibling->name && !strcmp(sibling->name, property))
			return sibling;

	return 0;
}

/* Search a property by its name in a JSON object and return its value. */
char const *json_getPropertyValue(json_t const *obj, char const *property)
{
	json_t const *field = json_getProperty(obj, property);

	if (!field)
		return 0;
	jsonType_t type = json_getType(field);
	if (JSON_ARRAY >= type)
		return 0;
	return json_getValue(field);
}

double json_getReal(json_t const *property)
{
	for (uint16_t index = 0; property->u.value[index] != 0; index += 1)
	{
		if (property->u.value[index] == '.') {
			return atof(property->u.value);
		}
	}
	return 0.0 + atoll(property->u.value);
}

/* Internal prototypes: */
static char *goBlank(char *str);
static char *goNum(char *str);
static json_t *poolInit(jsonPool_t *pool);
static json_t *poolAlloc(jsonPool_t *pool);
static char *objValue(char *ptr, json_t *obj, jsonPool_t *pool);
static char *setToNull(char *ch);
static bool isEndOfPrimitive(char ch);

void dump(json_t const *json)
{

	jsonType_t const type = json_getType(json);
	if (type != JSON_OBJ && type != JSON_ARRAY)
	{
		puts("error");
		return;
	}

	printf("%s", type == JSON_OBJ ? " {" : " [");

	json_t const *child;
	for (child = json_getChild(json); child != 0; child = json_getSibling(child))
	{

		jsonType_t propertyType = json_getType(child);
		char const *name = json_getName(child);
		if (name)
			printf(" \"%s\": ", name);

		if (propertyType == JSON_OBJ || propertyType == JSON_ARRAY)
			dump(child);

		else
		{
			char const *value = json_getValue(child);
			if (value)
			{
				bool const text = JSON_TEXT == json_getType(child);
				char const *fmt = text ? " \"%s\"" : " %s";
				printf(fmt, value);
				bool const last = !json_getSibling(child);
				if (!last)
					putchar(',');
			}
		}
	}

	printf("%s", type == JSON_OBJ ? " }" : " ]");
}

/* Parse a string to get a json. */
json_t const *json_createWithPool(char *str, jsonPool_t *pool)
{
	char *ptr = goBlank(str);

	if (!ptr || *ptr != '{')
		return 0;

	json_t *obj = pool->init(pool);
	obj->name = 0;
	obj->sibling = 0;
	obj->u.c.child = 0;
	ptr = objValue(ptr, obj, pool);

	if (!ptr)
		return 0;
	return obj;
}

/* Parse a string to get a json. */
json_t const *json_create(char *str, json_t mem[], unsigned int qty)
{
	jsonStaticPool_t spool;
	spool.mem = mem;
	spool.qty = qty;
	spool.pool.init = poolInit;
	spool.pool.alloc = poolAlloc;
	return json_createWithPool(str, &spool.pool);
}

static char getEscape(char ch)
{
	static struct
	{
		char ch;
		char code;
	} const pair[] = {
		{'\"', '\"'},
		{'\\', '\\'},
		{'/', '/'},
		{'b', '\b'},
		{'f', '\f'},
		{'n', '\n'},
		{'r', '\r'},
		{'t', '\t'},
	};

	unsigned int i;

	for (i = 0; i < sizeof pair / sizeof *pair; ++i)
		if (pair[i].ch == ch)
			return pair[i].code;
	return '\0';
}

static unsigned char getCharFromUnicode(unsigned char const *str)
{
	unsigned int i;

	for (i = 0; i < 4; ++i)
		if (!isxdigit(str[i]))
			return '\0';
	return '?';
}

static char *parseString(char *str)
{
	unsigned char *head = (unsigned char *)str;
	unsigned char *tail = (unsigned char *)str;

	for (; *head; ++head, ++tail)
	{
		if (*head == '\"')
		{
			*tail = '\0';
			return (char *)++head;
		}
		if (*head == '\\')
		{
			if (*++head == 'u')
			{
				char const ch = getCharFromUnicode(++head);
				if (ch == '\0')
					return 0;
				*tail = ch;
				head += 3;
			}
			else
			{
				char const esc = getEscape(*head);
				if (esc == '\0')
					return 0;
				*tail = esc;
			}
		}
		else
			*tail = *head;
	}
	return 0;
}

static char *propertyName(char *ptr, json_t *property)
{
	property->name = ++ptr;
	ptr = parseString(ptr);

	if (!ptr)
		return 0;

	ptr = goBlank(ptr);

	if (!ptr)
		return 0;

	if (*ptr++ != ':')
		return 0;

	return goBlank(ptr);
}

static char *textValue(char *ptr, json_t *property)
{
	++property->u.value;
	ptr = parseString(++ptr);

	if (!ptr)
		return 0;

	property->type = JSON_TEXT;

	return ptr;
}

static char *checkStr(char *ptr, char const *str)
{
	while (*str)
		if (*ptr++ != *str++)
			return 0;
	return ptr;
}

static char *primitiveValue(char *ptr, json_t *property, char const *value, jsonType_t type)
{
	ptr = checkStr(ptr, value);

	if (!ptr || !isEndOfPrimitive(*ptr))
		return 0;

	ptr = setToNull(ptr);
	property->type = type;

	return ptr;
}

static char *trueValue(char *ptr, json_t *property)
{
	return primitiveValue(ptr, property, "true", JSON_BOOLEAN);
}

static char *falseValue(char *ptr, json_t *property)
{
	return primitiveValue(ptr, property, "false", JSON_BOOLEAN);
}

static char *nullValue(char *ptr, json_t *property)
{
	return primitiveValue(ptr, property, "null", JSON_NULL);
}

static char *expValue(char *ptr)
{
	if (*ptr == '-' || *ptr == '+')
		++ptr;
	if (!isdigit((int)(*ptr)))
		return 0;
	ptr = goNum(++ptr);

	return ptr;
}

static char *fraqValue(char *ptr)
{
	if (!isdigit((int)(*ptr)))
		return 0;

	ptr = goNum(++ptr);

	if (!ptr)
		return 0;

	return ptr;
}

static char *numValue(char *ptr, json_t *property)
{
	if (*ptr == '-')
		++ptr;

	if (!isdigit((int)(*ptr)))
		return 0;

	if (*ptr != '0')
	{
		ptr = goNum(ptr);
		if (!ptr)
			return 0;
	}
	else if (isdigit((int)(*++ptr)))
		return 0;

	property->type = JSON_INTEGER;

	if (*ptr == '.')
	{
		ptr = fraqValue(++ptr);
		if (!ptr)
			return 0;
		property->type = JSON_REAL;
	}

	if (*ptr == 'e' || *ptr == 'E')
	{
		ptr = expValue(++ptr);
		if (!ptr)
			return 0;
		property->type = JSON_REAL;
	}

	if (!isEndOfPrimitive(*ptr))
		return 0;

	if (JSON_INTEGER == property->type)
	{
		char const *value = property->u.value;
		bool const negative = *value == '-';
		static char const min[] = "-9223372036854775808";
		static char const max[] = "9223372036854775807";
		unsigned int const maxdigits = (negative ? sizeof min : sizeof max) - 1;
		unsigned int const len = (unsigned int const)(ptr - value);
		if (len > maxdigits)
			return 0;
		if (len == maxdigits)
		{
			char const tmp = *ptr;
			*ptr = '\0';
			char const *const threshold = negative ? min : max;
			if (0 > strcmp(threshold, value))
				return 0;
			*ptr = tmp;
		}
	}

	ptr = setToNull(ptr);

	return ptr;
}

static void add(json_t *obj, json_t *property)
{
	property->sibling = 0;
	if (!obj->u.c.child)
	{
		obj->u.c.child = property;
		obj->u.c.last_child = property;
	}
	else
	{
		obj->u.c.last_child->sibling = property;
		obj->u.c.last_child = property;
	}
}

static char *objValue(char *ptr, json_t *obj, jsonPool_t *pool)
{
	obj->type = JSON_OBJ;
	obj->u.c.child = 0;
	obj->sibling = 0;
	ptr++;

	for (;;)
	{
		ptr = goBlank(ptr);
		if (!ptr)
			return 0;

		if (*ptr == ',')
		{
			++ptr;
			continue;
		}

		char const endchar = (obj->type == JSON_OBJ) ? '}' : ']';
		if (*ptr == endchar)
		{
			*ptr = '\0';
			json_t *parentObj = obj->sibling;
			if (!parentObj)
				return ++ptr;
			obj->sibling = 0;
			obj = parentObj;
			++ptr;
			continue;
		}

		json_t *property = pool->alloc(pool);
		if (!property)
			return 0;

		if (obj->type != JSON_ARRAY)
		{
			if (*ptr != '\"')
				return 0;
			ptr = propertyName(ptr, property);
			if (!ptr)
				return 0;
		}
		else
			property->name = 0;
		add(obj, property);
		property->u.value = ptr;

		switch (*ptr)
		{
		case '{':
			property->type = JSON_OBJ;
			property->u.c.child = 0;
			property->sibling = obj;
			obj = property;
			++ptr;
			break;
		case '[':
			property->type = JSON_ARRAY;
			property->u.c.child = 0;
			property->sibling = obj;
			obj = property;
			++ptr;
			break;
		case '\"':
			ptr = textValue(ptr, property);
			break;
		case 't':
			ptr = trueValue(ptr, property);
			break;
		case 'f':
			ptr = falseValue(ptr, property);
			break;
		case 'n':
			ptr = nullValue(ptr, property);
			break;
		default:
			ptr = numValue(ptr, property);
			break;
		}
		if (!ptr)
			return 0;
	}
}

static json_t *poolInit(jsonPool_t *pool)
{
	jsonStaticPool_t *spool = json_containerOf(pool, jsonStaticPool_t, pool);
	spool->nextFree = 1;

	return spool->mem;
}

static json_t *poolAlloc(jsonPool_t *pool)
{
	jsonStaticPool_t *spool = json_containerOf(pool, jsonStaticPool_t, pool);
	if (spool->nextFree >= spool->qty)
		return 0;

	return spool->mem + spool->nextFree++;
}

static bool isOneOfThem(char ch, char const *set)
{
	while (*set != '\0')
		if (ch == *set++)
			return true;
	return false;
}

static char *goWhile(char *str, char const *set)
{
	for (; *str != '\0'; ++str)
	{
		if (!isOneOfThem(*str, set))
			return str;
	}

	return 0;
}

/** Set of characters that defines a blank. */
static char const *const blank = " \n\r\t\f";

static char *goBlank(char *str)
{
	return goWhile(str, blank);
}

static char *goNum(char *str)
{
	for (; *str != '\0'; ++str)
	{
		if (!isdigit((int)(*str)))
			return str;
	}

	return 0;
}

/** Set of characters that defines the end of an array or a JSON object. */
static char const *const endofblock = "}]";

static char *setToNull(char *ch)
{
	if (!isOneOfThem(*ch, endofblock))
		*ch++ = '\0';

	return ch;
}

/** Indicate if a character is the end of a primitive value. */
static bool isEndOfPrimitive(char ch)
{
	return ch == ',' || isOneOfThem(ch, blank) || isOneOfThem(ch, endofblock);
}

// JSON OBJECT CREATE

void ftos(char *s, double number, int length);
void itos(char *s, long long int number);

/********************************************************
**function describtion: reverse the contents of string
**input: pointer to string,length of string ,start index
**output: none
***********************************************************/
void reverse(char *s, int len, int start_index)
{
	int j = len - 1, temp = 0;
	while (start_index < j)
	{
		temp = s[j];
		s[j--] = s[start_index];
		s[start_index++] = temp;
	}
	return;
}

/************************************************************************
** function name: jW_Start_obj
** function description: put cerly opening bracket { in the first index of JS
** inputs: -JS : json string buffer
		   -JS_start: index to put {
** output: None
************************************************************************/
void jW_Start_obj(char *JS, int *JS_start)
{
	//if(!(*JS_start))
	JS[(*JS_start)++] = '{';
	return;
}

/************************************************************************
** function name: jW_End_obj
** function description: put cerly closing bracket } in the last index of JS
** inputs: -JS : json string buffer
		   -JS_len: index to put }
** output: None
************************************************************************/
void jW_End_obj(char *JS, int *JS_len)
{
	JS[(*JS_len) - 1] = '}';
	JS[(*JS_len)] = '\0';
	return;
}

/************************************************************************
** function name: jW_Start_array
** function description: start the array. Do nothing
** inputs: -JS : json string buffer
		   -JS_start: 
** output: None
************************************************************************/
void jW_Start_array(char *JS, int *JS_start)
{
	return;
}

/************************************************************************
** function name: jW_End_array
** function description: close the array. Remove last ,
** inputs: -JS : json string buffer
		   -JS_len:
** output: None
************************************************************************/
void jW_End_array(char *JS, int *JS_len)
{
	(*JS_len) = (*JS_len) - 1;
	JS[(*JS_len)] = '\0';
	return;
}

/************************************************************************
** function name: jW_Object_add_to_array
** function description: add the object to an array
** inputs: -key: attribute always string
		   -value: value ofthe object to add and it must be object_string
		   -JS: json string buffer
		   -JS_idx: current index of JS
** output: None
************************************************************************/
void jW_Object_add_to_array(char *value, char *JS, int *JS_idx)
{
	static int idx;
	int value_len = strlen(value);

	for (idx = 0; idx < (value_len); idx++, (*JS_idx)++)
	{
		JS[(*JS_idx)] = value[idx];
	}

	JS[(*JS_idx)++] = ',';
}

/************************************************************************
** function name: jW_String
** function description: add key and value of string token in JS
** inputs: -key: attribute always string
		   -value: value of corresponding key and it must be string
		   -JS: json string buffer
		   -JS_idx: current index of JS
** output: None
************************************************************************/
void jW_String(char *key, char *value, char *JS, int *JS_idx)
{
	int idx = 0;
	int key_len = strlen(key);
	int value_len = strlen(value);

	JS[(*JS_idx)++] = '\"';

	for (idx = 0; idx < (key_len); idx++, (*JS_idx)++)
	{
		JS[(*JS_idx)] = key[idx];
	}
	JS[(*JS_idx)++] = '\"';
	JS[(*JS_idx)++] = ':';

	if (value)
	{
		JS[(*JS_idx)++] = '\"';
		for (idx = 0; idx < (value_len); idx++, (*JS_idx)++)
		{
			JS[(*JS_idx)] = value[idx];
		}
		JS[(*JS_idx)++] = '\"';
	}
	else
	{
		JS[(*JS_idx)++] = 'n';
		JS[(*JS_idx)++] = 'u';
		JS[(*JS_idx)++] = 'l';
		JS[(*JS_idx)++] = 'l';
	}

	JS[(*JS_idx)++] = ',';
}

/************************************************************************
** function name: jW_Object
** function description: add key and value of object token in JS
** inputs: -key: attribute always string
		   -value: value of corresponding key and it must be object_string
		   -JS: json string buffer
		   -JS_idx: current index of JS
** output: None
************************************************************************/
void jW_Object(char *key, char *value, char *JS, int *JS_idx)
{
	static int idx;
	int key_len = strlen(key);
	int value_len = strlen(value);

	JS[(*JS_idx)++] = '\"';

	for (idx = 0; idx < (key_len); idx++, (*JS_idx)++)
	{
		JS[(*JS_idx)] = key[idx];
	}

	JS[(*JS_idx)++] = '\"';
	JS[(*JS_idx)++] = ':';

	for (idx = 0; idx < (value_len); idx++, (*JS_idx)++)
	{
		JS[(*JS_idx)] = value[idx];
	}

	JS[(*JS_idx)++] = ',';
}

/************************************************************************
** function name: jW_Num
** function description: add key and value of number token in JS
** inputs: -key: attribute always string
		   -value: value of corresponding key and it must be number
		   -fI: indicate if fi is equal 1 this float number else fi equal 0 this integer number
		   -JS: json string buffer
		   -JS_idx: current index of JS
		   -
** output: None
************************************************************************/
void jW_Num(char *key, double value, NUMBER_t IntegerOrDouble, char *JS, int *JS_idx)
{
	int idx = 0;
	static char num[128];
	int key_len = strlen(key);

	JS[(*JS_idx)++] = '\"';

	for (idx = 0; idx < (key_len); idx++, (*JS_idx)++)
	{
		JS[(*JS_idx)] = key[idx];
	}
	JS[(*JS_idx)++] = '\"';
	JS[(*JS_idx)++] = ':';

	if (IntegerOrDouble == DOUBLE)
		ftos(num, value, 10); //5 number of characters +1
		// snprintf(num, 128, "%.10f", value);
	else if (IntegerOrDouble == INTEGER)
		itos(num, value);
		// snprintf(num, 128, "%.0f", value);
	else
		return; //Error invalid argument value

	for (idx = 0; num[idx] != 0; (*JS_idx)++)
	{
		JS[(*JS_idx)] = num[idx++];
	}

	JS[(*JS_idx)++] = ',';
}

int getDoubleArrayAsString(double *array, int size, char *string, int stringMaxSize)
{
	int res = 0;
	static char num[64];
	for (int idx = 0; idx < size; idx += 1)
	{
		if (array[idx] == 0.0)
			itos(num, array[idx]);	// if the value is 0 "print" as an integer
		else
			ftos(num, array[idx], 10);
		for (int idx_num = 0; num[idx_num] != 0; res++)
		{
			string[res] = num[idx_num++];
		}
		string[res++] = ',';
	}

	string[res - 1] = '\0';

	// TODO: improve string out of bound handling
	if (res > stringMaxSize)
	{
		log_error("Writing out of string bound\n");
		fprintf(stderr, "Writing out of string bound\n");
	}
	return res;
}

int getIntArrayAsString(int *array, int size, char *string, int stringMaxSize)
{
	int res = 0;
	static char num[64];
	for (int idx = 0; idx < size; idx += 1)
	{
		itos(num, array[idx]);
		for (int idx_num = 0; num[idx_num] != 0; res++)
		{
			string[res] = num[idx_num++];
		}
		string[res++] = ',';
	}

	string[res - 1] = '\0';

	// TODO: improve string out of bound handling
	if (res > stringMaxSize)
	{
		log_error("Writing out of string bound\n");
		fprintf(stderr, "Writing out of string bound\n");
	}
	return res;
}

int getDoubleMatrixAsString(double *matrix, int sizeX, int sizeY, char *string, int stringMaxSize)
{
	int res = 0;
	for (int idx = 0; idx < sizeX; idx += 1)
	{
		string[res++] = '[';
		res += getDoubleArrayAsString(&(matrix[idx * sizeY]), sizeY, &(string[res]), stringMaxSize);
		string[res - 1] = ']';
		string[res++] = ',';
	}

	string[res - 1] = '\0';

	// TODO: improve string out of bound handling
	if (res > stringMaxSize)
	{
		log_error("Writing out of string bound\n");
		fprintf(stderr, "Writing out of string bound\n");
	}
	return res;

}

/************************************************************************
 ** function name: jW_array
 ** function description: add key and value of array token in JS inside sqaure bracketss
 ** inputs: -key: attribute always string
 -value: value of corresponding key and it must be string
 -JS: json string buffer
 -JS_idx: current index of JS
 ** output: None
 ************************************************************************/
void jW_Array(char *key, char *value, char *JS, int *JS_idx)
{
	int idx=0;
	int key_len=strlen(key);
	int value_len=strlen(value);
	
	JS[(*JS_idx)++]='\"';
	
	for(idx=0;idx<(key_len);idx++,(*JS_idx)++)
		JS[(*JS_idx)]=key[idx];

	JS[(*JS_idx)++]='\"';
	JS[(*JS_idx)++]=':';
	JS[(*JS_idx)++] = '[';

	for(idx=0;idx<(value_len);idx++,(*JS_idx)++)
		JS[(*JS_idx)]=value[idx];

	JS[(*JS_idx)++] = ']';
	JS[(*JS_idx)++] = ',';
}

/********************************************************
**function description: convert from floating number to string
**input: pointer to string,floating number , length (number of characters to be printed)
**output: none
note: the length musn't exceede the sizeof(string)
***********************************************************/
void ftos(char *s, double number, int length)
{
	int s_index = 0; // G:start of index of integer part and then it will be the start of index of firiction part
	int index = 0, temp_index = 0;
	long i_part = 0;
	double f_part = 0;
	int icount = 0; // to count digits of integral part even though it's -ve
	int digits_after_point = 0;
	//uint32_t hold_f_part = 0; // to hold firction part as integer part
	/*if number is - ve put sign (-) at the first element in array*/
	if (number < 0)
	{
		s[index++] = '-';
		s_index++;
		number *= -1;
		icount++;
	}
	i_part = (long)number;				  // G: store integer part
	f_part = (number) - ((double)i_part); // store firction part

	if (0 == i_part)
	{
		s[index++] = 48;
		icount++;
	}
	else
	{
		while (i_part > 0)
		{
			s[index++] = (i_part % 10) + 48;
			i_part /= 10;
			icount++;
		}
		reverse(s, index, s_index);
	}
	s[index++] = '.';

	/*convert firction into integer number */
	digits_after_point = length - (icount + 2); // to know how many digits of friction should be stored , +2 because '-' &'\0' count as 2 charachter
	while (digits_after_point)
	{
		f_part *= 10;
		s[index++] = (int)f_part + 48; // store each digit as a chrachter
		f_part -= (int)f_part;		   // contains the rest of friction only
		digits_after_point--;
	}
	s[index++] = '\0';
}

/********************************************************
**function description: convert from integer number to string
**input: pointer to string,integer number
**output: none
***********************************************************/
void itos(char *s, long long int number)
{
	int index = 0, s_index = 0;

	if (!number)
	{
		s[index++] = '0';
		s[index] = 0;
		return;
	}
	if (number < 0)
	{
		s[index++] = '-';
		s_index++;
		number *= -1;
	}
	while (number > 0)
	{
		s[index++] = (number % 10) + 48;
		number /= 10;
	}
	if (index != 1) // to not call fn if number is only one digit
		reverse(s, index, s_index);
	s[index] = 0;
}
