#include <texture/vsx_texture.h>

class module_bitmap_load_tga: public vsx_module
{
  // in
  vsx_module_param_resource* filename_in;
  vsx_module_param_int* reload_in;
  vsx_module_param_int* flip_vertical_in;

  // out
  vsx_module_param_bitmap* bitmap_out;

  // internal
  vsx_bitmap* bitmap;

  vsx_string<> current_filename;
  int flip_vertical_cache = 0;

public:

  module_bitmap_load_tga()
  {
    loading_done = false;
  }

  void module_info(vsx_module_info* info)
  {
    info->identifier =
      "bitmaps;loaders;tga_bitm_load";

    info->description =
      "Loads a TGA image from\n"
      "disk and outputs a:\n"
      " - bitmap \n "
    ;

    info->in_param_spec =
      "filename:resource,"
      "reload:enum?no|yes,"
      "flip_vertical:enum?no|yes"
    ;

    info->out_param_spec =
      "bitmap:bitmap";

    info->component_class =
      "bitmap";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    filename_in = (vsx_module_param_resource*)in_parameters.create(VSX_MODULE_PARAM_ID_RESOURCE,"filename");
    filename_in->set("");
    current_filename = "";

    reload_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "reload");
    flip_vertical_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "flip_vertical");

    // out
    bitmap_out = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP, "bitmap");
  }


  void run()
  {
    if (bitmap->data_ready)
    {
      bitmap->timestamp++;

      bitmap_out->set(bitmap);
      loading_done = true;
      message = "module||ok";
      bitmap->data_ready = 0;
    }

    bool reload = false;

    if (current_filename != filename_in->get())
      reload = true;

    if (reload_in->get())
      reload = true;

    if (flip_vertical_in->get() != flip_vertical_cache)
      reload = true;

    if (!reload)
      return;

    flip_vertical_cache = flip_vertical_in->get();

    reload_in->set(0);

    if (!verify_filesuffix(filename_in->get(),"tga"))
    {
      filename_in->set(current_filename);
      message = "module||ERROR! This is not a TGA image file!";
      return;
    }

    current_filename = filename_in->get();

    vsx_bitmap_loader_hint data_hint(
      flip_vertical_cache,
      false
    );

    if (bitmap)
      vsx_bitmap_cache::get_instance()->destroy(bitmap);

    if (!bitmap)
      bitmap = vsx_bitmap_cache::get_instance()->
        aquire_create( current_filename, data_hint );

    vsx_bitmap_loader_helper::load(bitmap, current_filename, engine->filesystem, true, data_hint );
  }

  void output(vsx_module_param_abs* param)
  {
  }

  void stop() {
  }

  void start() {
  }

  void on_delete()
  {
    if (bitmap)
      vsx_bitmap_cache::get_instance()->destroy(bitmap);
  }

};


