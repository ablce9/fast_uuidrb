#include <ruby.h>
#include <ruby/io.h>
#include <ruby/st.h>

#include <stdio.h>

#define FAST_UUID_VERSION "0.0.0"

static VALUE FastUUID;

#define UUID_VERSION_4 0x4

#define UUID_SIZE 36

#define UUID_VERSION_INDEX 12

#define UUID_FIRST_SEGMENT_INDEX_END  8
#define UUID_SECOND_SEGMENT_INDEX_END  12
#define UUID_THIRD_SEGMENT_INDEX_END  16
#define UUID_FOURTH_SEGMENT_SEGMENT_INDEX_END 20
#define UUID_FIFTH_SEGMENT_INDEX_END  32

typedef struct {
    int current_segment_number;
    char *uuid;
    int pos;
    uint8_t version;
    uint8_t variant;
} fast_uuid_t;

static fast_uuid_t *fs_check_segment(fast_uuid_t *fast_uuid) {

    char *uuid = fast_uuid->uuid, *p, *segment, ch;

    switch (fast_uuid->current_segment_number) {

    case 0:

	for (p = uuid; p < uuid + UUID_SIZE; p++) {

	    ch = *p;

	    if (ch == '-') {
		rb_raise(rb_eTypeError, "Invalid first segment");
	    }

	    ++fast_uuid->pos;

	    if (fast_uuid->pos == UUID_FIRST_SEGMENT_INDEX_END) {
		break;
	    }
	}

	fast_uuid->current_segment_number = 1;
	break;

    case 1:
	segment = uuid + UUID_FIRST_SEGMENT_INDEX_END + 1;

	for (p = segment; p < segment + UUID_SIZE; p++) {

	    ch = *p;

	    if (ch == '-') {
		rb_raise(rb_eTypeError, "Invalid second segment");
	    }

	    ++fast_uuid->pos;

	    if (fast_uuid->pos == UUID_SECOND_SEGMENT_INDEX_END) {
		break;
	    }

	}

	fast_uuid->current_segment_number = 2;
	break;

    case 2:
	segment = uuid + UUID_SECOND_SEGMENT_INDEX_END + 2;

	for (p = segment; p < segment + UUID_SIZE; p++) {

	    ch = *p;

	    if (ch == '-') {
		rb_raise(rb_eTypeError, "Invalid third segment");
	    }

	    // Check version number
	    if (fast_uuid->pos == UUID_VERSION_INDEX) {
		switch (ch) {

		case '4':
		    fast_uuid->version = UUID_VERSION_4;
		    break;

		default:
		    rb_raise(rb_eTypeError, "Unsupported uuid version");
		    break;
		}
	    }

	    ++fast_uuid->pos;

	    if (fast_uuid->pos == UUID_THIRD_SEGMENT_INDEX_END) {
		break;
	    }

	}

	fast_uuid->current_segment_number = 3;
	break;

    case 3:
	segment = uuid + UUID_THIRD_SEGMENT_INDEX_END + 3;

	for (p = segment; p < segment + UUID_SIZE; p++) {

	    ch = *p;

	    if (ch == '-') {
		rb_raise(rb_eTypeError, "Invalid fourth segment");
	    }

	    // TODO: check uuid variant
	    if (fast_uuid->pos == 16) {
		fast_uuid->variant = ch;
	    }

	    ++fast_uuid->pos;

	    if (fast_uuid->pos == UUID_FOURTH_SEGMENT_SEGMENT_INDEX_END) {
		break;
	    }
	}

	fast_uuid->current_segment_number = 4;
	break;

    case 4:
	segment = uuid + UUID_FOURTH_SEGMENT_SEGMENT_INDEX_END + 4;

	for (p = segment; p < segment + UUID_SIZE; p++) {

	    ch = *p;

	    if (ch == '-') {
		rb_raise(rb_eTypeError, "Invalid fourth segment");
	    }

	    ++fast_uuid->pos;

	    if (fast_uuid->pos == UUID_FIFTH_SEGMENT_INDEX_END) {
		break;
	    }
	}

	break;
    }

    return fast_uuid;
}

static VALUE fu_check_uuid(VALUE klass, VALUE value) {
    if (TYPE(value) != T_STRING) {
	rb_raise(rb_eTypeError, "Invalid type for uuid check");
    }

    fast_uuid_t fast_uuid = {
	.current_segment_number = 0,
	.uuid = RSTRING_PTR(value)
    };
    char *p, ch;

    if (strlen(RSTRING_PTR(value)) != UUID_SIZE) {
	rb_raise(rb_eTypeError, "Invalid length for uuid");
    }

    for (p = RSTRING_PTR(value); p < RSTRING_PTR(value) + UUID_SIZE; p++) {

	ch = *p;

	/*
	  segments info
	  xxxxxxxx-xxxx-Mxxx-Nxxx-xxxxxxxxxxxx
	*/

	if ((ch != '-') && ((!(ch >= 'a' && ch <= 'f')) && (!(ch >= 'A' && ch <= 'F')) && (!(ch >= '0' && ch <= '9')))) {
	    rb_raise(rb_eTypeError, "Invalid character for uuid");
	}

	if (ch == '-') {
	    fs_check_segment(&fast_uuid);
	} else if (fast_uuid.current_segment_number == 4) {
	    fs_check_segment(&fast_uuid);
	    break;
	}
    }

    printf("[debug] version: %i\n", fast_uuid.version);
    return true;
}

void Init_fast_uuid(void) {
    FastUUID = rb_define_module("FastUUID");
    rb_define_const(FastUUID, "VERSION", rb_str_new2(FAST_UUID_VERSION));
    rb_define_singleton_method(FastUUID, "uuid?", fu_check_uuid, 1);
}
