#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for esp_random.h */
#ifndef STUB_ESP_RANDOM_H_B40BFDA3
#define STUB_ESP_RANDOM_H_B40BFDA3
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_random_h_b40bfda3;
#endif
/* stub for freertos/FreeRTOS.h */
#ifndef STUB_FREERTOS_FREERTOS_H_AC994C92
#define STUB_FREERTOS_FREERTOS_H_AC994C92
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_freertos_freertos_h_ac994c92;
#endif
/* stub for freertos/task.h */
#ifndef STUB_FREERTOS_TASK_H_55F96C2C
#define STUB_FREERTOS_TASK_H_55F96C2C
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_freertos_task_h_55f96c2c;
#endif
/* stub for sdkconfig.h */
#ifndef STUB_SDKCONFIG_H_87D33A20
#define STUB_SDKCONFIG_H_87D33A20
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_sdkconfig_h_87d33a20;
#endif
/* stub for tinycrypt/ecc.h */
#ifndef STUB_TINYCRYPT_ECC_H_84235D74
#define STUB_TINYCRYPT_ECC_H_84235D74
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_tinycrypt_ecc_h_84235d74;
#endif
/* stub for unity.h */
#ifndef STUB_UNITY_H_F941ADD4
#define STUB_UNITY_H_F941ADD4
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_unity_h_f941add4;
#endif
#endif

/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * Unit tests for TinyCrypt ECC (P-256 / P-192) APIs declared in tinycrypt/ecc.h.
 * Procedure mirrors TEST_CASE("ble_smp_public_key_check", "[ble_smp][native_crypto]") in test_smp.c.
 */

#include <stdint.h>
#include <string.h>



#if defined(CONFIG_BT_SMP_CRYPTO_STACK_TINYCRYPT) && CONFIG_BT_SMP_CRYPTO_STACK_TINYCRYPT

static void tinycrypt_fill_private_p256(uint8_t *priv)
{
    for (int i = 0; i < NUM_ECC_BYTES; i++) {
        priv[i] = (uint8_t)(0x10 + (i & 0x0F));
    }
}

static void tinycrypt_bt_rand_p256(uint8_t *priv)
{
    memset(priv, 0x55, NUM_ECC_BYTES);
    for (int i = 0; i < NUM_ECC_BYTES / (int)sizeof(uint32_t); i++) {
        uint32_t r = esp_random();
        memcpy(priv + i * sizeof(uint32_t), &r, sizeof(uint32_t));
    }
}

#if uECC_SUPPORTS_secp192r1
static void tinycrypt_fill_private_p192(uint8_t *priv)
{
    for (int i = 0; i < NUM_ECC_BYTES_SECP192R1; i++) {
        priv[i] = (uint8_t)(0x10 + (i & 0x0F));
    }
}

static void tinycrypt_bt_rand_p192(uint8_t *priv)
{
    memset(priv, 0x55, NUM_ECC_BYTES_SECP192R1);
    for (int i = 0; i < NUM_ECC_BYTES_SECP192R1 / (int)sizeof(uint32_t); i++) {
        uint32_t r = esp_random();
        memcpy(priv + i * sizeof(uint32_t), &r, sizeof(uint32_t));
    }
}
#endif // uECC_SUPPORTS_secp192r1

/**
 * @brief P-256: public key from private, validation, generator on curve, tamper, random keys.
 *
 * Same flow as ble_smp_public_key_check (native): fixed private → public → valid;
 * zero Y → invalid; generator point check; random private loop.
 */
TEST_CASE("tinycrypt_ecc_p256_public_key_check", "[ble_smp][tinycrypt_ecc]")
{
    vTaskDelay(200 / portTICK_PERIOD_MS);

    uECC_Curve curve = uECC_secp256r1();
    uint8_t private_key[NUM_ECC_BYTES];
    uint8_t public_key[NUM_ECC_BYTES * 2];

    tinycrypt_fill_private_p256(private_key);
    TEST_ASSERT_EQUAL_INT(1, uECC_compute_public_key(private_key, public_key, curve));
    TEST_ASSERT_EQUAL_INT(0, uECC_valid_public_key(public_key, curve));

    for (int i = 0; i < NUM_ECC_BYTES; i++) {
        public_key[NUM_ECC_BYTES + i] = 0;
    }
    TEST_ASSERT_NOT_EQUAL(0, uECC_valid_public_key(public_key, curve));

    uint8_t g_pub[NUM_ECC_BYTES * 2];
    uECC_vli_nativeToBytes(g_pub, NUM_ECC_BYTES, curve->G);
    uECC_vli_nativeToBytes(g_pub + NUM_ECC_BYTES, NUM_ECC_BYTES, curve->G + NUM_ECC_WORDS);
    TEST_ASSERT_EQUAL_INT(-4, uECC_valid_public_key(g_pub, curve));

    for (int j = 0; j < 100; j++) {
        tinycrypt_bt_rand_p256(private_key);
        if (uECC_compute_public_key(private_key, public_key, curve) != 1) {
            continue;
        }
        TEST_ASSERT_EQUAL_INT(0, uECC_valid_public_key(public_key, curve));
    }
}

