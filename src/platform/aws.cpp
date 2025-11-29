#include "aws.hpp"

#include <aws/common/allocator.h>

#include <aws/cal/cal.h>
#include <aws/cal/hmac.h>
#include <aws/cal/hash.h>

#define OPENSSL_SUPPRESS_DEPRECATED

#include <openssl/crypto.h>
#include <openssl/hmac.h>

class ByoCryptoHmac {
    static void destroy(aws_hmac *hmac) {
        if (hmac == NULL) {
            return;
        }

        HMAC_CTX *ctx = (HMAC_CTX *)hmac->impl;
        if (ctx != NULL) {
            HMAC_CTX_free(ctx);
        }

        aws_mem_release(hmac->allocator, hmac);
    }

    static int update(aws_hmac *hmac, const aws_byte_cursor *to_hmac) {
        if (!hmac->good) {
            return aws_raise_error(AWS_ERROR_INVALID_STATE);
        }

        HMAC_CTX *ctx = (HMAC_CTX *)hmac->impl;

        if (AWS_LIKELY(HMAC_Update(ctx, to_hmac->ptr, to_hmac->len))) {
            return AWS_OP_SUCCESS;
        }

        hmac->good = false;
        return aws_raise_error(AWS_ERROR_INVALID_ARGUMENT);
    }

    static int finalize(aws_hmac *hmac, aws_byte_buf *output) {
        if (!hmac->good) {
            return aws_raise_error(AWS_ERROR_INVALID_STATE);
        }

        HMAC_CTX *ctx = (HMAC_CTX *)hmac->impl;

        size_t buffer_len = output->capacity - output->len;

        if (buffer_len < hmac->digest_size) {
            return aws_raise_error(AWS_ERROR_SHORT_BUFFER);
        }

        if (AWS_LIKELY(HMAC_Final(ctx, output->buffer + output->len,
                                    (unsigned int *)&buffer_len))) {
            hmac->good = false;
            output->len += hmac->digest_size;
            return AWS_OP_SUCCESS;
        }

        hmac->good = false;
        return aws_raise_error(AWS_ERROR_INVALID_ARGUMENT);
    }

    inline static aws_hmac_vtable hmac256Vtable = {
        .alg_name = "SHA256 HMAC",
        .provider = "OpenSSL Compatible libcrypto",
        .destroy = destroy,
        .update = update,
        .finalize = finalize,
    };

    inline static aws_hmac_vtable hmac512Vtable = {
        .alg_name = "SHA512 HMAC",
        .provider = "OpenSSL Compatible libcrypto",
        .destroy = destroy,
        .update = update,
        .finalize = finalize,
    };

    static aws_hmac *createHmacCommon(
        aws_allocator *allocator,
        const aws_byte_cursor *secret,
        size_t digestSize,
        aws_hmac_vtable *vtable,
        const EVP_MD *(*evpMdProvider)()
    ) {
        aws_hmac *hmac = (aws_hmac *)aws_mem_acquire(allocator, sizeof(aws_hmac));

        hmac->allocator = allocator;
        hmac->vtable = vtable;
        hmac->digest_size = digestSize;
        HMAC_CTX *ctx = NULL;
        ctx = HMAC_CTX_new();

        if (!ctx) {
            aws_raise_error(AWS_ERROR_CAL_CRYPTO_OPERATION_FAILED);
            aws_mem_release(allocator, hmac);
            return NULL;
        }

        hmac->impl = ctx;
        hmac->good = true;

        if (!HMAC_Init_ex(ctx, secret->ptr, secret->len, evpMdProvider(), NULL)) {
            destroy(hmac);
            aws_raise_error(AWS_ERROR_INVALID_ARGUMENT);
            return NULL;
        }

        return hmac;
    }

public:
    static aws_hmac *createHmac256(aws_allocator *allocator, const aws_byte_cursor *secret) {
        return createHmacCommon(
            allocator,
            secret,
            AWS_SHA256_HMAC_LEN,
            &hmac256Vtable,
            EVP_sha256
        );
    }

    static aws_hmac *createHmac512(aws_allocator *allocator, const aws_byte_cursor *secret) {
        return createHmacCommon(
            allocator,
            secret,
            AWS_SHA512_HMAC_LEN,
            &hmac512Vtable,
            EVP_sha512
        );
    }
};

class ByoCryptoHash {
    static void destroy(aws_hash *hash) {
        if (hash == NULL) {
            return;
        }

        EVP_MD_CTX *ctx = (EVP_MD_CTX *)hash->impl;
        if (ctx != NULL) {
            EVP_MD_CTX_free(ctx);
        }

        aws_mem_release(hash->allocator, hash);
    }

