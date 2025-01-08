# libuart

Easy to use library for accessing the UART (RS232) interface.


## Functions:

### uart\_t *uart\_open(const char *dev, int baud, const char *opt)

Open a UART port connection.

#### Arguments:
Arg | Description
--- | -----------
*dev* | The device name of the UART port
*baud* | The baud rate of the UART port (Use the *enums* in the header file, other values are not accepted)
*opt* | The configuration string of the UART port

The configuration string must be 4 chars long. The first char represents the number of data bits (valid values are **5**, **6**, **7** or **8**), the second char represents the parity (valid values are **N** for No parity, **O** for Odd parity or **E** for Even parity), the third char represents the number of stop bits (valid values are **1** or **2**) and the fourth char represents the flow control (valid values are **N** for No flow control, **S** for Software flow control or **H** for Hardware flow control).

*Example configuration string:*  
**8N1N**

Char | Description
---- | -----------
8 | 8 data bits
N | No parity
1 | 1 stop bit
N | No flow control

#### Return:
On success, an *uart\_t* object will be returned. On error, a NULL pointer will be returned.

### void uart\_close(uart\_t *uart)

Close the connection.

#### Arguments:
Arg | Description
--- | -----------
*uart* | The *uart_t* object

### int uart\_send(uart\_t *uart, char *send\_buf, int len)

Transmit data via UART.

### Arguments:
Arg | Description
--- | -----------
*uart* | The *uart_t* object
*send\_buf* | The pointer to the data to transmit
*len* | The length of the data in bytes

#### Return:
On success, the number of transmited bytes will be returned. On error, -1 will be returned.

### int uart\_recv(uart\_t *uart, char *recv\_buf, int len)

Receive data from the UART port.

### Arguments:
Arg | Description
--- | -----------
*uart* | The *uart_t* object
*recv\_buf* | The pointer where the received data is stored
*len* | The length of the data in bytes


#### Return:
On success, the number of received bytes will be returned. On error, -1 will be returned.

### void uart\_baud\_set(uart\_t *uart, int baud)

Set the baud rate from the UART port.

#### Arguments:
Arg | Description
--- | -----------
*uart* | The *uart_t* object
*baud* | The baud rate (Use the *enums* in the header file, other values where not accepted)

### int uart\_baud\_get(uart\_t *uart)

Get the current baud rate from the UART port.

#### Arguments:
Arg | Description
--- | -----------
*uart* | The *uart_t* object

#### Return:
On success, the baud rate will be returned. On error, -1 will be returned.

### int uart\_fd\_get(uart\_t *uart)

Get the file descriptor from the UART port. (Linux/UNIX only) 

#### Arguments:
Arg | Description
--- | -----------
*uart* | The *uart_t* object

#### Return:
On success, the file descriptor will be returned. On error, -1 will be returned.

### int uart\_handle\_get(uart\_t *uart)

Get the handle from the UART port. (Windows only)

#### Arguments:
Arg | Description
--- | -----------
*uart* | The *uart_t* object

#### Return:
On success, the handle will be returned. On error, -1 will be returned.

### char *uart\_dev\_get(uart\_t *uart)

Get the device of the from the UART port.

#### Arguments:
Arg | Description
--- | -----------
*uart* | The *uart_t* object

#### Return:
On success, the device name will be returned. After usage the string must be freed. On error, a NULL pointer will be returned.

### void uart\_databits\_set(uart\_t *uart, int data\_bits)

Set the data bits from the UART port.

#### Arguments:
Arg | Description
--- | -----------
*uart* | The *uart_t* object
*data\_bits* | The number of data bits (valid values are **5**, **6**, **7** ot **8**)

### int uart\_databits\_get(uart\_t *uart)

Get the data bits from the uart port.

#### Arguments:
Arg | Description
--- | -----------
*uart* | The *uart_t* object

#### Return:
On success, the number of data bits will be returned. On error, -1 will be returned.

### void uart\_parity\_set(uart\_t *uart, int parity)

Set the parity from the UART port.

#### Arguments:
Arg | Description
--- | -----------
*uart* | The *uart_t* object
*parity* | The parity (valid values are **UART\_PARITY\_NO**, **UART\_PARITY\_ODD** or **UART\_PARITY\_EVEN**)

### int uart\_parity\_get(uart\_t *uart)

Get the parity from the UART port.

#### Arguments:
Arg | Description
--- | -----------
*uart* | The *uart_t* object

#### Return:
On success, the parity will be returned. On error, -1 will be returned.

### void uart\_stopbits\_set(uart\_t *uart, int stop\_bits)

Set the stop bits from the UART port.

#### Arguments:
Arg | Description
--- | -----------
*uart* | The *uart_t* object
*stop\_bits* | The number of stop bits (valid values are **1** or **2**)

### int uart\_stopbits\_get(uart\_t *uart)

Get the stop bits from the UART port.

#### Arguments:
Arg | Description
--- | -----------
*uart* | The *uart_t* object

#### Return:
On success, the number of stop bits will be returned. On error, -1 will be returned.

### void uart\_flow\_set(uart\_t *uart, int flow\_ctrl)

Set the flow control from the UART port.


#### Arguments:
Arg | Description
--- | -----------
*uart* | The *uart_t* object
*flow\_ctrl* | The flow control (valid values are **UART\_FLOW\_NO**, **UART\_FLOW\_SOFTWARE** or **UART\_FLOW\_HARDWARE**)

### int uart\_flow\_get(uart\_t *uart)

Get the flow control from the UART port.

#### Arguments:
Arg | Description
--- | -----------
*uart* | The *uart_t* object

#### Return:
On success, the flow control will be returned. On error, -1 will be returned.

### void uart\_pin\_set(uart\_t *uart, int pin, int state)

Set the state of a specific pin from the UART port.

#### Arguments:
Arg | Description
--- | -----------
*uart* | The *uart_t* object
*pin* | The control pin of the UART
*state* | The state of the pin to set (valid values are **0** or **1**)

##### Pins:
Pin | Description
--- | -----------
UART\_PIN\_RTS | Request to Send
UART\_PIN\_CTS | Clear to Send
UART\_PIN\_DSR | Data Set Ready
UART\_PIN\_DCD | Data Carrier Detect
UART\_PIN\_DTR | Data Terminal Ready
UART\_PIN\_RI | Ring Indicator

### int uart\_pin\_get(uart\_t *uart, int pin)

Get the state of a specific pin from the UART port.

#### Arguments:
Arg | Description
--- | -----------
*uart* | The *uart_t* object
*pin* | The control pin of the UART

#### Return:
On success, the state of the pin will be returned. On error, -1 will be returned.

### int uart\_bytes\_get(uart\_t *uart);

Get the current available bytes in the receive buffer.

#### Arguments:
Arg | Description
--- | -----------
*uart* | The *uart_t* object

#### Return:
On success, the number of bytes in the receive buffer will be returned. On error, -1 will be returned.
