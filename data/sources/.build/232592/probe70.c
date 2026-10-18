#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for common.h */
#ifndef STUB_COMMON_H_2674790B
#define STUB_COMMON_H_2674790B
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_common_h_2674790b;
#endif
/* stub for mbedtls/asn1write.h */
#ifndef STUB_MBEDTLS_ASN1WRITE_H_EA02F767
#define STUB_MBEDTLS_ASN1WRITE_H_EA02F767
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_mbedtls_asn1write_h_ea02f767;
#endif
/* stub for mbedtls/cipher.h */
#ifndef STUB_MBEDTLS_CIPHER_H_B7AF6EF3
#define STUB_MBEDTLS_CIPHER_H_B7AF6EF3
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_mbedtls_cipher_h_b7af6ef3;
#endif
/* stub for mbedtls/ecp.h */
#ifndef STUB_MBEDTLS_ECP_H_9478C2E4
#define STUB_MBEDTLS_ECP_H_9478C2E4
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_mbedtls_ecp_h_9478c2e4;
#endif
/* stub for mbedtls/entropy.h */
#ifndef STUB_MBEDTLS_ENTROPY_H_00DDFDE0
#define STUB_MBEDTLS_ENTROPY_H_00DDFDE0
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_mbedtls_entropy_h_00ddfde0;
#endif
/* stub for mbedtls/error.h */
#ifndef STUB_MBEDTLS_ERROR_H_B02E54B6
#define STUB_MBEDTLS_ERROR_H_B02E54B6
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_mbedtls_error_h_b02e54b6;
#endif
/* stub for mbedtls/lms.h */
#ifndef STUB_MBEDTLS_LMS_H_61C9F28D
#define STUB_MBEDTLS_LMS_H_61C9F28D
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_mbedtls_lms_h_61c9f28d;
#endif
/* stub for mbedtls/md.h */
#ifndef STUB_MBEDTLS_MD_H_BA71F681
#define STUB_MBEDTLS_MD_H_BA71F681
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_mbedtls_md_h_ba71f681;
#endif
/* stub for mbedtls/pk.h */
#ifndef STUB_MBEDTLS_PK_H_CBBBA51D
#define STUB_MBEDTLS_PK_H_CBBBA51D
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_mbedtls_pk_h_cbbba51d;
#endif
/* stub for mbedtls/rsa.h */
#ifndef STUB_MBEDTLS_RSA_H_D72FA81B
#define STUB_MBEDTLS_RSA_H_D72FA81B
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_mbedtls_rsa_h_d72fa81b;
#endif
/* stub for mbedtls/ssl.h */
#ifndef STUB_MBEDTLS_SSL_H_2B1E541D
#define STUB_MBEDTLS_SSL_H_2B1E541D
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_mbedtls_ssl_h_2b1e541d;
#endif
/* stub for psa/crypto.h */
#ifndef STUB_PSA_CRYPTO_H_E5BF8E99
#define STUB_PSA_CRYPTO_H_E5BF8E99
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_psa_crypto_h_e5bf8e99;
#endif
/* stub for psa/crypto_sizes.h */
#ifndef STUB_PSA_CRYPTO_SIZES_H_EF270AFB
#define STUB_PSA_CRYPTO_SIZES_H_EF270AFB
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_psa_crypto_sizes_h_ef270afb;
#endif
/* stub for psa_util_internal.h */
#ifndef STUB_PSA_UTIL_INTERNAL_H_BBDDD6EB
#define STUB_PSA_UTIL_INTERNAL_H_BBDDD6EB
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_psa_util_internal_h_bbddd6eb;
#endif
#endif

/*
 *  PSA hashing layer on top of Mbed TLS software crypto
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */


/* This is needed for MBEDTLS_ERR_XXX macros */

#if defined(MBEDTLS_ASN1_WRITE_C)
#endif


#if defined(MBEDTLS_PSA_CRYPTO_CLIENT)


