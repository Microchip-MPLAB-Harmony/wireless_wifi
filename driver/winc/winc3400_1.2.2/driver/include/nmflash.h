/*******************************************************************************
  WINC Flash Interface.

  File Name:
    nmflash.h

  Summary:
    WINC Flash Interface.

  Description:
    WINC Flash Interface.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2019, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/
// DOM-IGNORE-END



#ifndef __NM_FLASH_H__
#define __NM_FLASH_H__

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
INCLUDES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#include "nm_common.h"
#include "m2m_types.h"

#define FLASH_RETURN_OK                     0
#define FLASH_SECTOR_SIZE                   FLASH_SECTOR_SZ
#define FLASH_SIGNATURE                     HOST_CONTROL_FLASH_SIG
#define BACKUP_SIGNATURE                    0x424b5550

#define FLASH_MODE_FLAGS_CS                 NBIT0
#define FLASH_MODE_FLAGS_CS_SWITCH          NBIT1
#define FLASH_MODE_FLAGS_CS_SWITCH_TARGET   NBIT2
#define FLASH_MODE_FLAGS_CS_VALIDATE_IMAGE  NBIT3
#define FLASH_MODE_FLAGS_UNCHANGED          NBIT4
#define FLASH_MODE_FLAGS_DATA_IN_BACKUP     NBIT5

/*! Bit 0 of u8Flags parameter of @ref tpfDataAccessFn.\n
 *  Compare buffer against existing data, to avoid unnecessary operation. */
#define FLASH_FN_FLAGS_COMPARE_BEFORE       NBIT0
/*! Bit 1 of u8Flags parameter of @ref tpfDataAccessFn.\n
 *  Fill uninitialized portion of buffer with existing data to avoid losing it in subsequent erase.
 *  Typically not set unless @ref FLASH_FN_FLAGS_ERASE is set. */
#define FLASH_FN_FLAGS_READ_SURROUNDING     NBIT1
/*! Bit 2 of u8Flags parameter of @ref tpfDataAccessFn.\n
 *  Save buffer to a backup persistent location in case of power loss after subsequent erase.
 *  A (persistent) record of the backup status must also be kept.
 *  Typically not set unless @ref FLASH_FN_FLAGS_READ_SURROUNDING and @ref FLASH_FN_FLAGS_ERASE are both set. */
#define FLASH_FN_FLAGS_BACKUP               NBIT2
/*! Bit 3 of u8Flags parameter of @ref tpfDataAccessFn.\n
 *  Erase existing data before writing. */
#define FLASH_FN_FLAGS_ERASE                NBIT3
/*! Bit 4 of u8Flags parameter of @ref tpfDataAccessFn.\n
 *  Write buffer. */
#define FLASH_FN_FLAGS_WRITE                NBIT4
/*! Bit 5 of u8Flags parameter of @ref tpfDataAccessFn.\n
 *  Compare buffer against written data, to provide verification.
 *  Typically not set unless @ref FLASH_FN_FLAGS_WRITE is set. */
#define FLASH_FN_FLAGS_COMPARE_AFTER        NBIT5
/*! Bit 6 of u8Flags parameter of @ref tpfDataAccessFn.\n
 *  Read data to buffer. Typically this would be the only flag set, meaning read the existing data.
 *  However, if other flags are set, the read should be performed at the end. */
#define FLASH_FN_FLAGS_READ                 NBIT6

/*! Bit 0 of u8AccessOptions parameter of various function APIs.\n
 *  Request to erase existing data before writing.\n
 *  Only applies when module is providing data to the MCU application.\n
 *  If set, @ref FLASH_FN_FLAGS_ERASE will be set in subsequent call to function of type
 *  @ref tpfDataAccessFn. */
#define FLASH_ACCESS_OPTION_ERASE_FIRST         NBIT0
/*! Bit 1 of u8AccessOptions parameter of various function APIs.\n
 *  When set with @ref FLASH_ACCESS_OPTION_ERASE_FIRST, this is a request to do
 *  read-modify-erase-write (eg if MCU application is storing received data in flash).\n
 *  Only applies when module is providing data to the MCU application.\n
 *  If set, @ref FLASH_FN_FLAGS_READ_SURROUNDING may be set in subsequent call to function of type
 *  @ref tpfDataAccessFn. */
