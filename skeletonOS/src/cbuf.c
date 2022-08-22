#include <string.h>

#include "cbuf.h"

int __circ_gbuf_pop(circ_gbuf_t *circ_buf, void *elem, uint16_t read_only)
{
	int32_t total;
	char *tail;

	total = circ_buf->push_count - circ_buf->pop_count;
	if (total < 0)
		total += (2 * circ_buf->size);

	if (total == 0)
		return -1; // Empty

	tail = (char *)circ_buf->buffer + ((circ_buf->pop_count % circ_buf->size) * circ_buf->element_size);

	if (elem)
		memcpy(elem, tail, circ_buf->element_size);

	if (!read_only)
	{
		circ_buf->pop_count++;
		if (circ_buf->pop_count >= (2 * circ_buf->size))
			circ_buf->pop_count = 0;
	}
	
	return 0;
}

int __circ_gbuf_push(circ_gbuf_t *circ_buf, void *elem)
{
	int32_t total;
	char *head;

	total = circ_buf->push_count - circ_buf->pop_count;
	if (total < 0)
		total += (2 * circ_buf->size);

	if (total >= circ_buf->size)
		return -1; // Full

	head = (char *)circ_buf->buffer + ((circ_buf->push_count % circ_buf->size) * circ_buf->element_size);
	memcpy(head, elem, circ_buf->element_size);
	circ_buf->push_count++;

	if (circ_buf->push_count >= (2 * circ_buf->size))
		circ_buf->push_count = 0;

	return 0;
}

int __circ_gbuf_free_space(circ_gbuf_t *circ_buf)
{
	int32_t total;

	total = circ_buf->push_count - circ_buf->pop_count;
	if (total < 0)
		total += (2 * circ_buf->size);

	return circ_buf->size - total;
}