    static int update(aws_hash *hash, const aws_byte_cursor *to_hash) {
        if (!hash->good) {
            return aws_raise_error(AWS_ERROR_INVALID_STATE);
        }

        EVP_MD_CTX *ctx = (EVP_MD_CTX *)hash->impl;

        if (AWS_LIKELY(EVP_DigestUpdate(ctx, to_hash->ptr, to_hash->len))) {
            return AWS_OP_SUCCESS;
        }

        hash->good = false;
        return aws_raise_error(AWS_ERROR_INVALID_ARGUMENT);
    }

    static int finalize(aws_hash *hash, aws_byte_buf *output) {
        if (!hash->good) {
            return aws_raise_error(AWS_ERROR_INVALID_STATE);
        }

        EVP_MD_CTX *ctx = (EVP_MD_CTX *)hash->impl;

        size_t buffer_len = output->capacity - output->len;

        if (buffer_len < hash->digest_size) {
            return aws_raise_error(AWS_ERROR_SHORT_BUFFER);
        }

        if (AWS_LIKELY(EVP_DigestFinal_ex(ctx, output->buffer + output->len,
                                            (unsigned int *)&buffer_len))) {
            output->len += hash->digest_size;
            hash->good = false;
            return AWS_OP_SUCCESS;
        }

        hash->good = false;
        return aws_raise_error(AWS_ERROR_INVALID_ARGUMENT);
    }

    inline static aws_hash_vtable md5Vtable = {
        .alg_name = "MD5",
        .provider = "OpenSSL Compatible libcrypto",
        .destroy = destroy,
        .update = update,
        .finalize = finalize,
    };

    inline static aws_hash_vtable sha512Vtable = {
        .alg_name = "SHA512",
        .provider = "OpenSSL Compatible libcrypto",
        .destroy = destroy,
        .update = update,
        .finalize = finalize,
    };

    inline static aws_hash_vtable sha256Vtable = {
        .alg_name = "SHA256",
        .provider = "OpenSSL Compatible libcrypto",
        .destroy = destroy,
        .update = update,
        .finalize = finalize,
    };

    inline static aws_hash_vtable sha1Vtable = {
        .alg_name = "SHA1",
        .provider = "OpenSSL Compatible libcrypto",
        .destroy = destroy,
        .update = update,
        .finalize = finalize,
    };

    static aws_hash *createHashCommon(
        aws_allocator *allocator,
        size_t digestSize,
        aws_hash_vtable *vtable,
        const EVP_MD *(*evpMdProvider)()
    ) {
        aws_hash *hash = (aws_hash *)aws_mem_acquire(allocator, sizeof(aws_hash));

        hash->allocator = allocator;
        hash->vtable = vtable;
        hash->digest_size = digestSize;
        EVP_MD_CTX *ctx = EVP_MD_CTX_new();
        hash->impl = ctx;
        hash->good = true;

        AWS_FATAL_ASSERT(hash->impl);

        if (!EVP_DigestInit_ex(ctx, evpMdProvider(), NULL)) {
            destroy(hash);
            aws_raise_error(AWS_ERROR_UNKNOWN);
            return NULL;
        }

        return hash;
    }

public:
    static aws_hash *md5Create(aws_allocator *allocator) {
        return createHashCommon(
            allocator,
            AWS_MD5_LEN,
            &md5Vtable,
            EVP_md5
        );
    }

    static aws_hash *sha512Create(aws_allocator *allocator) {
        return createHashCommon(
            allocator,
            AWS_SHA512_LEN,
            &sha512Vtable,
            EVP_sha512
        );
    }

    static aws_hash *sha256Create(aws_allocator *allocator) {
        return createHashCommon(
            allocator,
            AWS_SHA256_LEN,
            &sha256Vtable,
            EVP_sha256
        );
    }

    static aws_hash *sha1Create(aws_allocator *allocator) {
        return createHashCommon(
            allocator,
            AWS_SHA1_LEN,
            &sha1Vtable,
            EVP_sha1
        );
    }
};

void sm::initAwsByoCrypto() {
    OPENSSL_init_crypto(OPENSSL_INIT_ADD_ALL_CIPHERS | OPENSSL_INIT_ADD_ALL_DIGESTS, NULL);

    auto alloc = aws_default_allocator();
    aws_cal_library_init(alloc);
    aws_set_sha256_hmac_new_fn(ByoCryptoHmac::createHmac256);
    aws_set_sha512_hmac_new_fn(ByoCryptoHmac::createHmac512);

    aws_set_md5_new_fn(ByoCryptoHash::md5Create);
    aws_set_sha256_new_fn(ByoCryptoHash::sha256Create);
    aws_set_sha512_new_fn(ByoCryptoHash::sha512Create);
    aws_set_sha1_new_fn(ByoCryptoHash::sha1Create);
}
