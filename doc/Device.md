# External Device Interface

External Device Interface (EDI) is a emulated device that allows software running on the EFM32_TEST_MCU under the Qemu to communicate with external world using
nanonmsg (https://nanomsg.org/) based connection.

## Device

The device itself is visible as set of registers mapped to the address space. There can be more then one EDI instance mapped at any given time. Every present EDI device is completely independent from all other devices.
The device itself is seen as hardware device but it serves as a intermediary that can either connect to the network nanomsg based service or open any nanomsg supported port/protocols combination and wait for incoming connections.

There are two message queues in any of the EDI devices. One queue is dedicated for the incoming messages and one for outgoing messages.

When new message is being received by the EDI device it is added at the end of this queue and new interrupt is generated. This queue has not any built-in length limit.

Queue for outgoing messages is used for caching when the connection itself is temporarily not available. It is limited to 32 messages. When this limit is reached the device will fail to accept new messages to sent until the queue's length drops below limit.

This device generates interrupt in edge mode. For every incoming message interrupt is generated once. It will not be generated again if the message is not immediately received and removed from queue.

### Registers

Each EDI device uses four registers. Each register is four bytes long and serves different purpose:

| register  | offset | size | mode |
|-----------|-------:|:----:|:----:|
| Command   |      0 |   4  |   w  |
| Status    |      0 |   4  |   r  |
| Pointer   |      4 |   4  |  r/w |
| Size      |      8 |   4  |  r/w |
| Interrupt |     12 |   4  |  r/w |
| ID        |     16 |   4  |  r   |

* **Command** register is used to issue commands to the device. Command is issued when new value is written to this register. This register cannot be read.
* **Status** register provides status of the last issued and completed command. This register cannot be written. See the error codes section for the list of currently specified errors.
* **Pointer** register is used to provide device with physical memory address of structure containing parameters for next command send to the EDI device. When next command does not have arguments then contents of this register is ignored.
* **Size** register contains size in bytes of the data structure pointed by the **Pointer** register. When next command does not have arguments then contents of this register is ignored.
* **Interrupt** register contains interrupt number used by the device to signal that there are new incoming messages waiting to be processed. Setting this register value to the 0xffffffff will disable generation of the interrupts.
* **ID** register contains well-known number `0xDEADCAFE` allowing to verify correct configuration of QEMU device.

### Commands

Following table presents list of the available commands:

| command             | code |
|---------------------|-----:|
| None                |    0 |
| Connect             |    1 |
| Bind                |    2 |
| Disconnect          |    3 |
| Write               |    4 |
| Write Gather        |    5 |
| Read                |    6 |
| Read Scatter        |    7 |
| Query Message Count |    8 |
| Query Message Size  |    9 |
| Remove Message      |   10 |
| Set Connection Type |   11 |

All of the commands are performed immediately and from the emulated cpu point of view their execution time is effectively zero.

* **None** - This is no operation command that always generates success response. It does not have any arguments.
* **Connect** - This command requests connecting to the external network service with address specified by the **Pointer** register. Arguments:

    | register | type           | description                                       |
    |----------|----------------|:--------------------------------------------------|
    | Pointer  | const char*    | Address of utf-8 encoded nanomsg network address  |
    | Size     | uint32_t       | Size of string in bytes including null terminator |

    If the device is already connected the existing connection will be closed and new one will be opened. This function provides commit/rollback error handling mode. The existing connection will not be closed unless the new one is already fully setup.
* **Bind** - This command requests opening nanomsg network service. Arguments:

    | register | type           | description                                       |
    |----------|----------------|:--------------------------------------------------|
    | Pointer  | const char*    | Address of utf-8 encoded nanomsg network address  |
    | Size     | uint32_t       | Size of string in bytes including null terminator |

    If the device is already connected the existing connection will be closed and new one will be opened. This function provides commit/rollback error handling mode. The existing connection will not be closed unless the new one is already fully setup.
* **Disconnect** - This command requests shutting down all network communication. This command has no arguments.
* **Write** - This command requests sending message over the connected network link.
    Arguments:

    | register | type           | description                                              |
    |----------|----------------|:---------------------------------------------------------|
    | Pointer  | const uint8_t* | Address of buffer whose content should be sent over the network  |
    | Size     | uint32_t       | Size of buffer in bytes |
* **Write Gather** - This is variant of the **Write** command that is capable of gathering several message parts and merging them together before sending through network connection. The message parts are described by elements in an array whose address is passed to the device in **Pointer** register. Messages are merged in description order.
    Arguments:

    | register | type           | description                                              |
    |----------|----------------|:---------------------------------------------------------|
    | Pointer  | const edi_message_part* | Address of array containing description of message parts |
    | Size     | uint32_t       | Size of array in **bytes**                               |

    Each message part is described with following data structure:

    | name    | offset | size | type           | description                |
    |---------|-------:|-----:|----------------|----------------------------|
    | address | 0      | 4    | const uint8_t* | message part address       |
    | size    | 4      | 4    | uint32_t       | size in bytes of this part |

* **Read** - This command reads contents of the message from the top of the receive message queue. Arguments:

    | register | type           | description                                              |
    |----------|----------------|:---------------------------------------------------------|
    | Pointer  | uint8_t*       | Address of buffer that should be filled with content of the first message in queue |
    | Size     | uint32_t       | Size of buffer in bytes                                  |

    When this command is completed the **Size** register will contain number of bytes that were actually read from message and written to the requested buffer. If next message in queue is shorter than the provided buffer than the buffer will be filled with the message content from the beginning with the extra space being unmodified. The **Size** register will be set to the size of the message in bytes. When there are no messages in queue or the next message in queue is empty this command will return success with number of bytes written to buffer set to zero.
* **Read Scatter** - This variant of the **Read** command is able to read the message from queue and split it into one or more parts as requested by the machine. Description how the message should be split is contained in message part array whose address is passed to the device in **Pointer** register. Arguments:

    | register | type           | description                                              |
    |----------|----------------|:---------------------------------------------------------|
    | Pointer  | edi_message_part* | Address of array containing description of message parts to fill |
    | Size     | uint32_t       | Size of array in **bytes**                               |

    Each message part is described with following data structure:

    | name    | offset | size | type           | description                |
    |---------|-------:|-----:|----------------|----------------------------|
    | address | 0      | 4    | uint8_t*       | message part address       |
    | size    | 4      | 4    | uint32_t       | size in bytes of this part |

    When this command is completed the **Size** register will contain number of bytes written to the message part array. This value describes how many message parts were actually updated if the message was shorter than requested. Also each updated part will have its description updated with number of bytes that were actually written to this part. This number can be smaller than the requested when there were not enough bytes in the message to fill it completely. If there are no messages in queue or the next message from queue is empty the value of the **Size** register will be set to zero.
* **Query Message Count** - This command returns number of the messages in the incoming message queue. This command has no arguments. The number of messages in the queue is returned in the **Size** register.
* **Query Message Size** - This command return size of the next message in bytes from the incoming message queue. This command has no arguments. Size of the message is returned in the **Size** register. When there are no messages in queue the command will return with success and  **Size** register will be set to zero.
* **Remove Message** - This command requests removal of the next message from incoming message queue. This command has no arguments. When there are no messages in incoming message queue this command has no effect and end with success.
* **Set Connection Type** - This command sets communication mode for the next **Connect**/**Bind** commands.
     Arguments:

    | register | type           | description                                              |
    |----------|----------------|:---------------------------------------------------------|
    | Pointer  | const uint8_t* | Address of structure containing communication mode parameters |
    | Size     | uint32_t       | Size of structure in **bytes**                               |

    Each message part is described with following data structure:

    | name    | offset | size | type           | description                |
    |---------|-------:|-----:|----------------|----------------------------|
    | mode    | 0      | 4    | edi_communication_mode | requested communication mode |

    Supported communication modes:

    | mode          | value | nanomsg type |
    |---------------|------:|--------------|
    | edi_mode_req  | 0     | NN_REQ       |
    | edi_mode_rep  | 1     | NN_REP       |
    | edi_mode_pair | 2     | NN_PAIR      |
    | edi_mode_pull | 3     | NN_PULL      |
    | edi_mode_push | 4     | NN_PUSH      |
    | edi_mode_pub  | 5     | NN_PUB       |
    | edi_mode_sub  | 6     | NN_SUB       |

### Error codes

| code                                  | value | description                                                                                     |
|---------------------------------------|------:|-------------------------------------------------------------------------------------------------|
| edi_status_success                    |     0 | no error                                                                                        |
| edi_status_invalid_command            |     1 | unsupported/invalid command requested                                                           |
| edi_status_invalid_address            |     2 | unable to access provided address either from **Pointer** register or any nested data structure |
| edi_status_invalid_block_size         |     3 | invalid value in **Size** register                                                              |
| edi_status_disconnected               |     4 | unable to satisfy request due to device being in disconnected state                             |
| edi_status_write_error                |     5 | unable to send message                                                                          |
| edi_status_unable_to_setup_connection |     6 | unable to setup network connection                                                              |
| edi_status_invalid_communication_mode |     7 | invalid communication mode requested                                                            |

### Initial Device State

Initially the device is disconnected with interrupt being disabled. Default communication mode is set to **edi_mode_pair**.

### Troubleshooting

EDI device has its own dedicated logging channel named `edi`. It can be enabled using Qemu's `-d` option. On this channel edi logs all errors and all important state changes.
