/*******************************************************************************
  File Name:
    nmflash.c

  Summary:
    WINC Flash Interface.

  Description:
    WINC Flash Interface.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
INCLUDES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#include "m2m_flash.h"
#include "nmflash.h"
#include "spi_flash.h"
#include "nmdrv.h"

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
GLOBALS
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
static uint32_t     gu32LocationId = MEM_ID_NONE;
static uint8_t      *gpu8Location = NULL;

uint16_t            gu16LastAccessId = 0;
uint8_t             gu8Success = 0;
uint8_t             gu8Changed = 0;
uint8_t             gu8Init = 0;
uint8_t             gu8Reset = 0;


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
FUNCTIONS
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

static int8_t winc_flash_compare(uint8_t *pu8Buf, uint32_t u32Offset, uint32_t u32Size)
{
    int8_t ret = M2M_SUCCESS;
    uint8_t buf[128];
    uint32_t offset = 0;

    while (offset < u32Size)
    {
        uint32_t chunk_sz = sizeof(buf);
        if (chunk_sz > u32Size - offset)
            chunk_sz = u32Size - offset;
        ret = spi_flash_read(buf, u32Offset + offset, chunk_sz);
        if (ret != M2M_SUCCESS)
            break;
        ret = memcmp(buf, pu8Buf + offset, chunk_sz);
        if (ret != 0)
            break;
        offset += chunk_sz;
    }
    return ret;
}

int8_t winc_flash_write_verify(uint8_t *pu8Buf, uint32_t u32Offset, uint32_t u32Size)
{
    int8_t  ret = M2M_ERR_FAIL;
    uint8_t count = 20;

    while ((ret != M2M_SUCCESS) && (count-- > 0))
    {
        ret = spi_flash_write(pu8Buf, u32Offset, u32Size);
        if (ret == M2M_SUCCESS)
            ret = winc_flash_compare(pu8Buf, u32Offset, u32Size);
    }
    return ret;
}
static int8_t set_changed_flag(tstrFlashAccessPersistent *pstrPersistentInfo)
{
    int8_t  ret = FLASH_RETURN_OK;
    if (pstrPersistentInfo->u8ModeFlags & FLASH_MODE_FLAGS_UNCHANGED)
    {
        pstrPersistentInfo->u8ModeFlags &= ~FLASH_MODE_FLAGS_UNCHANGED;
        if (winc_flash_write_verify((uint8_t*)pstrPersistentInfo, HOST_CONTROL_FLASH_OFFSET, sizeof(tstrFlashAccessPersistent)) == M2M_SUCCESS)
            gu8Changed = 1;
        else
            ret = FLASH_ERR_WINC_ACCESS;
    }
    return ret;
}

static uint8_t crc7(uint8_t crc, const uint8_t *buff, uint16_t len)
{
    uint8_t reg = crc;
    uint16_t i;
    for(i = 0; i < len; i++)
    {
        uint16_t g;
        for(g = 0; g < 8; g++)
        {
            uint8_t inv = (((buff[i] << g) & 0x80) >> 7) ^ ((reg >> 6) & 1);
            reg = ((reg << 1) & 0x7f) ^ (9 * inv);
        }
    }
    return reg;
}
static int8_t read_control_sector(tstrOtaControlSec *pstrControlSec, uint32_t u32Offset)
{
    int8_t  s8Ret = spi_flash_read((uint8_t*)pstrControlSec, u32Offset, sizeof(tstrOtaControlSec));
    if (s8Ret == M2M_SUCCESS)
    {
        if (pstrControlSec->u32OtaMagicValue != OTA_MAGIC_VALUE)
            s8Ret = M2M_ERR_FAIL;
        if (pstrControlSec->u32OtaControlSecCrc != crc7(0x7f, (uint8_t*)pstrControlSec, sizeof(tstrOtaControlSec) - 4))
            s8Ret = M2M_ERR_FAIL;
    }
    return s8Ret;
}
static int8_t update_control_sector(tstrOtaControlSec *pstrControlSec)
{
    int8_t ret = M2M_ERR_FAIL;

    ret = spi_flash_erase(M2M_BACKUP_FLASH_OFFSET, M2M_BACKUP_FLASH_SZ);
    if (ret == M2M_SUCCESS)
    {
        pstrControlSec->u32OtaSequenceNumber++;
        pstrControlSec->u32OtaControlSecCrc = crc7(0x7f, (uint8_t*)pstrControlSec, sizeof(tstrOtaControlSec) - 4);
        ret = winc_flash_write_verify((uint8_t*)pstrControlSec, M2M_BACKUP_FLASH_OFFSET, sizeof(tstrOtaControlSec));
        if (ret == M2M_SUCCESS)
        {
            ret = spi_flash_erase(M2M_CONTROL_FLASH_OFFSET, M2M_CONTROL_FLASH_SZ);
            if (ret == M2M_SUCCESS)
            {
                pstrControlSec->u32OtaSequenceNumber++;
                pstrControlSec->u32OtaControlSecCrc = crc7(0x7f, (uint8_t*)pstrControlSec, sizeof(tstrOtaControlSec) - 4);
                ret = winc_flash_write_verify((uint8_t*)pstrControlSec, M2M_CONTROL_FLASH_OFFSET, sizeof(tstrOtaControlSec));
            }
        }
    }
    return ret;
}
static int8_t access_control_sector(tenuCSOp enuOp, uint32_t *param)
{
    static  tstrOtaControlSec strControlSec = {0};
    int8_t  s8Ret = M2M_SUCCESS;
    uint8_t bUpdate = false;

    if ((enuOp != CS_INITIALIZE) && (strControlSec.u32OtaMagicValue != OTA_MAGIC_VALUE))
    {
        if (param != NULL)
            *param = 0;
        return M2M_ERR_FAIL;
    }

    switch (enuOp)
    {
    case CS_INITIALIZE:
        s8Ret = read_control_sector(&strControlSec, M2M_CONTROL_FLASH_OFFSET);
        if (s8Ret != M2M_SUCCESS)
        {
            s8Ret = read_control_sector(&strControlSec, M2M_BACKUP_FLASH_OFFSET);
            if (s8Ret == M2M_SUCCESS)
            {
                /*
                 *  Reinstate the control sector from backup.
                 */
                s8Ret = spi_flash_erase(M2M_CONTROL_FLASH_OFFSET, M2M_CONTROL_FLASH_SZ);
                if (s8Ret == M2M_SUCCESS)
                    s8Ret = winc_flash_write_verify((uint8_t*)&strControlSec, M2M_CONTROL_FLASH_OFFSET, sizeof(tstrOtaControlSec));
            }
        }
        break;
    case CS_INVALIDATE_RB:
        // Update trashes the backup sector, so we need to avoid unnecessary updates.
        if (strControlSec.u32OtaRollbackImageValidStatus != OTA_STATUS_INVALID)
        {
            strControlSec.u32OtaRollbackImageValidStatus = OTA_STATUS_INVALID;
            bUpdate = true;
        }
        break;
    case CS_VALIDATE_RB:
        // Update trashes the backup sector, so we need to avoid unnecessary updates.
        if (strControlSec.u32OtaRollbackImageValidStatus != OTA_STATUS_VALID)
        {
            strControlSec.u32OtaRollbackImageValidStatus = OTA_STATUS_VALID;
            bUpdate = true;
        }
        break;
    case CS_VALIDATE_SWITCH:
        strControlSec.u32OtaRollbackImageValidStatus = OTA_STATUS_VALID;
        // intentional fallthrough.
    case CS_SWITCH:
        if (strControlSec.u32OtaRollbackImageValidStatus == OTA_STATUS_VALID)
        {
            uint32_t tmp = strControlSec.u32OtaCurrentWorkingImagOffset;
            strControlSec.u32OtaCurrentWorkingImagOffset = strControlSec.u32OtaRollbackImageOffset;
            strControlSec.u32OtaRollbackImageOffset = tmp;
            bUpdate = true;
        }
        else
            s8Ret = M2M_ERR_FAIL;
        break;
    case CS_GET_ACTIVE:
        if (param == NULL)
            s8Ret = M2M_ERR_FAIL;
        else
            *param = strControlSec.u32OtaCurrentWorkingImagOffset;
        break;
    case CS_GET_INACTIVE:
        if (param == NULL)
            s8Ret = M2M_ERR_FAIL;
        else
            *param = strControlSec.u32OtaRollbackImageOffset;
        break;
    case CS_DEINITIALIZE:
        memset((uint8_t*)&strControlSec, 0, sizeof(tstrOtaControlSec));
        break;
    default:
        s8Ret = M2M_ERR_FAIL;
    }
    if (bUpdate)
    {
        s8Ret = update_control_sector(&strControlSec);
        M2M_INFO("CS update:%d %s\r\n", enuOp, (s8Ret==M2M_SUCCESS)?"":"Failed");
    }
    return s8Ret;
}
static int8_t local_access_ptr(tenuFlashDataFnCtl enuCtl, void *pvStr)
{
    int8_t          s8Ret = M2M_ERR_FAIL;
    static uint8_t  *pu8Location = NULL;
    static uint8_t  u8Flags = 0;

    switch (enuCtl)
    {
    case FLASH_DATA_FN_INITIALIZE:
        {
            tstrDataAccessInitParams *init_params = (tstrDataAccessInitParams*)pvStr;
            u8Flags = init_params->u8Flags;
            pu8Location = gpu8Location;
            s8Ret = M2M_SUCCESS;
        }
        break;
    case FLASH_DATA_FN_DATA:
        if (pu8Location != NULL)
        {
            tstrDataAccessParams *params = (tstrDataAccessParams*)pvStr;
            if (u8Flags & FLASH_FN_FLAGS_WRITE)
                memcpy(pu8Location, params->pu8Buf + params->u32DataOffset, params->u32DataSize);
            if (u8Flags & FLASH_FN_FLAGS_READ)
                memcpy(params->pu8Buf + params->u32DataOffset, pu8Location, params->u32DataSize);
            pu8Location += params->u32DataSize;
            s8Ret = M2M_SUCCESS;
        }
        break;
    case FLASH_DATA_FN_TERMINATE:
    case FLASH_DATA_FN_COMPLETE:
        u8Flags = 0;
        pu8Location = NULL;
        s8Ret = M2M_SUCCESS;
        break;
    }
    return s8Ret;
}
static int8_t winc_flash_access(tenuFlashDataFnCtl enuCtl, void *pvStr)
{
    int8_t          s8Ret = M2M_ERR_FAIL;
    static uint32_t u32CurrentAddr = 0;
    static uint8_t  u8Flags = 0;

    switch (enuCtl)
    {
    case FLASH_DATA_FN_INITIALIZE:
        {
            tstrDataAccessInitParams *init_params = (tstrDataAccessInitParams*)pvStr;
            M2M_INFO("FA Init: 0x%lx Fg 0x%02x Sz 0x%lx\r\n", gu32LocationId, init_params->u8Flags, init_params->u32TotalSize);
            u8Flags = init_params->u8Flags;
            switch (gu32LocationId)
            {
            case MEM_ID_WINC_FLASH:
                u32CurrentAddr = 0;
                break;
            case MEM_ID_WINC_ACTIVE:
                s8Ret = access_control_sector(CS_GET_ACTIVE, &u32CurrentAddr);
                break;
            case MEM_ID_WINC_INACTIVE:
                s8Ret = access_control_sector(CS_GET_INACTIVE, &u32CurrentAddr);
                /*  If we're about to write to the inactive partition, mark it as invalid. */
                if ((s8Ret == M2M_SUCCESS) && (u8Flags & (FLASH_FN_FLAGS_WRITE | FLASH_FN_FLAGS_ERASE)))
                    s8Ret = access_control_sector(CS_INVALIDATE_RB, NULL);
                break;
            case MEM_ID_NONE:
                s8Ret = M2M_ERR_FAIL;
                break;
            default:
                u32CurrentAddr = gu32LocationId;
                break;
            }
            if (u8Flags & FLASH_FN_FLAGS_READ_SURROUNDING)
            {
                init_params->u32AlignmentSize = FLASH_SECTOR_SZ;
                init_params->u32StartAlignment = u32CurrentAddr & (FLASH_SECTOR_SZ - 1);
            }
            s8Ret = M2M_SUCCESS;
        }
        break;
    case FLASH_DATA_FN_DATA:
        {
            tstrDataAccessParams *params = (tstrDataAccessParams*)pvStr;
            if (u8Flags & FLASH_FN_FLAGS_COMPARE_BEFORE)
            {
                M2M_INFO("c-");
                // If contents match already, return success
                s8Ret = winc_flash_compare(params->pu8Buf + params->u32DataOffset, u32CurrentAddr, params->u32DataSize);
                if (s8Ret == M2M_SUCCESS)
                    goto END;
            }
            if (u8Flags & FLASH_FN_FLAGS_READ_SURROUNDING)
            {
                uint32_t prefill_sz = params->u32DataOffset;
                uint32_t postfill_sz = params->u32BufSize - (params->u32DataOffset + params->u32DataSize);
                if (prefill_sz > 0)
                {
                    M2M_INFO("r-");
                    s8Ret = spi_flash_read(params->pu8Buf, u32CurrentAddr - prefill_sz, prefill_sz);
                    if (s8Ret != M2M_SUCCESS)
                        goto END;
                }
                if (postfill_sz > 0)
                {
                    M2M_INFO("-r");
                    s8Ret = spi_flash_read(params->pu8Buf + params->u32BufSize - postfill_sz, u32CurrentAddr + params->u32DataSize, postfill_sz);
                    if (s8Ret != M2M_SUCCESS)
                        goto END;
                }
            }
            if (u8Flags & FLASH_FN_FLAGS_BACKUP)
            {
                if (params->u32BufSize > params->u32DataSize)
                {
                    M2M_INFO("b");
                    s8Ret = winc_flash_write_verify(params->pu8Buf, M2M_BACKUP_FLASH_OFFSET, params->u32BufSize);
                    if (s8Ret == M2M_SUCCESS)
                        s8Ret = prepare_backup(u32CurrentAddr - params->u32DataOffset);
                    if (s8Ret != M2M_SUCCESS)
                        goto END;
                }
            }
            if (u8Flags & FLASH_FN_FLAGS_ERASE)
            {
                M2M_INFO("e");
                s8Ret = spi_flash_erase(u32CurrentAddr, params->u32DataSize);
                if (s8Ret != M2M_SUCCESS)
                    goto END;
            }
            if (u8Flags & FLASH_FN_FLAGS_WRITE)
            {
                M2M_INFO("W");
                if (u8Flags & FLASH_FN_FLAGS_READ_SURROUNDING)
                    s8Ret = spi_flash_write(params->pu8Buf, u32CurrentAddr - params->u32DataOffset, params->u32BufSize);
                else
                    s8Ret = spi_flash_write(params->pu8Buf + params->u32DataOffset, u32CurrentAddr, params->u32DataSize);
                if (s8Ret != M2M_SUCCESS)
                    goto END;
            }
            if (u8Flags & FLASH_FN_FLAGS_COMPARE_AFTER)
            {
                M2M_INFO("-c");
                // If contents do not match, return failure
                if (u8Flags & FLASH_FN_FLAGS_READ_SURROUNDING)
                    s8Ret = winc_flash_compare(params->pu8Buf, u32CurrentAddr - params->u32DataOffset, params->u32BufSize);
                else
                    s8Ret = winc_flash_compare(params->pu8Buf + params->u32DataOffset, u32CurrentAddr, params->u32DataSize);
                if (s8Ret != M2M_SUCCESS)
                    goto END;
            }
            if (u8Flags & FLASH_FN_FLAGS_READ)
            {
                M2M_INFO("R");
                s8Ret = spi_flash_read(params->pu8Buf + params->u32DataOffset, u32CurrentAddr, params->u32DataSize);
                if (s8Ret != M2M_SUCCESS)
                    goto END;
            }
END:
            u32CurrentAddr += params->u32DataSize;
        }
        break;
    case FLASH_DATA_FN_TERMINATE:
    case FLASH_DATA_FN_COMPLETE:
        M2M_INFO(" FA End 0x%lx\r\n", u32CurrentAddr);
        u8Flags = 0;
        u32CurrentAddr = 0;
        s8Ret = M2M_SUCCESS;
        break;
    }
    return s8Ret;
}

