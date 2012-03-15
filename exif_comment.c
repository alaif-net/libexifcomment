#include <stdio.h>
#include <string.h>
#include <libexif/exif-data.h>

/* raw EXIF header data */
static const unsigned char exif_header[] = {
  0xff, 0xd8, 0xff, 0xe1
};
/* length of data in exif_header */
static const unsigned int exif_header_len = sizeof(exif_header);

int get_user_comment(char *infilename, char *outcomment, int incommentlen)
{
	ExifData *ed;
	ExifEntry *entry;

	ed = exif_data_new_from_file(infilename);
	if (!ed) {
		return -1;	
	}

	entry = exif_content_get_entry(ed->ifd[EXIF_IFD_EXIF], EXIF_TAG_USER_COMMENT);
	if (!entry) {
		return -2;
	}

	exif_entry_get_value(entry, outcomment, incommentlen);

	printf("IFD1 priv=%08x\n", (int)ed->ifd[EXIF_IFD_1]->priv);	

	return 0;
}

int put_user_comment(char *infilename, char *incomment, int incommentlen, char *outfilename)
{

	ExifData *ed;
	ExifEntry *entry;

	ed = exif_data_new_from_file(infilename);
	if (!ed) {
		ed = exif_data_new();	
		exif_data_set_option(ed, EXIF_DATA_OPTION_FOLLOW_SPECIFICATION);
		exif_data_set_data_type(ed, EXIF_DATA_TYPE_COMPRESSED);
		exif_data_set_byte_order(ed, EXIF_BYTE_ORDER_INTEL);
	}
	exif_data_fix(ed);

	entry = exif_content_get_entry(ed->ifd[EXIF_IFD_EXIF], EXIF_TAG_USER_COMMENT);
	if (entry) {
		puts("Remove USER_COMMENT entry.");
		exif_content_remove_entry(ed->ifd[EXIF_IFD_EXIF], entry);
	}

	ExifMem *mem = exif_mem_new_default();
	entry = exif_entry_new_mem(mem);
	void *buf = exif_mem_alloc(mem, incommentlen + 8);
	entry->data = buf;
	entry->size = incommentlen + 8;
	entry->tag = EXIF_TAG_USER_COMMENT;
	entry->components = incommentlen + 8;
	entry->format = EXIF_FORMAT_UNDEFINED;
	exif_content_add_entry(ed->ifd[EXIF_IFD_EXIF], entry);

	exif_mem_unref(mem);
	exif_entry_unref(entry);

	memcpy(entry->data, "\0\0\0\0\0\0\0\0", 8);
	memcpy(entry->data+8, incomment, incommentlen);

	unsigned char *exif_data;
	unsigned int exif_data_len;
	exif_data_save_data(ed, &exif_data, &exif_data_len);

	FILE *write_f;
	write_f = fopen(outfilename, "wb");
	if (write_f == NULL) {
		return -2;
	}

	if (fwrite(exif_header, exif_header_len, 1, write_f) != 1) {
		fclose(write_f);
		return -3;
	}
	/* Write EXIF block length in big-endian order */
	if (fputc((exif_data_len + 2) >> 8, write_f) < 0) {
		fclose(write_f);
		return -4;
	}
	if (fputc((exif_data_len + 2) & 0xff, write_f) < 0) {
		fclose(write_f);
		return -5;
	}
	/* Write EXIF data block */
	if (fwrite(exif_data, exif_data_len, 1, write_f) != 1) {
		fclose(write_f);
		return -6;
	}

	//Read jpeg data.
	FILE *read_f;
	size_t readlen;
	unsigned char jpgbuff[1024];
	read_f = fopen(infilename, "rb");
	if (read_f == NULL) {
		fclose(write_f);
		return -7;
	}

	fseek(read_f, 4L, SEEK_SET);
	
	int jpegdata_offset = (fgetc(read_f) << 8) + fgetc(read_f) + 4;
	printf("offset=%d\n", jpegdata_offset);
	fseek(read_f, jpegdata_offset, SEEK_SET);
	
	while (feof(read_f) == 0) {
		readlen = fread(jpgbuff, 1, 1024, read_f);
		printf("copy: read %d byte\n", readlen);
		if (fwrite(jpgbuff, readlen, 1, write_f) != 1) {
			puts("copy: write failed");
		}
	}
	fclose(read_f);

	fclose(write_f);
	return 0;
}
