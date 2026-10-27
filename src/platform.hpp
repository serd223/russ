#pragma once

#include <stdint.h>
typedef struct {
    uint8_t r, g, b, a;
} Color;

class IPlatform {
public:
    virtual void init(const char* title, int w, int h) = 0;
    virtual void destroy() = 0;
    virtual void clear(Color color) = 0;
    virtual uint32_t mapColor(Color color) = 0;
    virtual int width() = 0;
    virtual int height() = 0;
    virtual int stride() = 0;
    virtual void* data() = 0;
};
