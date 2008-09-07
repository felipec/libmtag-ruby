#include "ruby.h"

#include <mtag.h>

#include <stdbool.h>

static VALUE rb_mMTag;
static VALUE rb_cMTag_Tag;
static VALUE rb_cMTag_File;

struct file_data
{
    VALUE tag;
    MTag_File *c_file;
};

struct tag_data
{
    VALUE file;
    MTag_Tag *c_tag;
};

static VALUE
rb_file_alloc (VALUE self)
{
    struct file_data *data;

    return Data_Make_Struct (self, struct file_data, NULL, free, data);
}

static VALUE
rb_file_initialize (VALUE self,
		    VALUE file_name)
{
    struct file_data *data;
    char *c_file_name;

    Data_Get_Struct (self, struct file_data, data);

    c_file_name = rb_string_value_ptr (&file_name);

    data->c_file = mtag_file_new (c_file_name);

    if (!data->c_file)
    {
	rb_raise (rb_eArgError, "Bad file '%s'", c_file_name);
    }

    return Qnil;
}

static VALUE
rb_file_finalize (VALUE self)
{
    struct file_data *data;

    Data_Get_Struct (self, struct file_data, data);

    mtag_file_free (data->c_file);

    return Qnil;
}

static VALUE
rb_file_tag (VALUE self)
{
    struct file_data *data;

    Data_Get_Struct (self, struct file_data, data);

    if (!data->tag)
    {
	VALUE args[1];
	args[0] = self;
	data->tag = rb_class_new_instance (1, args, rb_cMTag_Tag);
	/* @todo unref */
    }

    return data->tag;
}

static VALUE
rb_file_get_info (VALUE self,
		  VALUE tag_id)
{
    struct file_data *data;

    Data_Get_Struct (self, struct file_data, data);

    return rb_str_new2 (mtag_file_get_type (data->c_file));
}

static VALUE
rb_file_strip (VALUE self,
	       VALUE tag_id)
{
    struct file_data *data;
    const char *c_tag_id;

    Data_Get_Struct (self, struct file_data, data);

    c_tag_id = rb_string_value_ptr (&tag_id);
    mtag_file_strip_tag (data->c_file, c_tag_id);

    return Qnil;
}

static VALUE
rb_file_save (VALUE self)
{
    struct file_data *data;

    Data_Get_Struct (self, struct file_data, data);

    mtag_file_save (data->c_file);

    return Qnil;
}

static VALUE
rb_tag_alloc (VALUE self)
{
    struct tag_data *data;

    return Data_Make_Struct (self, struct tag_data, NULL, free, data);
}

static VALUE
rb_tag_initialize (VALUE self,
		   VALUE file)
{
    struct tag_data *data;
    struct file_data *file_data;

    Data_Get_Struct (self, struct tag_data, data);

    data->file = file;

    Data_Get_Struct (file, struct file_data, file_data);

    data->c_tag = mtag_file_tag (file_data->c_file);

    return Qnil;
}

static VALUE
rb_tag_get (VALUE self,
	    VALUE key)
{
    struct tag_data *data;
    char *c_key;

    Data_Get_Struct (self, struct tag_data, data);

    c_key = rb_string_value_ptr (&key);

    return rb_str_new2 (mtag_tag_get (data->c_tag, c_key));
}

static VALUE
rb_tag_set (VALUE self,
	    VALUE key,
	    VALUE value)
{
    struct tag_data *data;
    char *c_key;
    char *c_value;

    Data_Get_Struct (self, struct tag_data, data);

    c_key = rb_string_value_ptr (&key);
    c_value = rb_string_value_ptr (&value);

    mtag_tag_set (data->c_tag, c_key, c_value);

    return Qnil;
}

static VALUE
rb_tag_artist (VALUE self)
{
    struct tag_data *data;

    Data_Get_Struct (self, struct tag_data, data);

    return rb_str_new2 (mtag_tag_get (data->c_tag, "artist"));
}

static VALUE
rb_tag_title (VALUE self)
{
    struct tag_data *data;

    Data_Get_Struct (self, struct tag_data, data);

    return rb_str_new2 (mtag_tag_get (data->c_tag, "title"));
}

static VALUE
rb_tag_set_title (VALUE self,
		  VALUE title)
{
    struct tag_data *data;
    char *c_title;

    Data_Get_Struct (self, struct tag_data, data);

    c_title = rb_string_value_ptr (&title);

    mtag_tag_set (data->c_tag, "title", c_title);

    return Qnil;
}

static VALUE
rb_tag_set_artist (VALUE self,
		   VALUE artist)
{
    struct tag_data *data;
    char *c_artist;

    Data_Get_Struct (self, struct tag_data, data);

    c_artist = rb_string_value_ptr (&artist);

    mtag_tag_set (data->c_tag, "artist", c_artist);

    return Qnil;
}

void
Init_mtag ()
{
    rb_mMTag = rb_define_module ("MTag");
    rb_cMTag_File = rb_define_class_under (rb_mMTag, "File", rb_cObject);
    rb_cMTag_Tag = rb_define_class_under (rb_mMTag, "Tag", rb_cObject);

    rb_define_alloc_func (rb_cMTag_File, rb_file_alloc);
    rb_define_method (rb_cMTag_File, "initialize", rb_file_initialize, 1);
    rb_define_method (rb_cMTag_File, "finalize", rb_file_finalize, 0);
    rb_define_method (rb_cMTag_File, "save", rb_file_save, 0);
    rb_define_method (rb_cMTag_File, "tag", rb_file_tag, 0);
    rb_define_method (rb_cMTag_File, "get_info", rb_file_get_info, 0);
    rb_define_method (rb_cMTag_File, "strip", rb_file_strip, 1);

    rb_define_alloc_func (rb_cMTag_Tag, rb_tag_alloc);
    rb_define_method (rb_cMTag_Tag, "initialize", rb_tag_initialize, 1);
    rb_define_method (rb_cMTag_Tag, "get", rb_tag_get, 1);
    rb_define_method (rb_cMTag_Tag, "set", rb_tag_set, 2);
    rb_define_method (rb_cMTag_Tag, "artist", rb_tag_artist, 0);
    rb_define_method (rb_cMTag_Tag, "title", rb_tag_title, 0);
    rb_define_method (rb_cMTag_Tag, "artist=", rb_tag_set_artist, 1);
    rb_define_method (rb_cMTag_Tag, "title=", rb_tag_set_title, 1);
}
