-c
-heap  0x400
-stack 0x1000
-l C:\ti\pdk_C6678_1_1_2_6\packages\ti\csl\lib\ti.csl.ae66
-l C:\ti\pdk_C6678_1_1_2_6\packages\ti\csl\lib\ti.csl.intc.ae66
-l c:\ti\pdk_C6678_1_1_2_6\packages\ti\platform\evmc6678l\platform_lib\lib\release\ti.platform.evm6678l.ae66
 
/* Memory Map */
MEMORY
{
    L1PSRAM (RWX)  : org = 0x00E00000,  len = 0x00008000
    L1DSRAM (RWX)  : org = 0x00F00000,  len = 0x00008000 
    CODE_RAM (RWX) : org = 0x00800000,  len = 0x00020000
    DATA_RAM (RWX) : org = 0x00820000,  len = 0x00060000
    MSMCSRAM (RWX) : org = 0x0c000000,  len = 0x00400000
    DDR3 (RWX)     : org = 0x80000000,  len = 0x80000000
}
 
SECTIONS
{
	.text          >  CODE_RAM
	.stack         >  CODE_RAM
	.cio           >  CODE_RAM
	.const         >  CODE_RAM
	.data          >  CODE_RAM
	.switch        >  CODE_RAM
	.sysmem        >  CODE_RAM
	.far           >  CODE_RAM
	.args          >  CODE_RAM
	.ppinfo        >  CODE_RAM
	.ppdata        >  CODE_RAM
	.csl_vect      >  CODE_RAM
    platform_lib   >  CODE_RAM 

	GROUP
	{
		.neardata
		.rodata
		.bss
	} > CODE_RAM

	/* COFF sections */
	.pinit         >  CODE_RAM
	.cinit         >  CODE_RAM
  
	/* EABI sections */
	.binit         >  CODE_RAM
	.init_array    >  CODE_RAM
	.fardata       >  CODE_RAM
	.c6xabi.exidx  >  CODE_RAM
	.c6xabi.extab  >  CODE_RAM

	/* ThreadX section which should be the last RAM section loaded */
	.zend          >  DATA_RAM
}
