/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Donlon
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <string.h>

#include "py/runtime.h"
#include "extmod/vfs.h"
#include "modrp2.h"
#include "pico/binary_info.h"

#include <malloc.h>

#define BLOCK_SIZE_BYTES   512
#define MAX_MEM_SIZE_BYTES (32 * 1024) // 32k

typedef struct _rp2_memblocks_obj_t {
    mp_obj_base_t base;
    uint32_t size;
    bool first_created;
    uint8_t *mem_base;
} rp2_memblocks_obj_t;

uint8_t *memblocks_ptr = NULL;
uint32_t memblocks_size = 0;

STATIC mp_obj_t rp2_memblocks_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    static const mp_arg_t allowed_args[] = {
            { MP_QSTR_size, MP_ARG_REQUIRED | MP_ARG_INT },
    };
    // Parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int mem_size = args[0].u_int;
    if (mem_size < 0) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid memory size %d"), mem_size);
    }
    if (mem_size > MAX_MEM_SIZE_BYTES) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("memory size %d too large"), mem_size);
    }
    if (mem_size < BLOCK_SIZE_BYTES) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("memory size should be at least %d"), BLOCK_SIZE_BYTES);
    }
    mem_size &= ~(BLOCK_SIZE_BYTES - 1);
    rp2_memblocks_obj_t *memblocks = m_new_obj(rp2_memblocks_obj_t);
    if (memblocks_ptr) {
        if (mem_size != memblocks_size) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("memory size %d is different from previous %d"),
                              mem_size, memblocks_size);
        }
        memblocks->first_created = false;
    } else {
        memblocks_size = mem_size;
        memblocks_ptr = malloc(mem_size);
        memblocks->first_created = true;
        memset(memblocks_ptr, 0, mem_size);
    }
    memblocks->base.type = &rp2_memblocks_type;
    memblocks->size = memblocks_size;
    memblocks->mem_base = memblocks_ptr;
    return MP_OBJ_FROM_PTR(memblocks);
}

STATIC mp_obj_t rp2_memblocks_readblocks(size_t n_args, const mp_obj_t *args) {
    rp2_memblocks_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t offset = mp_obj_get_int(args[1]) * BLOCK_SIZE_BYTES;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_WRITE);
    if (n_args == 4) {
        offset += mp_obj_get_int(args[3]);
    }
    memcpy(bufinfo.buf, self->mem_base + offset, bufinfo.len);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rp2_memblocks_readblocks_obj, 3, 4, rp2_memblocks_readblocks);

STATIC mp_obj_t rp2_memblocks_writeblocks(size_t n_args, const mp_obj_t *args) {
    rp2_memblocks_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t offset = mp_obj_get_int(args[1]) * BLOCK_SIZE_BYTES;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_READ);
    if (n_args == 4) {
        offset += mp_obj_get_int(args[3]);
    }
    memcpy(self->mem_base + offset, bufinfo.buf, bufinfo.len);
    // TODO check return value
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rp2_memblocks_writeblocks_obj, 3, 4, rp2_memblocks_writeblocks);

STATIC mp_obj_t rp2_memblocks_ioctl(mp_obj_t self_in, mp_obj_t cmd_in, mp_obj_t arg_in) {
    rp2_memblocks_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t cmd = mp_obj_get_int(cmd_in);
    switch (cmd) {
        case MP_BLOCKDEV_IOCTL_INIT:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_DEINIT:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_SYNC:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
            return MP_OBJ_NEW_SMALL_INT(self->size / BLOCK_SIZE_BYTES);
        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
            return MP_OBJ_NEW_SMALL_INT(BLOCK_SIZE_BYTES);
        case MP_BLOCKDEV_IOCTL_BLOCK_ERASE: {
            return MP_OBJ_NEW_SMALL_INT(0);
        }
        default:
            return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(rp2_memblocks_ioctl_obj, rp2_memblocks_ioctl);

STATIC mp_obj_t rp2_memblocks_first_created(mp_obj_t self_in) {
    rp2_memblocks_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->first_created) {
        return mp_const_true;
    } else {
        return mp_const_false;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(rp2_memblocks_first_created_obj, rp2_memblocks_first_created);

STATIC const mp_rom_map_elem_t rp2_memblocks_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&rp2_memblocks_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&rp2_memblocks_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&rp2_memblocks_ioctl_obj) },
    { MP_ROM_QSTR(MP_QSTR_first_created), MP_ROM_PTR(&rp2_memblocks_first_created_obj) },
};
STATIC MP_DEFINE_CONST_DICT(rp2_memblocks_locals_dict, rp2_memblocks_locals_dict_table);

const mp_obj_type_t rp2_memblocks_type = {
    { &mp_type_type },
    .name = MP_QSTR_MemBlocks,
    .make_new = rp2_memblocks_make_new,
    .locals_dict = (mp_obj_dict_t *)&rp2_memblocks_locals_dict,
};
