#include <stm32f4xx.h>
#define stack_ptr ((char*)__get_MSP())

extern int write(int file, char *ptr, int len);

#include <errno.h>
#undef errno
extern int errno;
#include <sys/stat.h>
#include <stdlib.h>

void _exit(int status)
{
	while(1);
}

int _close(int file)
{
	return -1;
}

char *__env[1] = { 0 };
char **environ = __env;

int _execve(char *name, char **argv, char **env)
{
	errno = ENOMEM;
	return -1;
}

int _fork(void)
{
	errno = EAGAIN;
	return -1;
}

int _fstat(int file, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}

int _getpid(void)
{
	return 1;
}

int _isatty(int file)
{
	return 1;
}

int _kill(int pid, int sig)
{
	errno = EINVAL;
	return -1;
}

int _link(char *old, char *new)
{
	errno = EMLINK;
	return -1;
}

int _lseek(int file, int ptr, int dir)
{
	return 0;
}

int _open(const char *name, int flags, int mode)
{
	return -1;
}

// read システムコール
// ・ファイルからデータを読み込む
// ・fileから、ptrに書き込む
// ・len読むように試みる
// ・成功：実際に読み込んだバイト数を返す
// ・失敗：-1が返る
// ここではfileはシリアルポート固定
extern char inbyte() __attribute__ ((weak));
int _read(int file, char *ptr, int len)
{
	if(inbyte){
		int in = 0;
		while(in < len){
			*ptr = inbyte();
			in++;
			if(*ptr=='\r') break;
			ptr++;
		}
	return in;
	}
	else return 0;
}

char *init_heap_end; // This should be initialized in startup routine
caddr_t _sbrk(int incr)
{
	static char *heap_end;
	char *prev_heap_end;
	if (heap_end == 0){
		heap_end = init_heap_end;
	}
	prev_heap_end = heap_end;
	if (heap_end + incr > stack_ptr){
		write (1, "Heap and stack collision\n", 25);
		abort ();
	}
	heap_end += incr;
	return (caddr_t) prev_heap_end;
}

int _astat(char *file, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}

int _unlink(char *name)
{
	errno = ENOENT;
	return -1;
}

int _wait(int *status)
{
	errno = ECHILD;
	return -1;
}

// write システムコール
// ・ファイルにデータを書き込む
// ・ptrに格納されているデータを、fileにlen書き込もうと試みる
// ・成功：実際に書き込んだバイト数を返す
// ・失敗：-1を返す
// ここではfileはシリアルポート固定
extern void outbyte(char) __attribute__ ((weak));
int _write(int file, char *ptr, int len)
{
	if(outbyte){
		int todo;
		for(todo = 0; todo < len; todo++){
			outbyte(*ptr++);
		}
		return len;
	}
	else return 0;
}

