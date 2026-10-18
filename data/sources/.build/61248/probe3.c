#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for adc_params.h */
#ifndef STUB_ADC_PARAMS_H_2FFC0A1C
#define STUB_ADC_PARAMS_H_2FFC0A1C
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_adc_params_h_2ffc0a1c;
#endif
/* stub for log.h */
#ifndef STUB_LOG_H_A26B3C1A
#define STUB_LOG_H_A26B3C1A
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_log_h_a26b3c1a;
#endif
/* stub for periph/adc.h */
#ifndef STUB_PERIPH_ADC_H_1E02BB63
#define STUB_PERIPH_ADC_H_1E02BB63
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_periph_adc_h_1e02bb63;
#endif
/* stub for saul/periph.h */
#ifndef STUB_SAUL_PERIPH_H_A585E403
#define STUB_SAUL_PERIPH_H_A585E403
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_saul_periph_h_a585e403;
#endif
/* stub for saul_reg.h */
#ifndef STUB_SAUL_REG_H_0FDA4D3F
#define STUB_SAUL_REG_H_0FDA4D3F
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_saul_reg_h_0fda4d3f;
#endif
#endif

/*
 * SPDX-FileCopyrightText: 2016 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of ADC lines directly mapped to SAUL reg
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */


/**
 * @brief   Define the number of configured sensors
 */
#define SAUL_ADC_NUMOF    ARRAY_SIZE(saul_adc_params)

/**
 * @brief   Allocate memory for pointers to the ADC parameter structs
 *
 * We use this extra level of indirection to be able to keep the saul_adc_params
 * array const and residing in ROM.
 */
static const saul_adc_params_t *saul_adcs[SAUL_ADC_NUMOF];

/**
 * @brief   Memory for the registry entries
 */
static saul_reg_t saul_reg_entries[SAUL_ADC_NUMOF];

/**
 * @brief   Reference the driver struct
 */
extern saul_driver_t adc_saul_driver;

void auto_init_saul_adc(void)
{
    for (unsigned i = 0; i < SAUL_ADC_NUMOF; i++) {
        const saul_adc_params_t *p = &saul_adc_params[i];
        saul_adcs[i] = p;

        LOG_DEBUG("[auto_init_saul] initializing direct ADC #%u\n", i);

        saul_reg_entries[i].dev = &saul_adcs[i];
        saul_reg_entries[i].name = p->name;
        saul_reg_entries[i].driver = &adc_saul_driver;
        /* initialize the ADC line */
        adc_init(p->line);
        /* add to registry */
        saul_reg_add(&(saul_reg_entries[i]));
    }
}
