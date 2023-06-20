int mod_main(int *base, int *bytta);

extern "C" int mod_start(int *base, int* bytta) {
  return mod_main(base, bytta);
}