#if defined(MBEDTLS_MD_LIGHT)
#endif
#if defined(MBEDTLS_LMS_C)
#endif
#if defined(MBEDTLS_SSL_TLS_C) && \
    (defined(MBEDTLS_USE_PSA_CRYPTO) || defined(MBEDTLS_SSL_PROTO_TLS1_3))
#endif
#if defined(PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY) ||    \
    defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC)
#endif
#if defined(MBEDTLS_USE_PSA_CRYPTO) && \
    defined(PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY)
#endif
#if defined(MBEDTLS_PK_C)
#endif
#if defined(MBEDTLS_BLOCK_CIPHER_SOME_PSA)
#endif

/* PSA_SUCCESS is kept at the top of each error table since
 * it's the most common status when everything functions properly. */
#if defined(MBEDTLS_MD_LIGHT)
const mbedtls_error_pair_t psa_to_md_errors[] =
{
    { PSA_SUCCESS,                     0 },
    { PSA_ERROR_NOT_SUPPORTED,         MBEDTLS_ERR_MD_FEATURE_UNAVAILABLE },
    { PSA_ERROR_INVALID_ARGUMENT,      MBEDTLS_ERR_MD_BAD_INPUT_DATA },
    { PSA_ERROR_INSUFFICIENT_MEMORY,   MBEDTLS_ERR_MD_ALLOC_FAILED }
};
#endif

#if defined(MBEDTLS_BLOCK_CIPHER_SOME_PSA)
const mbedtls_error_pair_t psa_to_cipher_errors[] =
{
    { PSA_SUCCESS,                     0 },
    { PSA_ERROR_NOT_SUPPORTED,         MBEDTLS_ERR_CIPHER_FEATURE_UNAVAILABLE },
    { PSA_ERROR_INVALID_ARGUMENT,      MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA },
    { PSA_ERROR_INSUFFICIENT_MEMORY,   MBEDTLS_ERR_CIPHER_ALLOC_FAILED }
};
#endif

#if defined(MBEDTLS_LMS_C)
const mbedtls_error_pair_t psa_to_lms_errors[] =
{
    { PSA_SUCCESS,                     0 },
    { PSA_ERROR_BUFFER_TOO_SMALL,      MBEDTLS_ERR_LMS_BUFFER_TOO_SMALL },
    { PSA_ERROR_INVALID_ARGUMENT,      MBEDTLS_ERR_LMS_BAD_INPUT_DATA }
};
#endif

#if defined(MBEDTLS_SSL_TLS_C) && \
    (defined(MBEDTLS_USE_PSA_CRYPTO) || defined(MBEDTLS_SSL_PROTO_TLS1_3))
const mbedtls_error_pair_t psa_to_ssl_errors[] =
{
    { PSA_SUCCESS,                     0 },
    { PSA_ERROR_INSUFFICIENT_MEMORY,   MBEDTLS_ERR_SSL_ALLOC_FAILED },
    { PSA_ERROR_NOT_SUPPORTED,         MBEDTLS_ERR_SSL_FEATURE_UNAVAILABLE },
    { PSA_ERROR_INVALID_SIGNATURE,     MBEDTLS_ERR_SSL_INVALID_MAC },
    { PSA_ERROR_INVALID_ARGUMENT,      MBEDTLS_ERR_SSL_BAD_INPUT_DATA },
    { PSA_ERROR_BAD_STATE,             MBEDTLS_ERR_SSL_INTERNAL_ERROR },
    { PSA_ERROR_BUFFER_TOO_SMALL,      MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL }
};
#endif

#if defined(PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY) ||    \
    defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC)
const mbedtls_error_pair_t psa_to_pk_rsa_errors[] =
{
    { PSA_SUCCESS,                     0 },
    { PSA_ERROR_NOT_PERMITTED,         MBEDTLS_ERR_RSA_BAD_INPUT_DATA },
    { PSA_ERROR_INVALID_ARGUMENT,      MBEDTLS_ERR_RSA_BAD_INPUT_DATA },
    { PSA_ERROR_INVALID_HANDLE,        MBEDTLS_ERR_RSA_BAD_INPUT_DATA },
    { PSA_ERROR_BUFFER_TOO_SMALL,      MBEDTLS_ERR_RSA_OUTPUT_TOO_LARGE },
    { PSA_ERROR_INSUFFICIENT_ENTROPY,  MBEDTLS_ERR_RSA_RNG_FAILED },
    { PSA_ERROR_INVALID_SIGNATURE,     MBEDTLS_ERR_RSA_VERIFY_FAILED },
    { PSA_ERROR_INVALID_PADDING,       MBEDTLS_ERR_RSA_INVALID_PADDING }
};
#endif