#define FLASH_ACCESS_OPTION_KEEP_SURROUNDING    NBIT1
/*! Bit 2 of u8AccessOptions parameter of various function APIs.\n
 *  When set with @ref FLASH_ACCESS_OPTION_ERASE_FIRST and @ref FLASH_ACCESS_OPTION_KEEP_SURROUNDING,
 *  this is a request to keep a persistent backup of modified contents during read-modify-erase-write.\n
 *  Only applies when module is providing data to the MCU application.\n
 *  If set, @ref FLASH_FN_FLAGS_BACKUP may be set in subsequent call to function of type
 *  @ref tpfDataAccessFn. */
#define FLASH_ACCESS_OPTION_USE_BACKUP          NBIT2
/*! Bit 3 of u8AccessOptions parameter of various function APIs.\n
 *  Request to compare new data against existing data before erasing/writing, to avoid unnecessary operations.\n
 *  Applies to data transfer in either direction.\n
 *  If set, @ref FLASH_FN_FLAGS_COMPARE_BEFORE will be set in subsequent call to function of type
 *  @ref tpfDataAccessFn. */
#define FLASH_ACCESS_OPTION_COMPARE_BEFORE      NBIT3
/*! Bit 4 of u8AccessOptions parameter of various function APIs.\n
 *  Request for byte-wise verification of write.\n
 *  Applies to data transfer in either direction.\n
 *  If set, @ref FLASH_FN_FLAGS_COMPARE_AFTER will be set in subsequent call to function of type
 *  @ref tpfDataAccessFn. */
#define FLASH_ACCESS_OPTION_COMPARE_AFTER       NBIT4
/*! When modifying WINC flash contents, the module determines most options internally. Only two
 *  options are taken from the u8AccessOptions parameter.\n
 *  When providing data to the MCU application, the module takes all options from the
 *  u8AccessOptions parameter. */
#define FLASH_ACCESS_WINC_MASK                  (FLASH_ACCESS_OPTION_COMPARE_BEFORE | FLASH_ACCESS_OPTION_COMPARE_AFTER)


typedef enum {
    CS_INITIALIZE,
    CS_INVALIDATE_RB,
    CS_VALIDATE_RB,
    CS_SWITCH,
    CS_VALIDATE_SWITCH,
    CS_GET_ACTIVE,
    CS_GET_INACTIVE,
    CS_DEINITIALIZE
}tenuCSOp;

typedef enum {
    /*
     *  Status values arranged so that status can be updated without any erase operation.
     */
    FLASH_STATUS_EMPTY          = 0xFFFFFFFF,
    FLASH_STATUS_NOT_ACTIVE     = 0xFFFFFF00,
    FLASH_STATUS_ACTIVE         = 0xFFFF0000,
    FLASH_STATUS_DONE           = 0xFF000000
}tenuFlashAccessStatus;

typedef enum {
    /*
     *  Status values arranged so that status can be updated without any erase operation.
     */
    BACKUP_STATUS_EMPTY         = 0xFFFFFFFF,
    BACKUP_STATUS_NOT_ACTIVE    = BACKUP_SIGNATURE | 0xFFFFFF00,
    BACKUP_STATUS_ACTIVE        = BACKUP_SIGNATURE,
    BACKUP_STATUS_DONE          = 0x00000000
}tenuBackupStatus;

typedef enum {
    /*
     *  Special location ID values. If top bit is not set, the location ID is interpreted as flash address.
     */
    MEM_ID_WINC_FLASH = 0x80000000,
    MEM_ID_WINC_ACTIVE,
    MEM_ID_WINC_INACTIVE,
    MEM_ID_NONE = 0xFFFFFFFF
}tenuMemId;

/*!
@enum   tenuFlashAccessItemMode

@brief  Transfer modes available for accessing items in WINC flash stores such as TLS root
        certificate store.
@see    m2m_flash_rootcert_access
 */
typedef enum {
    /*! Add an item to the relevant WINC flash store. */
    FLASH_ITEM_ADD,
    /*! Remove an item from the relevant WINC flash store. */
    FLASH_ITEM_REMOVE,
    /*! Read an item from the relevant WINC flash store, using an identifier. */
    FLASH_ITEM_READ,
    /*! Read an item from the relevant WINC flash store, using an index. */
    FLASH_ITEM_READIDX
}tenuFlashAccessItemMode;


typedef struct {
    uint32_t                u32Signature;
    tenuFlashAccessStatus   enuTransferStatus;
    uint16_t                u16AppId;
    uint8_t                 u8AccessFlags;      // These correspond bitwise to application access options.
    uint8_t                 u8ModeFlags;        // These are set internally, with reference to application mode options.
}tstrFlashAccessPersistent;
#define FLASH_SIG_SZ        (sizeof(uint32_t))
#define FLASH_STA_SZ        (sizeof(tenuFlashAccessStatus))
#define FLASH_SIG_STA_SZ    (FLASH_SIG_SZ+FLASH_STA_SZ)

