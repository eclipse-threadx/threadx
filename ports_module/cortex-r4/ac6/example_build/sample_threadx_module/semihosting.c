/* Disable semihosting */
#include <rt_sys.h>

#if (__ARMCC_VERSION < 6000000)
#pragma import(__use_no_semihosting)
#else
void use_no_semihosting(void)
{
	__asm(".global __use_no_semihosting\n\t");
}
#endif
 
char *$Sub$$_sys_command_string(char *cmd, int len)
{
	return 0;
}
 
__attribute__((noreturn)) void $Sub$$_sys_exit(int return_code)
{
	while(1);
}
 
__attribute__((noreturn)) int $Sub$$__raise(int signal, int type)
{
	while(1);
}

