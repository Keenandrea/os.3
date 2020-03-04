INTRODUCTION
------------

The goal of this module, Semaphores and Message Pa
ssing, is to become familiar with semaphores in th
e UNIX operating system. In addition, you will imp
lement a concept, provide it as a statically linke
d library, and use it to solve the following probl
em:

Problem: Write a program that computes the summati
on of n integers in each of the following ways and
assess their performance. Assume that n is a power
of 2.

	1. Partition the n integers into n/2 pairs
	   Employ n/2 processes to add together ea
	   ch pair of integers resulting in n/2 in
	   tegers. Repeat the method on the n/2 in
	   tegers and continue until the final res
	   ult is obtained. (This is a binary tree
	   algorithm.)

	2. Divide the n integers into n/ log n gro
	   ups of log n numbers each. Use n/ log n
	   processes, each adding the numbers in o
	   ne group sequentially. Then add the n/l
	   og n results using method 1.

 * For a full description of the module, visit th
   e project page of CS4760: Operating Systems at:
   http://www.cs.umsl.edu/~sanjiv/classes/cs4760/

 * To submit bug reports and feature suggestions o
   r track changes visit:
   https://github.com/Keenandrea?tab=repositories


REQUIREMENTS
------------

This module requires the following:

 * Hoare Linux (http://www.umsl.edu/technology/tsc/)
 * UMSL ID (https://www.umsl.edu/degrees/index.html)


INSTALLATION
------------

 * Install as you would normally install a distrib
   uted C module. To compile and execute, you must
   have the build-essential packages properly inst
   alled on your system. 


COMPILATION
-----------

To compile, open a new CLI window, change the the
directory nesting your module. Type:

 * make


EXECUTION
---------
										    Find the executable named master located in the d
irectory in which you compiled the module. Once y
ou have found it, run the following command:

	./master

That will get the ball rolling. Check the ouput f
ile for details on execution.


MAINTAINERS
-----------

Current maintainers:
 * Keenan Andrea - https://github.com/Keenandrea

This project has been sponsored by:
 * UMSL Mathematics and Computer Science 
   Specialized in bringing young minds to graduat
   ion with the skills needed. 


MODIFICATION LOG
------------ ---

LOG #  DATE:	DESCRIPTION:

LOG 1  3/2/20   Wrote three files according to th
		e project requirements: master.c,
		bin_addr.c, and makefile. Used va
		riable oriented rules to create m
		akefile commands. Included suffix
		rules, as per professor. Included
		-lpthread linker option for <pthr
		ead> preprocessor directive. Then
		I programmed 6 functions, not inc
		luding main, in master.c. These f
		unctions, temporally and respecti
		vely, handled interrupt signals o
		f 100 second timeout or ctrl-c; c
		onverted a pathname and project i
		dentifier to System V IPC key usi
		ng ftok, created shared memory, a
		ttached shared memory, and initia
		ted two semaphores, one to be use
		d for n/2 computations, and the o
		ther for n/log n computations. Th
		en I wrote a seperate header file
		shmem.h. There I placed the two s
		emaphores into a struct. I linked
		the .h in my .c files, and shared
		it in my makefile. I wrote anothe
		r empty file "shmfile" for shared
		memory. The remaining three funct
		ions detach the shared memory seg
		ment, and write 64 random integer
		s, one integer per line. Each int
		eger was randomly numbered [1 ...
		256]. The integers were written t
		o file input.dat, the file was cl
		osed.

LOG 2  3/3/20   Wrote comments. Programmed new fu
		nction readtosm() that opens inpu
		t data file and reads contents in
		to char array that is then itself
		read into a shared memory integer
		array. So: all integers are now r
		ead into shared memory from the s
		pecified file.

PERFORMANCE REVIEW
----------- ------
