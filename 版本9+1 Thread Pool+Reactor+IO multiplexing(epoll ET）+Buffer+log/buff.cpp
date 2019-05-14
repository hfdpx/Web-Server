#include "buff.h"

const char Buffer::kCRLF[] = "\r\n";

const size_t Buffer::kCheapPrepend;
const size_t Buffer::kInitialSize;

//buffer大小不够的时候，使用栈上的空间，避免内存使用过大，提高内存使用效率
ssize_t Buffer::readFd(int fd, int* savedErrno)
{
    //栈额外空间 用于从套接字读数据但Buffer不够用时暂存数据，待buffer重新分配足够空间后，再把数据交换给Buffer
    //这样就buffer就能扩容得恰到好处！
    //节省一次ioctl系统调用（获取有多少可读数据）
    char extrabuf[65536];
    struct iovec vec[2];

    const size_t writable = writableBytes();

    //第一块缓冲区
    vec[0].iov_base = begin() + writerIndex_;
    vec[0].iov_len = writable;

    //第二块缓冲区
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;
    const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
    const ssize_t n = readv(fd, vec, iovcnt);

    if (n < 0)
    {
        *savedErrno = errno;
    }
    //第一块缓冲区足够容纳
    else if (static_cast<size_t>(n) <= writable)
    {
        writerIndex_ += n;
    }
    //当前缓冲区不够容纳，因而数据被接收到了第二块缓冲区，将其append至buffer
    else
    {
        writerIndex_ = buffer_.size();
        append(extrabuf, n - writable);
    }
    return n;
}

//读取一行数据
bool Buffer::getLine(char *dest, size_t len)
{
    const char* end = findEOL();
    if (end == 0) return false; /* 没有找到换行符 */

    const char* start = peek();
    assert(end >= start); /* 保证size是一个正数,然后下面static_cast转换的时候才会正确 */
    ptrdiff_t size = end - start - 1;

    if (len < static_cast<size_t>(size))
    {
        return false; /* 空间不够 */
    }
    std::copy(start, end - 1, dest);
    dest[size] = 0;
    retrieveUntil(end + 1);
    return true;
}

void Buffer::appendStr(const char* format, ...)   /* 格式化输入 */
{
    char extralbuf[256];
    memset(extralbuf, 0, sizeof extralbuf);
    va_list arglist;
    va_start(arglist, format);
    vsnprintf(extralbuf, sizeof extralbuf, format, arglist);
    va_end(arglist);
    append(extralbuf, strlen(extralbuf));
}
