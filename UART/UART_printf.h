/*
File: printf.h

 Copyright (c) 2004,2008 Kustaa Nyholm / SpareTimeLabs
 
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:
 
 Redistributions of source code must retain the above copyright notice, this list
 of conditions and the following disclaimer.
 
 Redistributions in binary form must reproduce the above copyright notice, this
 list of conditions and the following disclaimer in the documentation and/or other
 materials provided with the distribution.
 
 Neither the name of the Kustaa Nyholm or SpareTimeLabs nor the names of its
 contributors may be used to endorse or promote products derived from this software
 without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 OF SUCH DAMAGE.
 
 ----------------------------------------------------------------------

This library is realy just two files: 'printf.h' and 'printf.c'.

They provide a simple and small (+100 loc) printf functionality to 
be used in embedded systems.

I've found them so usefull in debugging that I do not bother with a 
debugger at all.

They are distributed in source form, so to use them, just compile them 
into your project. 

The formats supported by this implementation are: 'd' 'u' 'c' 's' 'x' 'X'.

Zero padding and field width (limited to 255) are also supported.

The memory foot print of course depends on the target cpu, compiler and 
compiler options, but a rough guestimate (based on a HC08 target) is about 
600 - 1100 bytes for code and some twenty bytes of static data.  Note
that this printf is not re-entrant. 

Note that the code expects that int size is 16 bits, and that char is
8 bits.

To use the printf you need to supply your own character output function, 
something like :


The printf function is actually a macro that translates to 'UART_printf'. 
This makes it possible to use it along with 'stdio.h' printf's in a single 
source file. You just need to undef the names before you include the 'stdio.h'.
Note that these are not function-like macros, so if you have variables
or struct members with these names, things will explode in your face.
Without variadic macros this is the best we can do. If it is a  problem 
just give up the macros and use the functions directly or rename them.

For further details see source code.

regs Kusti, 26.2.2008
*/

//in case of multiple printf, only one of the common part will be included
#ifndef __PRINTF_COMMON__
#define __PRINTF_COMMON__

static char* bf;
static char buf[12];
static unsigned int num;
static char uc;
static char zs;


static void out(char c) {
    *bf++ = c;
    }

static void outDgt(char dgt) {
	out(dgt+(dgt<10 ? '0' : (uc ? 'A' : 'a')-10));
	zs=1;
    }

static void divOut(unsigned int div) {
    unsigned char dgt=0;
	num &= 0xffff; // just for testing the code  with 32 bit ints
	while (num>=div) {
		num -= div;
		dgt++;
		}
	if (zs || dgt>0)
		outDgt(dgt);
    }

#endif

#ifndef __PRINTF_UART__
#define __PRINTF_UART__

void UART_printf(char *fmt, ...)
	{
	va_list va;
	char ch;
	char* p;

	va_start(va,fmt);

	while ((ch=*(fmt++))) {
		if (ch!='%') {
			UART_Write_Char(ch);
			}
		else {
			char lz=0;
			char w=0;
			ch=*(fmt++);
			if (ch=='0') {
				ch=*(fmt++);
				lz=1;
				}
			if (ch>='0' && ch<='9') {
				w=0;
				while (ch>='0' && ch<='9') {
					w=(((w<<2)+w)<<1)+ch-'0';
					ch=*fmt++;
					}
				}
			bf=buf;
			p=bf;
			zs=0;
			switch (ch) {
				case 0:
					goto abort;
				case 'u':
				case 'd' :
					num=va_arg(va, unsigned int);
					if (ch=='d' && (int)num<0) {
						num = -(int)num;
						out('-');
						}
					divOut(10000);
					divOut(1000);
					divOut(100);
					divOut(10);
					outDgt(num);
					break;
				case 'x':
				case 'X' :
				    uc= ch=='X';
					num=va_arg(va, unsigned int);
					divOut(0x1000);
					divOut(0x100);
					divOut(0x10);
					outDgt(num);
					break;
				case 'c' :
					out((char)(va_arg(va, int)));
					break;
				case 's' :
					p=va_arg(va, char*);
					break;
				case '%' :
					out('%');
				default:
					break;
				}
			*bf=0;
			bf=p;
			while (*bf++ && w > 0)
				w--;
			while (w-- > 0)
				UART_Write_Char(lz ? '0' : ' ');
			while ((ch= *p++))
				UART_Write_Char(ch);
			}
		}
	abort:;
	va_end(va);
	}

#endif