typedef struct {
    tstrFlashAccessPersistent   strPersistentInfo;
    tpfDataAccessFn             pfDestinationFn;
    tpfDataAccessFn             pfSourceFn;
    uint32_t                    u32Size;
}tstrFlashAccess;

typedef struct {
    tenuBackupStatus        enuTransferStatus;
    uint32_t                u32DestinationAddr;
    uint32_t                u32SourceAddr;
    uint32_t                u32Size;
}tstrBackup;
#define FLASH_BACKUP_STA_SZ         sizeof(tenuBackupStatus)
#define FLASH_BACKUP_STORE_SZ       (2*sizeof(tstrBackup))
#define FLASH_BACKUP_STORE_OFFSET   (HOST_CONTROL_FLASH_OFFSET + HOST_CONTROL_FLASH_SZ - FLASH_BACKUP_STORE_SZ)

/*!
@struct \
    tstrDataAccessInitParams
@brief
    This structure contains parameters for initializing a local data access (read, erase or write).

@see    tpfDataAccessFn.
@see    FLASH_FN_FLAGS_COMPARE_BEFORE
@see    FLASH_FN_FLAGS_READ_SURROUNDING
@see    FLASH_FN_FLAGS_BACKUP
@see    FLASH_FN_FLAGS_ERASE
@see    FLASH_FN_FLAGS_WRITE
@see    FLASH_FN_FLAGS_COMPARE_AFTER
@see    FLASH_FN_FLAGS_READ
 */
typedef struct {
    /*! Total size of data to be accessed in data location. \n
     *  This field is set by the module in a call to @ref tpfDataAccessFn. */
    uint32_t    u32TotalSize;
    /*! Flags indicating type of data access. \n
     *  This field is set by the module in a call to @ref tpfDataAccessFn. */
    uint8_t     u8Flags;
    /*! Block size, to assist with alignment. \n
     *  This field can be set by the local memory access function @ref tpfDataAccessFn. Defaults to 0 if not set.
     *  Recommended values are:\n
     *  - 4096 if accessing flash memory with erase block size <= 4096.\n
     *  - 0 if accessing normal memory. */
    uint32_t    u32AlignmentSize;
    /*! Offset of access start address, relative to u32AlignmentSize. \n
     *  This field can be set by the local memory access function @ref tpfDataAccessFn. The field is ignored if u32AlignmentSize < 2. */
    uint32_t    u32StartAlignment;
}tstrDataAccessInitParams;

/*!
@struct \
    tstrDataAccessParams
@brief
    This structure contains data for local data access (read, erase or write).
@see    tpfDataAccessFn.
 */
typedef struct {
    /*! Buffer to be written to or read from. */
    uint8_t     *pu8Buf;
    /*! Total size of the buffer. */
    uint32_t    u32BufSize;
    /* Offset of data within the buffer. */
    uint32_t    u32DataOffset;
    /* Size of data to be written or read. */
    uint32_t    u32DataSize;
}tstrDataAccessParams;

extern uint16_t gu16LastAccessId;
extern uint8_t  gu8Success;
extern uint8_t  gu8Changed;
extern uint8_t  gu8Init;
extern uint8_t  gu8Reset;

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
FUNCTION PROTOTYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

int8_t winc_flash_write_verify(uint8_t *pu8Buf, uint32_t u32Offset, uint32_t u32Size);
void set_internal_info(tpfDataAccessFn *ppfFn, uint32_t u32LocationId);
void set_internal_info_ptr(tpfDataAccessFn *ppfFn, uint8_t *pu8Ptr);
uint8_t is_internal_info(tpfDataAccessFn pfFn);
int8_t recover_backup(void);
int8_t prepare_backup(uint32_t u32Target);
int8_t image_get_target(uint8_t *pu8Target);
int8_t rootcert_get_size(tstrRootCertEntryHeader *pstrHdr, uint16_t *pu16Size);
int8_t rootcert_access(tenuFlashAccessItemMode enuMode, tstrRootCertEntryHeader *pstrReferenceHdr, uint16_t *pu16EntrySize, uint8_t *pu8Buff, uint32_t *pu32Offset);
int8_t transfer_run(tstrFlashAccess *pstrFlashAccess);


#endif /* __NM_FLASH_H__ */
