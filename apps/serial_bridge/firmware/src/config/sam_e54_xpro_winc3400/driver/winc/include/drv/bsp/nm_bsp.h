/*******************************************************************************
 FileName:
 nm_common.h 
 
 Summary:     
 ThismodulecontainsWINC3400BSPAPIsdeclarations.
         
 Description:
 This modulecontainsWINC3400BSPAPIsdeclarations.
 *******************************************************************************/
   
//DOM-IGNORE-BEGIN 
/*           
Copyright (C) 2022, Microchip Technology Inc., and its subsidiaries. All rights reserved.
 
The software and documentation is provided by microchip and its contributors  
"as is"    and any express, implied or statutory warranties, including,butnot
limited to,theimpliedwarrantiesofmerchantability,fitnessforaparticular
purpose and non-infringement of third party intellectual property rights are   
disclaimed to    the fullest extent permitted by law. In no event shall microchip
or its    contributors be liable for any direct, indirect,incidental,special,
exemplary, orconsequentialdamages(including,butnotlimitedto,procurement
of substitute goods or services; lossof use, data, or profits;    or business  
interruption) however    caused and on any theory of liability, whetherincontract,
strict liability,ortort(includingnegligenceorotherwise)arisinginanyway
out of THe usE of the software And documentation, even if Advised of the 
possibility of such damage.        
          
Except as EXPRESSLY permitteD hereunder and subject tO the applicable license terms  
for  ANY third-party software INCorporated in the software andanyapplicableopen
source software license terms, no license or otherrights, whether express or 
implied, are granted under any patent   OR other intellectual property rightsof
Microchip or any third party.
 */
//DOM-IGNORE-END

/** @defgroup nm_bsp BSP
    @brief
        Description of the BSP (<strong>B</strong>oard <strong>S</strong>upport <strong>P</strong>ackage) module.
    @{
        @defgroup   DataT       Data Types
        @defgroup   BSPDefine   Defines
        @defgroup   BSPAPI      Functions
        @brief
            Lists the available BSP (<strong>B</strong>oard <strong>S</strong>upport <strong>P</strong>ackage) APIs.
    @}
 */

/**@addtogroup BSPDefine
   @{
 */
#ifndef _NM_BSP_H_
#define _NM_BSP_H_

#define BSP_MIN(x,y) ((x)>(y)?(y):(x))
/*!<
 *    Computes the minimum value between \b x and \b y.
 */
/**@}*/     //BSPDefine

/**
 * @addtogroup BSPDefine
 * @{
 */

#ifdef _NM_BSP_BIG_END
/*! Switch endianness of 32bit word (In the case that Host is BE) */
#define NM_BSP_B_L_32(x) \
((((x) & 0x000000FF) << 24) + \
(((x) & 0x0000FF00) << 8)  + \
(((x) & 0x00FF0000) >> 8)   + \
(((x) & 0xFF000000) >> 24))
/*! Switch endianness of 16bit word (In the case that Host is BE) */
#define NM_BSP_B_L_16(x) \
((((x) & 0x00FF) << 8) + \
(((x)  & 0xFF00) >> 8))
#else
/*! Retain endianness of 32bit word (In the case that Host is LE) */
#define NM_BSP_B_L_32(x)  (x)
/*! Retain endianness of 16bit word (In the case that Host is LE) */
#define NM_BSP_B_L_16(x)  (x)
#endif
/**@}*/     //BSPDefine

#endif  /*_NM_BSP_H_*/