void set_internal_info_ptr(tpfDataAccessFn *ppfFn, uint8_t *pu8Ptr)
{
    if (ppfFn != NULL)
        *ppfFn = local_access_ptr;
    gpu8Location = pu8Ptr;
}
void set_internal_info(tpfDataAccessFn *ppfFn, uint32_t u32LocationId)
{
    if (ppfFn != NULL)
        *ppfFn = winc_flash_access;
    gu32LocationId = u32LocationId;
}
uint8_t is_internal_info(tpfDataAccessFn pfFn)
{
    if (pfFn == winc_flash_access)
        return true;
    return false;
}
int8_t recover_backup(void)
{
    int8_t              ret = FLASH_RETURN_OK;
    uint32_t            u32BackupAddr = FLASH_BACKUP_STORE_OFFSET;
    tstrBackup          strBackup;

    while (u32BackupAddr < FLASH_BACKUP_STORE_OFFSET + FLASH_BACKUP_STORE_SZ)
    {
        int8_t status = spi_flash_read((uint8_t*)&strBackup, u32BackupAddr, sizeof(tstrBackup));
        if ((status == M2M_SUCCESS) && (strBackup.enuTransferStatus == BACKUP_STATUS_ACTIVE))
        {
            uint8_t *pu8Buff = malloc(strBackup.u32Size);
            if (pu8Buff == NULL)
            {
                ret = FLASH_ERR_INTERNAL;
                goto ERR;
            }
            status = spi_flash_read(pu8Buff, strBackup.u32SourceAddr, strBackup.u32Size);
            if (status == M2M_SUCCESS)
            {
                status = spi_flash_erase(strBackup.u32DestinationAddr, strBackup.u32Size);
                if (status == M2M_SUCCESS)
                {
                    status = winc_flash_write_verify(pu8Buff, strBackup.u32DestinationAddr, strBackup.u32Size);
                    if (status == M2M_SUCCESS)
                    {
                        strBackup.enuTransferStatus = BACKUP_STATUS_DONE;
                        status = winc_flash_write_verify((uint8_t*)&strBackup.enuTransferStatus, u32BackupAddr, FLASH_BACKUP_STA_SZ);
                    }
                }
            }
            free(pu8Buff);
        }
        if (status != M2M_SUCCESS)
        {
            ret = FLASH_ERR_WINC_ACCESS;
            goto ERR;
        }
        u32BackupAddr += sizeof(tstrBackup);
    }
ERR:
    return ret;
}
int8_t prepare_backup(uint32_t u32Target)
{
    int8_t              s8Ret = M2M_ERR_FAIL;
    uint32_t            u32BackupAddr = FLASH_BACKUP_STORE_OFFSET;
    tenuBackupStatus    enuStatus = BACKUP_STATUS_EMPTY;

    s8Ret = spi_flash_read((uint8_t*)&enuStatus, u32BackupAddr, sizeof(enuStatus));
    if ((s8Ret != M2M_SUCCESS) || (enuStatus != BACKUP_STATUS_EMPTY))
    {
        u32BackupAddr += sizeof(tstrBackup);
        s8Ret = spi_flash_read((uint8_t*)&enuStatus, u32BackupAddr, sizeof(enuStatus));
        if ((s8Ret == M2M_SUCCESS) && (enuStatus != BACKUP_STATUS_EMPTY))
            s8Ret = M2M_ERR_FAIL;
    }
    if (s8Ret == M2M_SUCCESS)
    {
        tstrBackup  strBackup = {BACKUP_STATUS_NOT_ACTIVE, u32Target, M2M_BACKUP_FLASH_OFFSET, M2M_BACKUP_FLASH_SZ};
        s8Ret = winc_flash_write_verify((uint8_t*)&strBackup.enuTransferStatus, u32BackupAddr, FLASH_BACKUP_STA_SZ);
        if (s8Ret == M2M_SUCCESS)
        {
            s8Ret = winc_flash_write_verify((uint8_t*)&strBackup, u32BackupAddr, sizeof(strBackup));
            if (s8Ret == M2M_SUCCESS)
            {
                strBackup.enuTransferStatus = BACKUP_STATUS_ACTIVE;
                s8Ret = winc_flash_write_verify((uint8_t*)&strBackup.enuTransferStatus, u32BackupAddr, FLASH_BACKUP_STA_SZ);
            }
        }
    }
    return s8Ret;
}
int8_t image_get_target(uint8_t *pu8Target)
{
    int8_t      s8Ret = M2M_ERR_FAIL;
    uint32_t    u32OffsetActive = 0;
    uint32_t    u32OffsetInactive = 0;

    s8Ret = access_control_sector(CS_INITIALIZE, NULL);
    if (s8Ret == M2M_SUCCESS)
    {
        s8Ret = access_control_sector(CS_GET_ACTIVE, &u32OffsetActive);
        if (s8Ret == M2M_SUCCESS)
        {
            s8Ret = access_control_sector(CS_GET_INACTIVE, &u32OffsetInactive);
            if (s8Ret == M2M_SUCCESS)
                *pu8Target = (u32OffsetInactive > u32OffsetActive) ? 1 : 0;
        }
        access_control_sector(CS_DEINITIALIZE, NULL);
    }
    return s8Ret;
}
int8_t rootcert_get_size(tstrRootCertEntryHeader *pstrHdr, uint16_t *pu16Size)
{
    int8_t s8Ret = M2M_ERR_FAIL;
    if ((pstrHdr == NULL) || (pu16Size == NULL))
        goto ERR;

    /* Set default size out to maximum. */
    *pu16Size = 0xFFFF;
    switch (pstrHdr->strPubKey.u32PubKeyType)
    {
    case ROOT_CERT_PUBKEY_RSA:
        if (pstrHdr->strPubKey.strRsaKeyInfo.u16NSz > M2M_TLS_ROOTCER_FLASH_SZ)
            goto ERR;
        if (pstrHdr->strPubKey.strRsaKeyInfo.u16ESz > M2M_TLS_ROOTCER_FLASH_SZ)
            goto ERR;
        *pu16Size = sizeof(tstrRootCertEntryHeader) + ((pstrHdr->strPubKey.strRsaKeyInfo.u16NSz + 0x3) & ~0x3) + ((pstrHdr->strPubKey.strRsaKeyInfo.u16ESz + 0x3) & ~0x3);
        s8Ret = M2M_SUCCESS;
        break;
    case ROOT_CERT_PUBKEY_ECDSA:
        if (pstrHdr->strPubKey.strEcsdaKeyInfo.u16KeySz > M2M_TLS_ROOTCER_FLASH_SZ)
            goto ERR;
        *pu16Size = sizeof(tstrRootCertEntryHeader) + ((pstrHdr->strPubKey.strEcsdaKeyInfo.u16KeySz + 0x3) & ~0x3) * 2;
        s8Ret = M2M_SUCCESS;
        break;
    case 0xFFFFFFFF:
        // Invalid. May indicate end of list. Fail with size set to 0.
        *pu16Size = 0;
        break;
    default:
        // Corrupt header.
        break;
    }
ERR:
    return s8Ret;
}
int8_t rootcert_access(tenuFlashAccessItemMode enuMode, tstrRootCertEntryHeader *pstrReferenceHdr, uint16_t *pu16EntrySize, uint8_t *pu8Buff, uint32_t *pu32Offset)
{
    int8_t                  ret = FLASH_RETURN_OK;
    int8_t                  status = M2M_SUCCESS;
    uint8_t                 au8RootCertSig[] = M2M_TLS_ROOTCER_FLASH_SIG;
    tstrRootCertEntryHeader strEntryHeader;
    /* Previous version used 0-identifiers to indicate removed entries. Use last 20 bytes of pu8Buff to help us check for them. */
    uint8_t                 *pu8Zero = pu8Buff + M2M_TLS_ROOTCER_FLASH_SZ - sizeof(pstrReferenceHdr->au8SHA1NameHash);
    uint32_t                u32StoreOffset = 0;
    uint32_t                u32Entries = 0;

    memset(pu8Zero, 0, sizeof(pstrReferenceHdr->au8SHA1NameHash));
    /* Use pu8Buff to read signature. */
    status = spi_flash_read(pu8Buff, M2M_TLS_ROOTCER_FLASH_OFFSET, sizeof(au8RootCertSig));
    if ((status != M2M_SUCCESS) || memcmp(pu8Buff, au8RootCertSig, sizeof(au8RootCertSig)))
    {
        /*
         *  Root certificate section is not initialized. We could try to initialize it
         *  here, but for now just fail.
         */
        ret = FLASH_ERR_WINC_ACCESS;
        goto ERR;
    }

    /*
     *  By default assume we'll get to the end of the flash store without finding what we need
     *  (matching entry or space to add new entry). If we break while loop before reaching end of
     *  store then we'll change ret accordingly. */
    if (enuMode == FLASH_ITEM_ADD)
        ret = FLASH_ERR_SIZE;
    else
        ret = FLASH_ERR_WINC_CONFLICT;

    u32StoreOffset = *pu32Offset = sizeof(tstrRootCertFlashHeader);
    while (u32StoreOffset + sizeof(tstrRootCertEntryHeader) < M2M_TLS_ROOTCER_FLASH_SZ)
    {
        uint16_t  u16EntrySize = 0;
        status = spi_flash_read((uint8_t*)&strEntryHeader, M2M_TLS_ROOTCER_FLASH_OFFSET + u32StoreOffset, sizeof(tstrRootCertEntryHeader));
        if (status != M2M_SUCCESS)
        {
            ret = FLASH_ERR_WINC_ACCESS;
            break;
        }
        status = rootcert_get_size(&strEntryHeader, &u16EntrySize);
        if (status != M2M_SUCCESS)
        {
            // Found the end of the list. We are done. If we are adding an entry, check the space here.
            if ((enuMode == FLASH_ITEM_ADD) && ((*pu32Offset + *pu16EntrySize) <= M2M_TLS_ROOTCER_FLASH_SZ))
            {
                u32Entries++;
                ret = FLASH_RETURN_OK;
            }
            break;
        }

        // If we are here we know that u32EntrySize is sane.
        if (memcmp(pu8Zero, (uint8_t*)pstrReferenceHdr, sizeof(strEntryHeader.au8SHA1NameHash)))
        {
            // Entry is not empty.
            status = spi_flash_read(pu8Buff + *pu32Offset, M2M_TLS_ROOTCER_FLASH_OFFSET + u32StoreOffset, u16EntrySize);
            if (status != M2M_SUCCESS)
            {
                ret = FLASH_ERR_WINC_ACCESS;
                break;
            }
            if (enuMode == FLASH_ITEM_READIDX)
            {
                if (u32Entries == *(uint32_t*)pstrReferenceHdr)
                {
                    // Found entry. pu16EntrySize is used to output size.
                    *pu16EntrySize = u16EntrySize;
                    ret = FLASH_RETURN_OK;
                    break;
                }
            }
            else if (!memcmp(strEntryHeader.au8SHA1NameHash, (uint8_t*)pstrReferenceHdr, sizeof(strEntryHeader.au8SHA1NameHash)))
            {
                if (enuMode == FLASH_ITEM_ADD)
                {
                    // Found a match. Cannot add.
                    ret = FLASH_ERR_WINC_CONFLICT;
                    break;
                }
                if (enuMode == FLASH_ITEM_READ)
                {
                    // Found a match. pu16EntrySize is used to output size.
                    *pu16EntrySize = u16EntrySize;
                    ret = FLASH_RETURN_OK;
                    break;
                }
                if (enuMode == FLASH_ITEM_REMOVE)
                {
                    // Found a match. Continue, to complete entry count.
                    ret = FLASH_RETURN_OK;
                    // Cancel out increment of u32BuffOffset.
                    *pu32Offset -= u16EntrySize;
                }
            }
            *pu32Offset += u16EntrySize;
            u32Entries++;
        }
        u32StoreOffset += u16EntrySize;
    }
    if (ret == FLASH_RETURN_OK)
        ((tstrRootCertFlashHeader*)pu8Buff)->u32nCerts = u32Entries;
ERR:
    return ret;
}

