/* SDSLib, A C dynamic strings library
 *
 * Copyright (c) 2006-2010, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __SDS_H
#define __SDS_H

#define SDS_MAX_PREALLOC (1024*1024)

#include <sys/types.h>
#include <stdarg.h>

typedef char *sds;

struct sdshdr {
    unsigned int len;	//buf中已使用的字节数
    unsigned int free;	//buf中未使用的字节数
    char buf[];		//缓冲区
};

//获取sds字符串的长度
static inline size_t sdslen(const sds s) {
    struct sdshdr *sh = (void*)(s-(sizeof(struct sdshdr)));
    return sh->len;
}

//获取sdshdr结构可用内存的大小
static inline size_t sdsavail(const sds s) {
    struct sdshdr *sh = (void*)(s-(sizeof(struct sdshdr)));
    return sh->free;
}

 //创建一个sds字符串，初始内容为init所指向的内容，buf空间为initlen大小
sds sdsnewlen(const void *init, size_t initlen);

//创建一个包含init字符串内容的sds字符串
sds sdsnew(const char *init);

//创建一个空的sds字符串
sds sdsempty(void);

//获取sds字符串的长度
size_t sdslen(const sds s);

//复制一个sds字符串
sds sdsdup(const sds s);

//释放sds字符串
void sdsfree(sds s);

//获取sdshdr结构可用内存的大小
size_t sdsavail(const sds s);

/*  增加sds字符串的长度，确保其长度大于等于len,如果sds长度大于len，则什么都不做,
*   否则，将其使用长度扩展为len，并将多余的扩展空间置为0
*/
sds sdsgrowzero(sds s, size_t len);

 //将t所指向的内容追加到sds字符串中
sds sdscatlen(sds s, const void *t, size_t len);

//将C字符串追加到sds字符串末尾
sds sdscat(sds s, const char *t);

//将sds字符串t追加到sds字符串s中
sds sdscatsds(sds s, const sds t);

//将t所指向的内容复制到sds字符串中（覆盖原来的值）
sds sdscpylen(sds s, const char *t, size_t len);

//将C字符串复制到sds字符串中（覆盖）
sds sdscpy(sds s, const char *t);

//将可变参数列表以格式fmt追加到sds字符串s的尾部
sds sdscatvprintf(sds s, const char *fmt, va_list ap);
#ifdef __GNUC__
//将可变参数以格式fmt追加到sds字符串s的尾部
sds sdscatprintf(sds s, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));
#else
sds sdscatprintf(sds s, const char *fmt, ...);
#endif

/*  格式化字符串，并拼接到sds字符串s的尾部
*   这里作者认为libc库的sprintf实现性能较差，所以实现了该功能以提高性能
*/
sds sdscatfmt(sds s, char const *fmt, ...);

//去掉sds字符串中前缀和后缀包含cset的字符
sds sdstrim(sds s, const char *cset);

//截取，保留[start,end]之间的字符，若<0则从尾部开始计算
void sdsrange(sds s, int start, int end);

//更新sds字符串的长度，更新长度为第一个'\0'的位置
void sdsupdatelen(sds s);

//清空sds字符串，但是不释放空间
void sdsclear(sds s);

 /*sds字符串之间比较，首先比较minlen范围的内容，若内容一致，则比较两个字符串的长度
 *  minlen = len(s1), len(s2)
 */
int sdscmp(const sds s1, const sds s2);

 /*使用sep分割字符串s，返回sds字符串数组，其大小通过count返回
 *  所分割后的sds数组可通过sdsfreesplitres释放
 */
sds *sdssplitlen(const char *s, int len, const char *sep, int seplen, int *count);

//释放由sdssplitlen()函数返回的数据
void sdsfreesplitres(sds *tokens, int count);

//将sds字符串转换为小写字符
void sdstolower(sds s);

//将sds字符串转换为大写字符
void sdstoupper(sds s);

 //将long long值转为sds字符串
sds sdsfromlonglong(long long value);

 //将p所指向的内容转换为可视字符，添加到sds字符串尾部（其中转换后的字符串前后添加了""）
sds sdscatrepr(sds s, const char *p, size_t len);

 /* 将字符串line分割转换为sds字符串，其中以'\0' '\n' '\r' ' ' '\t'分割，
 *  注：允许 demo"hello" demo类型字符串，但是"结尾下一个字符必须符合isspace()
 *  即demo"hello"demo类型字符串会以失败处理
*   会对双引号的内容进行解析，不对单引号的内容解析
*/
sds *sdssplitargs(const char *line, int *argc);

 /* 将sds中字符等于from[j]替换为to[j]中的字符，其中 j >= 0 && j < setlen
 *  即sds中hello，from=ho，to=01，其中sds中的h与from[0]相等，则以to[0]替换
 */
sds sdsmapchars(sds s, const char *from, const char *to, size_t setlen);

//将字符串数组argv通过sep字符串连接起来，并存为sds字符串
sds sdsjoin(char **argv, int argc, char *sep);

/* Low level functions exposed to the user API */
//保证sds字符串有足够的剩余未使用空间（大于或等于addlen）
sds sdsMakeRoomFor(sds s, size_t addlen);

//用于在剩余空间写入数据时，更新使用的大小。
void sdsIncrLen(sds s, int incr);

//去除sds字符串中未使用的空间，一般在内存紧张的时候使用
sds sdsRemoveFreeSpace(sds s);

//获取sds字符串所占用的内存大小，包括结构体，未使用内存，使用内存。
size_t sdsAllocSize(sds s);

#endif
