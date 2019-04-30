#include "e1000.h"
#include "defs.h"
#include "defs.h"
#include "defs.h"

#include "mmu.h"
#include "memlayout.h"

// LAB 6: Your driver code here
static uint* e1000;

//
struct e1000_tx_desc tx_desc_buf[TXRING_LEN] __attribute__ ((aligned (PGSIZE)));
struct e1000_data tx_data_buf[TXRING_LEN] __attribute__ ((aligned (PGSIZE)));
//
struct e1000_rx_desc rx_desc_buf[RXRING_LEN] __attribute__ ((aligned (PGSIZE)));
struct e1000_data rx_data_buf[RXRING_LEN] __attribute__ ((aligned (PGSIZE)));

/**
* @brief Function that does the initial descriptions for the queue and registers.
* This method of type void initilizes the sending and recieveing buffers.
* There are two for loops which set the addresses of each spot in the buffer.
* init_desc() cycles through data up to TXRING_LEN (64) & adds the
* appropriate buffer address containing the physical addr.
* @param none
* @see e1000_init()
*/
static void
init_desc(){
	int i;
	for (i = 0; i < TXRING_LEN; ++i)
	{
		//tx_desc_buf[i].buffer_addr = V2P(&tx_data_buf[i]);
		tx_desc_buf[i].buffer_addr = (int)&tx_data_buf[i];
		tx_desc_buf[i].upper.fields.status = E1000_TXD_STAT_DD;
	}
	//
	for (i = 0; i < RXRING_LEN; ++i)
	{
		//rx_desc_buf[i].buffer_addr = V2P(&rx_data_buf[i]);
		rx_desc_buf[i].buffer_addr = (int)&rx_data_buf[i];
	}
}

/**
* @brief helper method to initlize hardware/registers
* This method initlizes the E1000 card with specific buffer values.
* @param none
* @return none
*/
static void
e1000_init(){
	//assert(e1000[E1000_STATUS] == 0x80080783);
	//e1000[E1000_TDBAL] = V2P(tx_desc_buf);
	e1000[E1000_TDBAL] = (int)tx_desc_buf;
	e1000[E1000_TDBAH] = 0x0;
	e1000[E1000_TDH] = 0x0;
	e1000[E1000_TDT] = 0x0;
	e1000[E1000_TDLEN] = TXRING_LEN * sizeof(struct e1000_tx_desc);
	e1000[E1000_TCTL] = VALUEATMASK(1, E1000_TCTL_EN) |
						VALUEATMASK(1, E1000_TCTL_PSP) |
						VALUEATMASK(0x10, E1000_TCTL_CT) |
						VALUEATMASK(0x40, E1000_TCTL_COLD);
	e1000[E1000_TIPG] = VALUEATMASK(10, E1000_TIPG_IPGT) |
						VALUEATMASK(8, E1000_TIPG_IPGR1) |
						VALUEATMASK(6, E1000_TIPG_IPGR2);
	e1000[E1000_RAL] = 0x12005452;
	e1000[E1000_RAH] = 0x00005634 | E1000_RAH_AV;
	//e1000[E1000_RDBAL] = V2P(rx_desc_buf);
	e1000[E1000_RDBAL] = (int)rx_desc_buf;
	e1000[E1000_RDBAH] = 0x0;
	e1000[E1000_RDLEN] = RXRING_LEN * sizeof(struct e1000_rx_desc);
	e1000[E1000_RDH] = 0x0;
	e1000[E1000_RDT] = RXRING_LEN;
	e1000[E1000_RCTL] = E1000_RCTL_EN |
						!E1000_RCTL_LPE |
						E1000_RCTL_LBM_NO |
						E1000_RCTL_RDMTS_HALF |
						E1000_RCTL_MO_0 |
						E1000_RCTL_BAM |
						E1000_RCTL_BSEX |
						E1000_RCTL_SZ_4096 |
						E1000_RCTL_SECRC;
}

/**
* @brief This method attaches the network card's function to the kernel.
* enables the pci_func, initializes its descriptors &
* maps memory for its initialization
*
* @return 0 on successfull completion
*/
int
e1000_attach(struct pci_func *pcif)
{
	pci_func_enable(pcif);
	//
	init_desc();	
	//
	e1000 = mmio_map_region(pcif->reg_base[0], pcif->reg_size[0]);
	cprintf("e1000: bar0  %x size0 %x\n", pcif->reg_base[0], pcif->reg_size[0]);
	cprintf("e1000: status %x\n",((uint*)e1000)[STATUS/4]);
	e1000_init();
	return 0;
}

/**
* @brief helper method to help with sending a packet.
*
*/
int
e1000_xmit(void * addr, size_t length){
	uint32_t tail = e1000[E1000_TDT];
	struct e1000_tx_desc * tail_desc = &tx_desc_buf[tail];
	if (tail_desc->upper.fields.status != E1000_TXD_STAT_DD)
	{
		return -1;
	}
	length = length > DATA_SIZE ? DATA_SIZE : length;
	memmove(&tx_data_buf[tail], addr, length);
	tail_desc->lower.flags.length = length;
	tail_desc->upper.fields.status = 0;
	tail_desc->lower.data |=  (E1000_TXD_CMD_RS |
							   E1000_TXD_CMD_EOP);
	e1000[E1000_TDT] = (tail + 1) % TXRING_LEN;
	return 0;
}

/**
 * @brief   Handles the transmit of a packet
 * 
 * @args    buf:	address of the buffer (packet)
 *	    len:	length of the buffer
 *
 * @return  0 on success
 *
 * memmoves the packet information to a new fd and updates the TDT
 * Uses helper method
 * @see e1000_xmit() above
 */
int
e1000_transmit(const char *buf, unsigned int len)
{
	/* Fill this in */
	e1000_xmit((void*)buf,len);
	return 0;
}

/**
 * @brief   handles the receive of a packet
 * 
 * @args    buf:	address of the buffer (packet)
 *	    len:	length of the buffer
 *
 * @return  0 on success
 *
 * takes in a pointer to a buffer and the length
 * Updates TDT with appropriate information
 * Uses helper method defined 
 * @see my_recv() below
 */
int
e1000_receive(char *buf, unsigned int len)
{
	/* Fill this in */
	my_recv((uint8_t*)buf);
	return 0;
}

/**
* @brief Helper method for recieve function
*
*/
int
my_recv(uint8_t * data){
	static uint32_t real_tail = 0;
	uint32_t tail = real_tail;
	struct e1000_rx_desc * tail_desc = &rx_desc_buf[tail];
	if (!(tail_desc->status & E1000_RXD_STAT_DD))
	{
		return -1;
	}
	size_t length = tail_desc->length;
	memmove(data, &rx_data_buf[tail], length);
	tail_desc->status = 0;
	e1000[E1000_RDT] = tail;
	real_tail = (tail + 1) % RXRING_LEN;
	return length;
}