#if defined(MBEDTLS_USE_PSA_CRYPTO) && \
    defined(PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY)
const mbedtls_error_pair_t psa_to_pk_ecdsa_errors[] =
{
    { PSA_SUCCESS,                     0 },
    { PSA_ERROR_NOT_PERMITTED,         MBEDTLS_ERR_ECP_BAD_INPUT_DATA },
    { PSA_ERROR_INVALID_ARGUMENT,      MBEDTLS_ERR_ECP_BAD_INPUT_DATA },
    { PSA_ERROR_INVALID_HANDLE,        MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE },
    { PSA_ERROR_BUFFER_TOO_SMALL,      MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL },
    { PSA_ERROR_INSUFFICIENT_ENTROPY,  MBEDTLS_ERR_ECP_RANDOM_FAILED },
    { PSA_ERROR_INVALID_SIGNATURE,     MBEDTLS_ERR_ECP_VERIFY_FAILED }
};
#endif

int psa_generic_status_to_mbedtls(psa_status_t status)
{
    switch (status) {
        case PSA_SUCCESS:
            return 0;
        case PSA_ERROR_NOT_SUPPORTED:
            return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
        case PSA_ERROR_CORRUPTION_DETECTED:
            return MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
        case PSA_ERROR_COMMUNICATION_FAILURE:
        case PSA_ERROR_HARDWARE_FAILURE:
            return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
        case PSA_ERROR_NOT_PERMITTED:
        default:
            return MBEDTLS_ERR_ERROR_GENERIC_ERROR;
    }
}

int psa_status_to_mbedtls(psa_status_t status,
                          const mbedtls_error_pair_t *local_translations,
                          size_t local_errors_num,
                          int (*fallback_f)(psa_status_t))
{
    for (size_t i = 0; i < local_errors_num; i++) {
        if (status == local_translations[i].psa_status) {
            return local_translations[i].mbedtls_error;
        }
    }
    return fallback_f(status);
}

#if defined(MBEDTLS_PK_C)
int psa_pk_status_to_mbedtls(psa_status_t status)
{
    switch (status) {
        case PSA_ERROR_INVALID_HANDLE:
            return MBEDTLS_ERR_PK_KEY_INVALID_FORMAT;
        case PSA_ERROR_BUFFER_TOO_SMALL:
            return MBEDTLS_ERR_PK_BUFFER_TOO_SMALL;
        case PSA_ERROR_NOT_SUPPORTED:
            return MBEDTLS_ERR_PK_FEATURE_UNAVAILABLE;
        case PSA_ERROR_INVALID_ARGUMENT:
            return MBEDTLS_ERR_PK_INVALID_ALG;
        case PSA_ERROR_NOT_PERMITTED:
            return MBEDTLS_ERR_PK_TYPE_MISMATCH;
        case PSA_ERROR_INSUFFICIENT_MEMORY:
            return MBEDTLS_ERR_PK_ALLOC_FAILED;
        case PSA_ERROR_BAD_STATE:
            return MBEDTLS_ERR_PK_BAD_INPUT_DATA;
        case PSA_ERROR_DATA_CORRUPT:
        case PSA_ERROR_DATA_INVALID:
        case PSA_ERROR_STORAGE_FAILURE:
            return MBEDTLS_ERR_PK_FILE_IO_ERROR;
        default:
            return psa_generic_status_to_mbedtls(status);
    }
}
#endif /* MBEDTLS_PK_C */

/****************************************************************/
/* Key management */
/****************************************************************/

