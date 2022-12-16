#ifndef __REGDEF_H
#define __REGDEF_H

#define zero    $0      /* wired zero */
//#define AT      $1      /* assembler temp  - uppercase because of ".set at" */
#define v0      $2      /* values for function returns */
#define v1      $3
#define a0      $4      /* argument registers */
#define a1      $5
#define a2      $6
#define a3      $7
#define t0      $8      
#define t1      $9
#define t2      $10
#define t3      $11
#define t4      $12
#define t5      $13
#define t6      $14
#define t7      $15
#define s0      $16     
#define s1      $17
#define s2      $18
#define s3      $19
#define s4      $20
#define s5      $21
#define s6      $22
#define s7      $23
#define t8      $24     
#define t9      $25
#define jp      $25     
#define k0      $26     /* reserved for OS */
#define k1      $27     /* reserved for OS */
#define gp      $28     /* global pointer */
#define sp      $29     /* stack pointer */
#define fp      $30     /* frame pointer */
#define s8      $30     /* frame pointer */
#define ra      $31     /* return address */

#endif /* __REGDEF_H */
