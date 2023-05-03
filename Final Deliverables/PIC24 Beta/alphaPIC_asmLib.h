/* 
 * File:   alphaPIC_asmLib.h
 * Author: Croix
 *
 * Created on April 26, 2023, 11:42 AM
 */

#ifndef ALPHAPIC_ASMLIB_H
#define	ALPHAPIC_ASMLIB_H

#ifdef	__cplusplus
extern "C" {
#endif
    void wait1ms(void); 
    void wait100us(void);
    void write_0(void);
    void write_1(void);
#ifdef	__cplusplus
}
#endif

#endif	/* ALPHAPIC_ASMLIB_H */

