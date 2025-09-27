// Minimal local copy marker. Please replace with official stb_image.h if needed.
#ifndef STB_IMAGE_H
#define STB_IMAGE_H

// This is a placeholder to avoid missing include errors.
// For full functionality, please replace with the official stb_image.h
// from https://github.com/nothings/stb (public domain).

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char stbi_uc;

static inline stbi_uc* stbi_load(const char* filename, int* x, int* y, int* comp, int req_comp) {
    (void)filename; (void)x; (void)y; (void)comp; (void)req_comp;
    return (stbi_uc*)0; // always fail; placeholder
}

static inline void stbi_image_free(void* retval_from_stbi_load) {
    (void)retval_from_stbi_load;
}

#ifdef __cplusplus
}
#endif

#endif // STB_IMAGE_H
