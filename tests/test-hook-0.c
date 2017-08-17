#include "../dsk.h"
#include <string.h>
#include <stdio.h>

typedef struct TestHook_Object0Class TestHook_Object0Class;
struct TestHook_Object0Class {
  DskObjectClass base_class;
};
typedef struct TestHook_Object0 TestHook_Object0;
struct TestHook_Object0 {
  DskObject base_instance;
  DskHook simple_hook;
  unsigned notify_count;
};

static void
test_hook_object0_init (TestHook_Object0 *o)
{
  dsk_hook_init_zeroed(&o->simple_hook, o);
}

static void
test_hook_object0_finalize (TestHook_Object0 *o)
{
  dsk_hook_clear (&o->simple_hook);
}


DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(TestHook_Object0);
TestHook_Object0Class test_hook_object0_class = {
  DSK_OBJECT_CLASS_DEFINE(TestHook_Object0, &dsk_object_class,
                          test_hook_object0_init,
                          test_hook_object0_finalize)
};


static dsk_boolean cmdline_verbose = DSK_FALSE;

typedef struct HandlerData HandlerData;
struct HandlerData {
  unsigned notify_count;
  dsk_boolean destroyed;
};
#define HANDLER_DATA_INIT {0,DSK_FALSE}

static dsk_boolean handle_notify (void *obj, void *hd)
{
  dsk_assert (dsk_object_is_a (obj, &test_hook_object0_class));
  HandlerData *data = hd;
  TestHook_Object0 *o = obj;
  data->notify_count += 1;
  o->notify_count += 1;
  return DSK_TRUE;
}
static void handle_destroy (void *hd)
{
  HandlerData *data = hd;
  dsk_assert (!data->destroyed);
  data->destroyed = DSK_TRUE;
}

static void test_hook_simple (void)
{
  TestHook_Object0 *o;
  HandlerData handler_data = HANDLER_DATA_INIT;
  o = dsk_object_new (&test_hook_object0_class);
  dsk_hook_trap (&o->simple_hook, handle_notify, &handler_data, handle_destroy);
  dsk_assert (o->notify_count == 0);
  dsk_assert (handler_data.notify_count == 0);
  dsk_hook_notify (&o->simple_hook);
  dsk_assert (o->notify_count == 1);
  dsk_assert (handler_data.notify_count == 1);
  dsk_object_unref (o);
  dsk_assert (handler_data.notify_count == 1);
  dsk_assert (handler_data.destroyed);
}



static struct 
{
  const char *name;
  void (*test)(void);
} tests[] =
{
  { "trivial hook tests", test_hook_simple },
};
int main(int argc, char **argv)
{
  unsigned i;
  dsk_cmdline_init ("test hooks",
                    "Test hook system",
                    NULL, 0);
  dsk_cmdline_add_boolean ("verbose", "extra logging", NULL, 0,
                           &cmdline_verbose);
  dsk_cmdline_process_args (&argc, &argv);

  for (i = 0; i < DSK_N_ELEMENTS (tests); i++)
    {
      fprintf (stderr, "Test: %s... ", tests[i].name);
      tests[i].test ();
      fprintf (stderr, " done.\n");
    }
  dsk_cleanup ();
  return 0;
}