#if defined(PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY)
psa_ecc_family_t mbedtls_ecc_group_to_psa(mbedtls_ecp_group_id grpid,
                                          size_t *bits)
{
    switch (grpid) {
#if defined(MBEDTLS_ECP_HAVE_SECP192R1)
        case MBEDTLS_ECP_DP_SECP192R1:
            *bits = 192;
            return PSA_ECC_FAMILY_SECP_R1;
#endif
#if defined(MBEDTLS_ECP_HAVE_SECP224R1)
        case MBEDTLS_ECP_DP_SECP224R1:
            *bits = 224;
            return PSA_ECC_FAMILY_SECP_R1;
#endif
#if defined(MBEDTLS_ECP_HAVE_SECP256R1)
        case MBEDTLS_ECP_DP_SECP256R1:
            *bits = 256;
            return PSA_ECC_FAMILY_SECP_R1;
#endif
#if defined(MBEDTLS_ECP_HAVE_SECP384R1)
        case MBEDTLS_ECP_DP_SECP384R1:
            *bits = 384;
            return PSA_ECC_FAMILY_SECP_R1;
#endif
#if defined(MBEDTLS_ECP_HAVE_SECP521R1)
        case MBEDTLS_ECP_DP_SECP521R1:
            *bits = 521;
            return PSA_ECC_FAMILY_SECP_R1;
#endif
#if defined(MBEDTLS_ECP_HAVE_BP256R1)
        case MBEDTLS_ECP_DP_BP256R1:
            *bits = 256;
            return PSA_ECC_FAMILY_BRAINPOOL_P_R1;
#endif
#if defined(MBEDTLS_ECP_HAVE_BP384R1)
        case MBEDTLS_ECP_DP_BP384R1:
            *bits = 384;
            return PSA_ECC_FAMILY_BRAINPOOL_P_R1;
#endif
#if defined(MBEDTLS_ECP_HAVE_BP512R1)
        case MBEDTLS_ECP_DP_BP512R1:
            *bits = 512;
            return PSA_ECC_FAMILY_BRAINPOOL_P_R1;
#endif
#if defined(MBEDTLS_ECP_HAVE_CURVE25519)
        case MBEDTLS_ECP_DP_CURVE25519:
            *bits = 255;
            return PSA_ECC_FAMILY_MONTGOMERY;
#endif
#if defined(MBEDTLS_ECP_HAVE_SECP192K1)
        case MBEDTLS_ECP_DP_SECP192K1:
            *bits = 192;
            return PSA_ECC_FAMILY_SECP_K1;
#endif
#if defined(MBEDTLS_ECP_HAVE_SECP224K1)
    /* secp224k1 is not and will not be supported in PSA (#3541). */
#endif
#if defined(MBEDTLS_ECP_HAVE_SECP256K1)
        case MBEDTLS_ECP_DP_SECP256K1:
            *bits = 256;
            return PSA_ECC_FAMILY_SECP_K1;
#endif
#if defined(MBEDTLS_ECP_HAVE_CURVE448)
        case MBEDTLS_ECP_DP_CURVE448:
            *bits = 448;
            return PSA_ECC_FAMILY_MONTGOMERY;
#endif
        default:
            *bits = 0;
            return 0;
    }
}

