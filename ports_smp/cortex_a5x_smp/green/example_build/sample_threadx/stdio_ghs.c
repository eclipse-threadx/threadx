#include <stdint.h>

/*
 * Zynq Ultrascale+ MPSoC / GHS
 *
 * Implements standard I/O through UART
 *
 * XXX assume the UART has already been initialized by bootloader
 */
#define UART_BASE         0xff000000U   /* UART0 */

#define UART_FIFO         *((volatile uint32_t *)(UART_BASE+0x0030U))
#define UART_SR           *((volatile uint32_t *)(UART_BASE+0x002CU))
#define UART_SR_TXFULL    0x00000010U


long write(int fno, const void *buf, long size)
{
  if (fno != 1) return -1;

  const char *p = buf;
  const char *pmax = p + size;
  while (p < pmax) {
    char c = *p++;
    if (c == '\n') {
      /* expand LF to CR+LF */
      while ((UART_SR & UART_SR_TXFULL) != 0);
      UART_FIFO = '\r';
    }
    while ((UART_SR & UART_SR_TXFULL) != 0);
    UART_FIFO = c;
  }

  return size;
}
