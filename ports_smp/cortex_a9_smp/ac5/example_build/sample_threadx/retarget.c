// Copyright ARM Ltd 2009. All rights reserved.

extern void $Super$$main(void);
extern void enable_caches(void);

void $Sub$$main(void)
{
  enable_caches(); // enables caches
  $Super$$main();  // calls original main()
}