TEST_CASE("tinycrypt_ecc_p256_vli_mmod_fast", "[ble_smp][tinycrypt_ecc]")
{
    vTaskDelay(200 / portTICK_PERIOD_MS);

    uECC_Curve curve = uECC_secp256r1();
    unsigned int prod[NUM_ECC_WORDS * 2];
    unsigned int res[NUM_ECC_WORDS];

    memset(prod, 0, sizeof(prod));
    uECC_vli_set(prod, curve->p, NUM_ECC_WORDS);
    vli_mmod_fast_secp256r1(res, prod);
    TEST_ASSERT(uECC_vli_isZero(res, NUM_ECC_WORDS));
}

#if uECC_SUPPORTS_secp192r1
/**
 * @brief P-192: same procedure and uECC_valid_public_key expectations as P-256 (valid key 0, G is -4).
 */
TEST_CASE("tinycrypt_ecc_p192_public_key_check", "[ble_smp][tinycrypt_ecc]")
{
    vTaskDelay(200 / portTICK_PERIOD_MS);

    uECC_Curve curve = uECC_secp192r1();
    uint8_t private_key[NUM_ECC_BYTES_SECP192R1];
    uint8_t public_key[NUM_ECC_BYTES_SECP192R1 * 2];

    tinycrypt_fill_private_p192(private_key);
    TEST_ASSERT_EQUAL_INT(1, uECC_compute_public_key(private_key, public_key, curve));
    TEST_ASSERT_EQUAL_INT(0, uECC_valid_public_key(public_key, curve));

    for (int i = 0; i < NUM_ECC_BYTES_SECP192R1; i++) {
        public_key[NUM_ECC_BYTES_SECP192R1 + i] = 0;
    }
    TEST_ASSERT_NOT_EQUAL(0, uECC_valid_public_key(public_key, curve));

    uint8_t g_pub[NUM_ECC_BYTES_SECP192R1 * 2];
    uECC_vli_nativeToBytes(g_pub, NUM_ECC_BYTES_SECP192R1, curve->G);
    uECC_vli_nativeToBytes(g_pub + NUM_ECC_BYTES_SECP192R1, NUM_ECC_BYTES_SECP192R1,
                           curve->G + NUM_ECC_WORDS_SECP192R1);
    TEST_ASSERT_EQUAL_INT(-4, uECC_valid_public_key(g_pub, curve));

    for (int j = 0; j < 100; j++) {
        tinycrypt_bt_rand_p192(private_key);
        if (uECC_compute_public_key(private_key, public_key, curve) != 1) {
            continue;
        }
        TEST_ASSERT_EQUAL_INT(0, uECC_valid_public_key(public_key, curve));
    }
}

TEST_CASE("tinycrypt_ecc_p192_vli_mmod_fast", "[ble_smp][tinycrypt_ecc]")
{
    vTaskDelay(200 / portTICK_PERIOD_MS);

    uECC_Curve curve = uECC_secp192r1();
    unsigned int prod[NUM_ECC_WORDS_SECP192R1 * 2];
    unsigned int res[NUM_ECC_WORDS_SECP192R1];

    memset(prod, 0, sizeof(prod));
    uECC_vli_set(prod, curve->p, NUM_ECC_WORDS_SECP192R1);
    vli_mmod_fast_secp192r1(res, prod);
    TEST_ASSERT(uECC_vli_isZero(res, NUM_ECC_WORDS_SECP192R1));
}
#endif /* #if uECC_SUPPORTS_secp192r1 */

#endif /* CONFIG_BT_SMP_CRYPTO_STACK_TINYCRYPT */
