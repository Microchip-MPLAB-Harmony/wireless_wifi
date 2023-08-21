/**
 *
 * \file
 *
 * \brief This module contains WINC firmware information APIs declarations.
 */
/*
Copyright (C) 2022, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef _M2M_FWINFO_H_
#define _M2M_FWINFO_H_

#ifdef __cplusplus
extern "C" {
#endif

int_fast8_t m2m_fwinfo_version_check(tstrM2mRev *pstrRev);

int_fast8_t m2m_fwinfo_get_firmware_info(bool bMainImage, tstrM2mRev *pstrRev);

#ifdef __cplusplus
}
#endif

#endif /* _WINCVER_H_ */