mbedtls_ecp_group_id mbedtls_ecc_group_from_psa(psa_ecc_family_t family,
                                                size_t bits)
{
    switch (family) {
        case PSA_ECC_FAMILY_SECP_R1:
            switch (bits) {
#if defined(PSA_WANT_ECC_SECP_R1_192)
                case 192:
                    return MBEDTLS_ECP_DP_SECP192R1;
#endif
#if defined(PSA_WANT_ECC_SECP_R1_224)
                case 224:
                    return MBEDTLS_ECP_DP_SECP224R1;
#endif
#if defined(PSA_WANT_ECC_SECP_R1_256)
                case 256:
                    return MBEDTLS_ECP_DP_SECP256R1;
#endif
#if defined(PSA_WANT_ECC_SECP_R1_384)
                case 384:
                    return MBEDTLS_ECP_DP_SECP384R1;
#endif
#if defined(PSA_WANT_ECC_SECP_R1_521)
                case 521:
                    return MBEDTLS_ECP_DP_SECP521R1;
#endif
            }
            break;

        case PSA_ECC_FAMILY_BRAINPOOL_P_R1:
            switch (bits) {
#if defined(PSA_WANT_ECC_BRAINPOOL_P_R1_256)
                case 256:
                    return MBEDTLS_ECP_DP_BP256R1;
#endif
#if defined(PSA_WANT_ECC_BRAINPOOL_P_R1_384)
                case 384:
                    return MBEDTLS_ECP_DP_BP384R1;
#endif
#if defined(PSA_WANT_ECC_BRAINPOOL_P_R1_512)
                case 512:
                    return MBEDTLS_ECP_DP_BP512R1;
#endif
            }
            break;

        case PSA_ECC_FAMILY_MONTGOMERY:
            switch (bits) {
#if defined(PSA_WANT_ECC_MONTGOMERY_255)
                case 255:
                    return MBEDTLS_ECP_DP_CURVE25519;
#endif
#if defined(PSA_WANT_ECC_MONTGOMERY_448)
                case 448:
                    return MBEDTLS_ECP_DP_CURVE448;
#endif
            }
            break;

        case PSA_ECC_FAMILY_SECP_K1:
            switch (bits) {
#if defined(PSA_WANT_ECC_SECP_K1_192)
                case 192:
                    return MBEDTLS_ECP_DP_SECP192K1;
#endif
#if defined(PSA_WANT_ECC_SECP_K1_224)
            /* secp224k1 is not and will not be supported in PSA (#3541). */
#endif
#if defined(PSA_WANT_ECC_SECP_K1_256)
                case 256:
                    return MBEDTLS_ECP_DP_SECP256K1;
#endif
            }
            break;
    }

    return MBEDTLS_ECP_DP_NONE;
}
#endif /* PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY */

/* Wrapper function allowing the classic API to use the PSA RNG.
 *
 * `mbedtls_psa_get_random(MBEDTLS_PSA_RANDOM_STATE, ...)` calls
 * `psa_generate_random(...)`. The state parameter is ignored since the
 * PSA API doesn't support passing an explicit state.
 */
int mbedtls_psa_get_random(void *p_rng,
                           unsigned char *output,
                           size_t output_size)
{
    /* This function takes a pointer to the RNG state because that's what
     * classic mbedtls functions using an RNG expect. The PSA RNG manages
     * its own state internally and doesn't let the caller access that state.
     * So we just ignore the state parameter, and in practice we'll pass
     * NULL. */
    (void) p_rng;
    psa_status_t status = psa_generate_random(output, output_size);
    if (status == PSA_SUCCESS) {
        return 0;
    } else {
        return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
    }
}

#endif /* MBEDTLS_PSA_CRYPTO_CLIENT */

#if defined(MBEDTLS_PSA_UTIL_HAVE_ECDSA)

/**
 * \brief  Convert a single raw coordinate to DER ASN.1 format. The output der
 *         buffer is filled backward (i.e. starting from its end).
 *
 * \param raw_buf           Buffer containing the raw coordinate to be
 *                          converted.
 * \param raw_len           Length of raw_buf in bytes. This must be > 0.
 * \param der_buf_start     Pointer to the beginning of the buffer which
 *                          will be filled with the DER converted data.
 * \param der_buf_end       End of the buffer used to store the DER output.
 *
 * \return                  On success, the amount of data (in bytes) written to
 *                          the DER buffer.
 * \return                  MBEDTLS_ERR_ASN1_BUF_TOO_SMALL if the provided der
 *                          buffer is too small to contain all the converted data.
 * \return                  MBEDTLS_ERR_ASN1_INVALID_DATA if the input raw
 *                          coordinate is null (i.e. all zeros).
 *
 * \warning                 Raw and der buffer must not be overlapping.
 */
