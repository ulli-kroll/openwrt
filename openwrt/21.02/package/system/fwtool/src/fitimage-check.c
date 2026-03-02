#include <linux/types.h>
#include <stdio.h>
#include <linux/string.h>
#include <asm/byteorder.h>
#include <endian.h>
#include "fitimage.h"

static inline const void *_fdt_offset_ptr(const void *fdt, int offset)
{
	return (const char *)fdt + fdt_off_dt_struct(fdt) + offset;
}

const void *fdt_offset_ptr(const void *fdt, int offset, unsigned int len)
{
	const char *p;

	if (fdt_version(fdt) >= 0x11)
		if (((offset + len) < offset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			return NULL;

	p = _fdt_offset_ptr(fdt, offset);

	if (p + len < p)
		return NULL;
	return p;
}

unsigned int fdt_next_tag(const void *fdt, int startoffset, int *nextoffset)
{
	const fdt32_t *tagp, *lenp;
	unsigned int tag;
	int offset = startoffset;
	const char *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	if (!tagp)
		return FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	switch (tag) {
	case FDT_BEGIN_NODE:
		/* skip name */
		do {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} while (p && (*p != '\0'));
		if (!p)
			return FDT_END; /* premature end */
		break;

	case FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, sizeof(*lenp));
		if (!lenp)
			return FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += sizeof(struct fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		break;

	case FDT_END:
	case FDT_END_NODE:
	case FDT_NOP:
		break;

	default:
		return FDT_END;
	}

	if (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		return FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);

	return tag;
}

int _fdt_check_node_offset(const void *fdt, int offset)
{
	if ((offset < 0) || (offset % FDT_TAGSIZE)
	    || (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_NODE))
		return -FDT_ERR_BADOFFSET;

	return offset;
}

int _fdt_check_prop_offset(const void *fdt, int offset)
{
	if ((offset < 0) || (offset % FDT_TAGSIZE)
	    || (fdt_next_tag(fdt, offset, &offset) != FDT_PROP))
		return -FDT_ERR_BADOFFSET;

	return offset;
}

int fdt_next_node(const void *fdt, int offset, int *depth)
{
	int nextoffset = 0;
	unsigned int tag;

	if (offset >= 0)
		if ((nextoffset = _fdt_check_node_offset(fdt, offset)) < 0)
			return nextoffset;

	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		case FDT_PROP:
		case FDT_NOP:
			break;

		case FDT_BEGIN_NODE:
			if (depth)
				(*depth)++;
			break;

		case FDT_END_NODE:
			if (depth && ((--(*depth)) < 0))
				return nextoffset;
			break;

		case FDT_END:
			if ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				return -FDT_ERR_NOTFOUND;
			else
				return nextoffset;
		}
	} while (tag != FDT_BEGIN_NODE);

	return offset;
}

static int _nextprop(const void *fdt, int offset)
{
	unsigned int tag;
	int nextoffset;

	do {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		case FDT_END:
			if (nextoffset >= 0)
				return -FDT_ERR_BADSTRUCTURE;
			else
				return nextoffset;

		case FDT_PROP:
			return offset;
		}
		offset = nextoffset;
	} while (tag == FDT_NOP);

	return -FDT_ERR_NOTFOUND;
}


int fdt_next_property_offset(const void *fdt, int offset)
{
	if ((offset = _fdt_check_prop_offset(fdt, offset)) < 0)
		return offset;

	return _nextprop(fdt, offset);
}

int fdt_first_property_offset(const void *fdt, int nodeoffset)
{
	int offset;

	if ((offset = _fdt_check_node_offset(fdt, nodeoffset)) < 0)
		return offset;

	return _nextprop(fdt, offset);
}

const char *fdt_string(const void *fdt, int stroffset)
{
	return (const char *)fdt + fdt_off_dt_strings(fdt) + stroffset;
}


static int _fdt_string_eq(const void *fdt, int stroffset,
			  const char *s, int len)
{
	const char *p = fdt_string(fdt, stroffset);

	return (strnlen(p, len + 1) == len) && (memcmp(p, s, len) == 0);
}

const struct fdt_property *fdt_get_property_by_offset(const void *fdt,
						      int offset,
						      int *lenp)
{
	int err;
	const struct fdt_property *prop;

	if ((err = _fdt_check_prop_offset(fdt, offset)) < 0) {
		if (lenp)
			*lenp = err;
		return NULL;
	}
	
	prop = _fdt_offset_ptr(fdt, offset);

	if (lenp)
		*lenp = fdt32_to_cpu(prop->len);

	return prop;
}


const struct fdt_property *fdt_get_property_namelen(const void *fdt,
						    int offset,
						    const char *name,
						    int namelen, int *lenp)
{	
	for (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(fdt, offset))) {
		const struct fdt_property *prop;

		if (!(prop = fdt_get_property_by_offset(fdt, offset, lenp))) {
			offset = -FDT_ERR_INTERNAL;
			break;
		}

		if (_fdt_string_eq(fdt, fdt32_to_cpu(prop->nameoff),
				   name, namelen))
			return prop;
	}

	if (lenp)
		*lenp = offset;
	return NULL;
}


const void *fdt_getprop_namelen(const void *fdt, int nodeoffset,
				const char *name, int namelen, int *lenp)
{
	const struct fdt_property *prop;

	prop = fdt_get_property_namelen(fdt, nodeoffset, name, namelen, lenp);
	if (! prop)
		return NULL;

	return prop->data;
}


const void *fdt_getprop(const void *fdt, int nodeoffset,
			const char *name, int *lenp)
{
	return fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

/**
 * fit_get_end - get FIT image size
 * @fit: pointer to the FIT format image header
 *
 * returns:
 *     size of the FIT image (blob) in memory
 */
unsigned int fit_get_size(const void *fit)
{
	return fdt_totalsize(fit);
}

/**
 * fit_get_desc - get node description property
 * @fit: pointer to the FIT format image header
 * @noffset: node offset
 * @desc: double pointer to the char, will hold pointer to the descrption
 *
 * fit_get_desc() reads description property from a given node, if
 * description is found pointer to it is returened in third call argument.
 *
 * returns:
 *     0, on success
 *     -1, on failure
 */
int fit_get_desc(const void *fit, int noffset, char **desc)
{
	int len;

	*desc = (char *)fdt_getprop(fit, noffset, FIT_DESC_PROP, &len);
	if (*desc == NULL) {
		printf("Can't get '%s' property from FIT\r\n", FIT_DESC_PROP);
		return -1;
	}

	return 0;
}

int fdt_check_header(const void *fdt)
{
	if (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		if (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
			return -FDT_ERR_BADVERSION;
		if (fdt_last_comp_version(fdt) > FDT_LAST_SUPPORTED_VERSION)
			return -FDT_ERR_BADVERSION;
	} else if (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		if (fdt_size_dt_struct(fdt) == 0)
			return -FDT_ERR_BADSTATE;
	} else {
		return -FDT_ERR_BADMAGIC;
	}

	if (fdt_off_dt_struct(fdt) > (UINT_MAX - fdt_size_dt_struct(fdt)))
		return FDT_ERR_BADOFFSET;

	if (fdt_off_dt_strings(fdt) > (UINT_MAX -  fdt_size_dt_strings(fdt)))
		return FDT_ERR_BADOFFSET;

	if ((fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt)) > fdt_totalsize(fdt))
		return FDT_ERR_BADOFFSET;

	if ((fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt)) > fdt_totalsize(fdt))
		return FDT_ERR_BADOFFSET;

	return 0;
}


