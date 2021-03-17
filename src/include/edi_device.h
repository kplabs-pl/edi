#ifndef EDI_DEVICE_H
#define EDI_DEVICE_H

#pragma once

#include "edi.h"
#include <stdint.h>

typedef struct EDI
{
    volatile uint32_t command_register;
    volatile uintptr_t pointer_register;
    volatile uint32_t size_register;
    volatile uint32_t interrupt_register;
    volatile uint32_t id_register;
} EDI;

typedef struct edi_message_part
{
    void* buffer;
    uint32_t size;
} edi_message_part;

#define EDI_DEVICE_ADDRESS(base, index) (base + (index) * 0x20)
#define EDI_DEFAULT_BASE_ADDRESS 0xF0000010

#define EDI0_BASE_ADDRESS EDI_DEVICE_ADDRESS(EDI_DEFAULT_BASE_ADDRESS, 0)
#define EDI1_BASE_ADDRESS EDI_DEVICE_ADDRESS(EDI_DEFAULT_BASE_ADDRESS, 1)
#define EDI2_BASE_ADDRESS EDI_DEVICE_ADDRESS(EDI_DEFAULT_BASE_ADDRESS, 2)
#define EDI3_BASE_ADDRESS EDI_DEVICE_ADDRESS(EDI_DEFAULT_BASE_ADDRESS, 3)
#define EDI4_BASE_ADDRESS EDI_DEVICE_ADDRESS(EDI_DEFAULT_BASE_ADDRESS, 4)
#define EDI5_BASE_ADDRESS EDI_DEVICE_ADDRESS(EDI_DEFAULT_BASE_ADDRESS, 5)
#define EDI6_BASE_ADDRESS EDI_DEVICE_ADDRESS(EDI_DEFAULT_BASE_ADDRESS, 6)
#define EDI7_BASE_ADDRESS EDI_DEVICE_ADDRESS(EDI_DEFAULT_BASE_ADDRESS, 7)
#define EDI8_BASE_ADDRESS EDI_DEVICE_ADDRESS(EDI_DEFAULT_BASE_ADDRESS, 8)
#define EDI9_BASE_ADDRESS EDI_DEVICE_ADDRESS(EDI_DEFAULT_BASE_ADDRESS, 9)
#define EDI10_BASE_ADDRESS EDI_DEVICE_ADDRESS(EDI_DEFAULT_BASE_ADDRESS, 10)
#define EDI11_BASE_ADDRESS EDI_DEVICE_ADDRESS(EDI_DEFAULT_BASE_ADDRESS, 11)
#define EDI12_BASE_ADDRESS EDI_DEVICE_ADDRESS(EDI_DEFAULT_BASE_ADDRESS, 12)
#define EDI13_BASE_ADDRESS EDI_DEVICE_ADDRESS(EDI_DEFAULT_BASE_ADDRESS, 13)
#define EDI14_BASE_ADDRESS EDI_DEVICE_ADDRESS(EDI_DEFAULT_BASE_ADDRESS, 14)
#define EDI15_BASE_ADDRESS EDI_DEVICE_ADDRESS(EDI_DEFAULT_BASE_ADDRESS, 15)
#define EDI16_BASE_ADDRESS EDI_DEVICE_ADDRESS(EDI_DEFAULT_BASE_ADDRESS, 16)
#define EDI17_BASE_ADDRESS EDI_DEVICE_ADDRESS(EDI_DEFAULT_BASE_ADDRESS, 17)
#define EDI18_BASE_ADDRESS EDI_DEVICE_ADDRESS(EDI_DEFAULT_BASE_ADDRESS, 18)
#define EDI19_BASE_ADDRESS EDI_DEVICE_ADDRESS(EDI_DEFAULT_BASE_ADDRESS, 19)

#define EDI0 ((EDI*)EDI0_BASE_ADDRESS)
#define EDI1 ((EDI*)EDI1_BASE_ADDRESS)
#define EDI2 ((EDI*)EDI2_BASE_ADDRESS)
#define EDI3 ((EDI*)EDI3_BASE_ADDRESS)
#define EDI4 ((EDI*)EDI4_BASE_ADDRESS)
#define EDI5 ((EDI*)EDI5_BASE_ADDRESS)
#define EDI6 ((EDI*)EDI6_BASE_ADDRESS)
#define EDI7 ((EDI*)EDI7_BASE_ADDRESS)
#define EDI8 ((EDI*)EDI8_BASE_ADDRESS)
#define EDI9 ((EDI*)EDI9_BASE_ADDRESS)
#define EDI10 ((EDI*)EDI10_BASE_ADDRESS)
#define EDI11 ((EDI*)EDI11_BASE_ADDRESS)
#define EDI12 ((EDI*)EDI12_BASE_ADDRESS)
#define EDI13 ((EDI*)EDI13_BASE_ADDRESS)
#define EDI14 ((EDI*)EDI14_BASE_ADDRESS)
#define EDI15 ((EDI*)EDI15_BASE_ADDRESS)
#define EDI16 ((EDI*)EDI16_BASE_ADDRESS)
#define EDI17 ((EDI*)EDI17_BASE_ADDRESS)
#define EDI18 ((EDI*)EDI18_BASE_ADDRESS)
#define EDI19 ((EDI*)EDI19_BASE_ADDRESS)

static const uint32_t edi_interrupt_disabled_mask = UINT32_MAX;

static inline edi_status edi_device_execute(EDI* device, edi_command command, const void* buffer, uint32_t size)
{
    __sync_synchronize();
    device->size_register = size;
    device->pointer_register = (uintptr_t)buffer;
    device->command_register = command;
    return (edi_status)(device->command_register);
}

static inline void edi_disable_interrupt(EDI* device)
{
    device->interrupt_register = edi_interrupt_disabled_mask;
}

static inline void edi_enable_interrupt(EDI* device, uint32_t interrupt_number)
{
    device->interrupt_register = interrupt_number;
}

static inline bool edi_verify_id(EDI* device)
{
    return device->id_register == 0xDEADCAFE;
}

#endif // EDI_H

