#ifndef UUID4_H
#define UUID4_H

#define UUID4_VERSION "1.0.0"
#define UUID4_LEN 37

extern int uuid4_init();
void uuid4_generate(char *dst);

#define UUID4_INIT() \
	uuid4_init()

#define UUID4_GEN(DEST) \
	uuid4_generate(DEST)

#endif