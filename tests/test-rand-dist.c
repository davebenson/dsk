

static struct {
  const char *rand_algo;
  DskRand *(*create)();
} rand_algo_table[] = {
  { "mersenne-twister", create_mersenne_twister },
  { "mt", create_mersenne_twister },
};

static DskRand *
create_rand (const char *name, const char *seed)
{
  DskRand *rv = NULL;
  size_t rand_algo_i = 0;
  while (rand_algo_i < DSK_N_ELEMENTS (rand_algo_table) && rv == NULL)
    {
      if (strcmp (rand_algo_table[rand_algo_i].rand_algo, name) == 0)
        rv = rand_algo_table[rand_algo_i].create();
      rand_algo_i++;
    }
  if (rv == NULL)
    return NULL;

  ... parse seed + seed rv

  return rv;
}

/* --- Main Program --- */
static size_t n_counts;
static uint64_t *counts;

static const char *rand_algo_str;
static const char *rand_seed_str;

int main(int argc, char **argv)
{
  DskRand *rng = create_rand (rand_algo_str, rand_seed_str);
  dsk_assert (rng != NULL);
}