static int convert_raw_to_der_single_int(const unsigned char *raw_buf, size_t raw_len,
                                         unsigned char *der_buf_start,
                                         unsigned char *der_buf_end)
{
    unsigned char *p = der_buf_end;
    int len;
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;

    /* ASN.1 DER encoding requires minimal length, so skip leading 0s.
     * Provided input MPIs should not be 0, but as a failsafe measure, still
     * detect that and return error in case. */
    while (*raw_buf == 0x00) {
        ++raw_buf;
        --raw_len;
        if (raw_len == 0) {
            return MBEDTLS_ERR_ASN1_INVALID_DATA;
        }
    }
    len = (int) raw_len;

    /* Copy the raw coordinate to the end of der_buf. */
    if ((p - der_buf_start) < len) {
        return MBEDTLS_ERR_ASN1_BUF_TOO_SMALL;
    }
    p -= len;
    memcpy(p, raw_buf, len);

    /* If MSb is 1, ASN.1 requires that we prepend a 0. */
    if (*p & 0x80) {
        if ((p - der_buf_start) < 1) {
            return MBEDTLS_ERR_ASN1_BUF_TOO_SMALL;
        }
        --p;
        *p = 0x00;
        ++len;
    }

    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_len(&p, der_buf_start, len));
    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_tag(&p, der_buf_start, MBEDTLS_ASN1_INTEGER));

    return len;
}

int mbedtls_ecdsa_raw_to_der(size_t bits, const unsigned char *raw, size_t raw_len,
                             unsigned char *der, size_t der_size, size_t *der_len)
{
    unsigned char r[PSA_BITS_TO_BYTES(PSA_VENDOR_ECC_MAX_CURVE_BITS)];
    unsigned char s[PSA_BITS_TO_BYTES(PSA_VENDOR_ECC_MAX_CURVE_BITS)];
    const size_t coordinate_len = PSA_BITS_TO_BYTES(bits);
    size_t len = 0;
    unsigned char *p = der + der_size;
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;

    if (bits == 0) {
        return MBEDTLS_ERR_ASN1_INVALID_DATA;
    }
    if (raw_len != (2 * coordinate_len)) {
        return MBEDTLS_ERR_ASN1_INVALID_DATA;
    }
    if (coordinate_len > sizeof(r)) {
        return MBEDTLS_ERR_ASN1_BUF_TOO_SMALL;
    }

    /* Since raw and der buffers might overlap, dump r and s before starting
     * the conversion. */
    memcpy(r, raw, coordinate_len);
    memcpy(s, raw + coordinate_len, coordinate_len);

    /* der buffer will initially be written starting from its end so we pick s
     * first and then r. */
    ret = convert_raw_to_der_single_int(s, coordinate_len, der, p);
    if (ret < 0) {
        return ret;
    }
    p -= ret;
    len += ret;

    ret = convert_raw_to_der_single_int(r, coordinate_len, der, p);
    if (ret < 0) {
        return ret;
    }
    p -= ret;
    len += ret;

    /* Add ASN.1 header (len + tag). */
    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_len(&p, der, len));
    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_tag(&p, der,
                                                     MBEDTLS_ASN1_CONSTRUCTED |
                                                     MBEDTLS_ASN1_SEQUENCE));

    /* memmove the content of der buffer to its beginnig. */
    memmove(der, p, len);
    *der_len = len;

    return 0;
}

/**
 * \brief Convert a single integer from ASN.1 DER format to raw.
 *
 * \param der               Buffer containing the DER integer value to be
 *                          converted.
 * \param der_len           Length of the der buffer in bytes.
 * \param raw               Output buffer that will be filled with the
 *                          converted data. This should be at least
 *                          coordinate_size bytes and it must be zeroed before
 *                          calling this function.
 * \param coordinate_size   Size (in bytes) of a single coordinate in raw
 *                          format.
 *
 * \return                  On success, the amount of DER data parsed from the
 *                          provided der buffer.
 * \return                  MBEDTLS_ERR_ASN1_UNEXPECTED_TAG if the integer tag
 *                          is missing in the der buffer.
 * \return                  MBEDTLS_ERR_ASN1_LENGTH_MISMATCH if the integer
 *                          is null (i.e. all zeros) or if the output raw buffer
 *                          is too small to contain the converted raw value.
 *
 * \warning                 Der and raw buffers must not be overlapping.
 */
