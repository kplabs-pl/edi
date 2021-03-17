#ifndef EDI_H
#define EDI_H

struct EDI;
struct edi_message_part;

typedef enum
{
    edi_command_none = 0,
    edi_command_connect = 1,
    edi_command_bind = 2,
    edi_command_disconnect = 3,
    edi_command_write = 4,
    edi_command_write_gather = 5,
    edi_command_read = 6,
    edi_command_read_scatter = 7,
    edi_command_query_message_count = 8,
    edi_command_query_message_size = 9,
    edi_command_remove_message = 10,
    edi_command_set_connection_type = 11
} edi_command;

typedef enum
{
    edi_status_success = 0,
    edi_status_invalid_command = 1,
    edi_status_invalid_address = 2,
    edi_status_invalid_block_size = 3,
    edi_status_disconnected = 4,
    edi_status_write_error = 5,
    edi_status_unable_to_setup_connection = 6,
    edi_status_invalid_communication_mode = 7
} edi_status;

typedef enum
{
    edi_mode_req = 0,
    edi_mode_rep = 1,
    edi_mode_pair = 2,
    edi_mode_pull = 3,
    edi_mode_push = 4,
    edi_mode_pub = 5,
    edi_mode_sub = 6,
    edi_mode_ensure_size = 0xffffffff
} edi_communication_mode;

#endif
