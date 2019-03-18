//
// Created by wlwx on 2019/3/17.
//

#ifndef EASY_CHAT_BUFFER_H
#define EASY_CHAT_BUFFER_H

#include <algorithm>
#include <vector>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <string>


class Buffer
{
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    explicit Buffer(size_t initialSize = kInitialSize)
            : buffer_(kCheapPrepend + initialSize),
              readerIndex_(kCheapPrepend),
              writerIndex_(kCheapPrepend)
    {
        assert(readableBytes() == 0);
        assert(writableBytes() == initialSize);
        assert(prependableBytes() == kCheapPrepend);
    }

    void swap(Buffer& rhs)
    {
        buffer_.swap(rhs.buffer_);
        std::swap(readerIndex_, rhs.readerIndex_);
        std::swap(writerIndex_, rhs.writerIndex_);
    }

    size_t readableBytes() const
    { return writerIndex_ - readerIndex_; }

    size_t writableBytes() const
    { return buffer_.size() - writerIndex_; }

    size_t prependableBytes() const
    { return readerIndex_; }

    const char* peek() const
    { return begin() + readerIndex_; }

    const char* findCRLF() const
    {
        const char* crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF+2);
        return crlf == beginWrite() ? NULL : crlf;
    }

    const char* findCRLF(const char* start) const
    {
        assert(peek() <= start);
        assert(start <= beginWrite());
        const char* crlf = std::search(start, beginWrite(), kCRLF, kCRLF+2);
        return crlf == beginWrite() ? NULL : crlf;
    }

    const char* findEOL() const
    {
        const void* eol = memchr(peek(), '\n', readableBytes());
        return static_cast<const char*>(eol);
    }

    const char* findEOL(const char* start) const
    {
        assert(peek() <= start);
        assert(start <= beginWrite());
        const void* eol = memchr(start, '\n', beginWrite() - start);
        return static_cast<const char*>(eol);
    }

    void retrieve(size_t len)
    {
        assert(len <= readableBytes());
        if (len < readableBytes())
        {
            readerIndex_ += len;
        }
        else
        {
            retrieveAll();
        }
    }

    void retrieveUntil(const char* end)
    {
        assert(peek() <= end);
        assert(end <= beginWrite());
        retrieve(end - peek());
    }

    void retrieveAll()
    {
        readerIndex_ = kCheapPrepend;
        writerIndex_ = kCheapPrepend;
    }

    std::string retrieveAllAsString()
    {
        return retrieveAsString(readableBytes());
    }

    std::string retrieveAsString(size_t len)
    {
        assert(len <= readableBytes());
        std::string result(peek(), len);
        retrieve(len);
        return result;
    }

    void append(const std::string& str)
    {
        append(str.c_str(), sizeof(str.c_str()));
    }

    void append(const char* data, size_t len)
    {
        ensureWritableBytes(len);
        std::copy(data, data+len, beginWrite());
        hasWritten(len);
    }

    void append(const void* data, size_t len)
    {
        append(static_cast<const char*>(data), len);
    }

    void ensureWritableBytes(size_t len)
    {
        if (writableBytes() < len)
        {
            makeSpace(len);
        }
        assert(writableBytes() >= len);
    }

    char* beginWrite()
    { return begin() + writerIndex_; }

    const char* beginWrite() const
    { return begin() + writerIndex_; }

    void hasWritten(size_t len)
    {
        assert(len <= writableBytes());
        writerIndex_ += len;
    }

    void unwrite(size_t len)
    {
        assert(len <= readableBytes());
        writerIndex_ -= len;
    }

    size_t internalCapacity() const
    {
        return buffer_.capacity();
    }

    ssize_t readFd(int fd, int* savedErrno);

private:

    char* begin()
    { return &*buffer_.begin(); }

    const char* begin() const
    { return &*buffer_.begin(); }

    void makeSpace(size_t len)
    {
        if (writableBytes() + prependableBytes() < len + kCheapPrepend)
        {
            buffer_.resize(writerIndex_+len);
        }
        else
        {
            assert(kCheapPrepend < readerIndex_);
            size_t readable = readableBytes();
            std::copy(begin()+readerIndex_,
                      begin()+writerIndex_,
                      begin()+kCheapPrepend);
            readerIndex_ = kCheapPrepend;
            writerIndex_ = readerIndex_ + readable;
            assert(readable == readableBytes());
        }
    }

    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t  writerIndex_;
    static const char kCRLF[];

};
#endif //EASY_CHAT_BUFFER_H