static int convert_der_to_raw_single_int(unsigned char *der, size_t der_len,
                                         unsigned char *raw, size_t coordinate_size)
{
    unsigned char *p = der;
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    size_t unpadded_len, padding_len = 0;

    /* Get the length of ASN.1 element (i.e. the integer we need to parse). */
    ret = mbedtls_asn1_get_tag(&p, p + der_len, &unpadded_len,
                               MBEDTLS_ASN1_INTEGER);
    if (ret != 0) {
        return ret;
    }

    /* It's invalid to have:
     * - unpadded_len == 0.
     * - MSb set without a leading 0x00 (leading 0x00 is checked below). */
    if (((unpadded_len == 0) || (*p & 0x80) != 0)) {
        return MBEDTLS_ERR_ASN1_INVALID_DATA;
    }

    /* Skip possible leading zero */
    if (*p == 0x00) {
        p++;
        unpadded_len--;
        /* It is not allowed to have more than 1 leading zero.
         * Ignore the case in which unpadded_len = 0 because that's a 0 encoded
         * in ASN.1 format (i.e. 020100). */
        if ((unpadded_len > 0) && (*p == 0x00)) {
            return MBEDTLS_ERR_ASN1_INVALID_DATA;
        }
    }

    if (unpadded_len > coordinate_size) {
        /* Parsed number is longer than the maximum expected value. */
        return MBEDTLS_ERR_ASN1_INVALID_DATA;
    }
    padding_len = coordinate_size - unpadded_len;
    /* raw buffer was already zeroed by the calling function so zero-padding
     * operation is skipped here. */
    memcpy(raw + padding_len, p, unpadded_len);
    p += unpadded_len;

    return (int) (p - der);
}

int mbedtls_ecdsa_der_to_raw(size_t bits, const unsigned char *der, size_t der_len,
                             unsigned char *raw, size_t raw_size, size_t *raw_len)
{
    unsigned char raw_tmp[PSA_VENDOR_ECDSA_SIGNATURE_MAX_SIZE];
    unsigned char *p = (unsigned char *) der;
    size_t data_len;
    size_t coordinate_size = PSA_BITS_TO_BYTES(bits);
    int ret;

    if (bits == 0) {
        return MBEDTLS_ERR_ASN1_INVALID_DATA;
    }
    /* The output raw buffer should be at least twice the size of a raw
     * coordinate in order to store r and s. */
    if (raw_size < coordinate_size * 2) {
        return MBEDTLS_ERR_ASN1_BUF_TOO_SMALL;
    }
    if (2 * coordinate_size > sizeof(raw_tmp)) {
        return MBEDTLS_ERR_ASN1_BUF_TOO_SMALL;
    }

    /* Check that the provided input DER buffer has the right header. */
    ret = mbedtls_asn1_get_tag(&p, der + der_len, &data_len,
                               MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE);
    if (ret != 0) {
        return ret;
    }

    memset(raw_tmp, 0, 2 * coordinate_size);

    /* Extract r */
    ret = convert_der_to_raw_single_int(p, data_len, raw_tmp, coordinate_size);
    if (ret < 0) {
        return ret;
    }
    p += ret;
    data_len -= ret;

    /* Extract s */
    ret = convert_der_to_raw_single_int(p, data_len, raw_tmp + coordinate_size,
                                        coordinate_size);
    if (ret < 0) {
        return ret;
    }
    p += ret;
    data_len -= ret;

    /* Check that we consumed all the input der data. */
    if ((size_t) (p - der) != der_len) {
        return MBEDTLS_ERR_ASN1_LENGTH_MISMATCH;
    }

    memcpy(raw, raw_tmp, 2 * coordinate_size);
    *raw_len = 2 * coordinate_size;

    return 0;
}

#endif /* MBEDTLS_PSA_UTIL_HAVE_ECDSA */
