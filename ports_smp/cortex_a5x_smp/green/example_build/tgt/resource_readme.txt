Project File (resource.gpj)
------------------------------------------------------------------------------
The files provided in this project are resources to help get your project
running on your target. You may use these files as is or use them as examples
to create your own.

Unless the files here are included in the project file for your executable,
they will not impact your build.



Board Setup Files (.mbs)
------------------------------------------------------------------------------
The board setup files (.mbs) are used by MULTI to initialize target hardware
before beginning a debugging session.

The automatically-created default.con connection file (located in
default.gpj) contains connection methods that reference the .mbs files
included in resource.gpj.

You can modify the scripts in the board setup files to suit your specific
hardware configuration and connection needs.

For more information about board setup files and connection files, see the
"MULTI: Configuring Connections" book for your target processor.



Linker Directive Files (.ld)
------------------------------------------------------------------------------
Linker directive files (.ld) are used when your program is linked to define
program sections and assign them to specific addresses in memory.

The linker directive file from the project file for your executable will
be used when linking.  

If the .ld file included in the project for your executable does not suit
your hardware configuration or program layout needs, it can be modified or
replaced with a custom .ld file.


This resource.gpj contains example linker directives files:
** standalone_ram.ld      -- For programs that are linked into and run
                             out of RAM. 
** standalone_romcopy.ld  -- For programs that are linked into ROM, but
                             run out of RAM.
** standalone_romrun.ld   -- For programs that are linked into and run
                             out of ROM. 

Some configurations also contain other linker directives files:
** standalone_romdebug.ld -- For programs that are linked into and run
                             out of ROM with enhanced debugging capabilities.
** standalone_pic.ld      -- For programs built with position independent
                             code. 
** standalone_pid.ld      -- For programs built with position independent
                             data.
** standalone_picpid.ld   -- For programs built with position independent
                             code and data.

For more information about linker directives files, see the "MULTI: Building 
Applications" book for your processor.