int8_t transfer_run(tstrFlashAccess *pstrFlashAccess)
{
    /*
     *  Errors before start of first transfer will be reported as parameter errors.
     *  This means the information is insufficient to allow us to begin.
     */
    int8_t                  ret = FLASH_RETURN_OK;
    int8_t                  status = M2M_ERR_FAIL;

    tpfDataAccessFn             pfWriteFn = pstrFlashAccess->pfDestinationFn;
    tpfDataAccessFn             pfReadFn = pstrFlashAccess->pfSourceFn;
    tstrDataAccessInitParams    read_init_params = {pstrFlashAccess->u32Size, FLASH_FN_FLAGS_READ, 0, 0};
    tstrDataAccessInitParams    write_init_params = {pstrFlashAccess->u32Size, FLASH_FN_FLAGS_WRITE, 0, 0};
    uint32_t                    u32BytesTransferred = 0;
    uint32_t                    u32BytesRemaining = pstrFlashAccess->u32Size;
    uint8_t                     *pu8Buff = NULL;

    if (pstrFlashAccess->strPersistentInfo.u8AccessFlags & FLASH_ACCESS_OPTION_COMPARE_BEFORE)
        write_init_params.u8Flags |= FLASH_FN_FLAGS_COMPARE_BEFORE;
    if (pstrFlashAccess->strPersistentInfo.u8AccessFlags & FLASH_ACCESS_OPTION_ERASE_FIRST)
    {
        write_init_params.u8Flags |= FLASH_FN_FLAGS_ERASE;
        if (pstrFlashAccess->strPersistentInfo.u8AccessFlags & FLASH_ACCESS_OPTION_KEEP_SURROUNDING)
        {
            write_init_params.u8Flags |= FLASH_FN_FLAGS_READ_SURROUNDING;
            if (pstrFlashAccess->strPersistentInfo.u8AccessFlags & FLASH_ACCESS_OPTION_USE_BACKUP)
                write_init_params.u8Flags |= FLASH_FN_FLAGS_BACKUP;
        }
    }
    if (pstrFlashAccess->strPersistentInfo.u8AccessFlags & FLASH_ACCESS_OPTION_COMPARE_AFTER)
        write_init_params.u8Flags |= FLASH_FN_FLAGS_COMPARE_AFTER;

    if (pstrFlashAccess->strPersistentInfo.u8ModeFlags & FLASH_MODE_FLAGS_DATA_IN_BACKUP)
    {
        if (prepare_backup(gu32LocationId) != M2M_SUCCESS)
        {
            ret = FLASH_ERR_WINC_ACCESS;
            goto ERR;
        }
        set_changed_flag(&pstrFlashAccess->strPersistentInfo);
        ret = recover_backup();
        if (ret < 0)
            goto ERR;
    }
    /*
     *  Initialize control sector. Even if we don't need to access it, this at
     *  least ensures that the control sector is not relying on the flash backup sector.
     */
    status = access_control_sector(CS_INITIALIZE, NULL);
    if (status != M2M_SUCCESS && (pstrFlashAccess->strPersistentInfo.u8ModeFlags & FLASH_MODE_FLAGS_CS))
    {
        ret = FLASH_ERR_WINC_ACCESS;
        goto ERR;
    }

    if (pfReadFn != NULL)
    {
        /* Prepare for read. */

        status = pfReadFn(FLASH_DATA_FN_INITIALIZE, &read_init_params);
        if (status != M2M_SUCCESS)
        {
            if (is_internal_info(pfReadFn))
                ret = FLASH_ERR_WINC_ACCESS;
            else
                ret = FLASH_ERR_LOCAL_ACCESS;
            pfReadFn(FLASH_DATA_FN_TERMINATE, NULL);
            goto ERR;
        }
    }
    if (pfWriteFn != NULL)
    {
        /* Prepare for write. */
        status = pfWriteFn(FLASH_DATA_FN_INITIALIZE, &write_init_params);
        if (status != M2M_SUCCESS)
        {
            if (is_internal_info(pfWriteFn))
                ret = FLASH_ERR_WINC_ACCESS;
            else
                ret = FLASH_ERR_LOCAL_ACCESS;
            goto TERMINATE;
        }
    }
    if (u32BytesRemaining > 0)
    {
        if (u32BytesRemaining > FLASH_SECTOR_SIZE)
            pu8Buff = malloc(FLASH_SECTOR_SIZE);
        else if (write_init_params.u32AlignmentSize > 1)
            pu8Buff = malloc(write_init_params.u32AlignmentSize);
        else
            pu8Buff = malloc(u32BytesRemaining);
        if (pu8Buff == NULL)
        {
            ret = FLASH_ERR_INTERNAL;
            goto TERMINATE;
        }
    }

    while (u32BytesRemaining > 0)
    {
        tstrDataAccessParams    params = {pu8Buff, FLASH_SECTOR_SIZE, 0, FLASH_SECTOR_SIZE};

        if (u32BytesTransferred > 0)
            write_init_params.u32StartAlignment = 0;
        if (write_init_params.u32AlignmentSize > 1)
        {
            params.u32DataOffset = write_init_params.u32StartAlignment & (write_init_params.u32AlignmentSize-1);
            params.u32DataSize = write_init_params.u32AlignmentSize - params.u32DataOffset;
        }

        if (params.u32DataSize > u32BytesRemaining)
            params.u32DataSize = u32BytesRemaining;

        /* Read. */
        if (pfReadFn != NULL)
        {
            status = pfReadFn(FLASH_DATA_FN_DATA, &params);
            if (status != M2M_SUCCESS)
            {
                if (is_internal_info(pfReadFn))
                    ret = FLASH_ERR_WINC_ACCESS;
                else
                    ret = FLASH_ERR_LOCAL_ACCESS;
                break;
            }
        }

        /* Write. */
        if (pfWriteFn != NULL)
        {
            if (is_internal_info(pfWriteFn))
            {
                ret = set_changed_flag(&pstrFlashAccess->strPersistentInfo);
                if (ret < 0)
                    break;
            }
            status = pfWriteFn(FLASH_DATA_FN_DATA, &params);
            if (status != M2M_SUCCESS)
            {
                if (is_internal_info(pfWriteFn))
                    ret = FLASH_ERR_WINC_ACCESS;
                else
                    ret = FLASH_ERR_LOCAL_ACCESS;
                break;
            }
        }

        u32BytesTransferred += params.u32DataSize;
        u32BytesRemaining -= params.u32DataSize;
    }

    if (u32BytesRemaining > 0)
    {
TERMINATE:
        if (pfReadFn != NULL)
            pfReadFn(FLASH_DATA_FN_TERMINATE, NULL);
        if (pfWriteFn != NULL)
            pfWriteFn(FLASH_DATA_FN_TERMINATE, NULL);
        goto ERR;
    }
    else
    {
        if (pfReadFn != NULL)
            pfReadFn(FLASH_DATA_FN_COMPLETE, NULL);
        if (pfWriteFn != NULL)
            pfWriteFn(FLASH_DATA_FN_COMPLETE, NULL);
    }

    if (pstrFlashAccess->strPersistentInfo.u8ModeFlags & (FLASH_MODE_FLAGS_CS_VALIDATE_IMAGE | FLASH_MODE_FLAGS_CS_SWITCH))
    {
        tenuCSOp enuOp;
        ret = set_changed_flag(&pstrFlashAccess->strPersistentInfo);
        if (ret < 0)
            goto ERR;
        if (pstrFlashAccess->strPersistentInfo.u8ModeFlags & FLASH_MODE_FLAGS_CS_VALIDATE_IMAGE)
        {
            if (pstrFlashAccess->strPersistentInfo.u8ModeFlags & FLASH_MODE_FLAGS_CS_SWITCH)
                enuOp = CS_VALIDATE_SWITCH;
            else
                enuOp = CS_VALIDATE_RB;
        }
        else
            enuOp = CS_SWITCH;
        status = access_control_sector(enuOp, NULL);
        if (status != M2M_SUCCESS)
        {
            ret = FLASH_ERR_WINC_ACCESS;
            goto ERR;
        }
    }

    pstrFlashAccess->strPersistentInfo.enuTransferStatus = FLASH_STATUS_DONE;
    status = winc_flash_write_verify((uint8_t*)pstrFlashAccess, HOST_CONTROL_FLASH_OFFSET, sizeof(tstrFlashAccessPersistent));
    gu8Success = 1;
ERR:
    if (pu8Buff != NULL)
        free(pu8Buff);
    access_control_sector(CS_DEINITIALIZE, NULL);
    return ret;
}

//DOM-IGNORE-END