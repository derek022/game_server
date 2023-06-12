#include <cassert>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/param_build.h>
#include <memory>

template<typename T, typename D>
std::unique_ptr<T, D> make_handle(T* handle, D deleter)
{
    return std::unique_ptr<T, D>{handle, deleter};
}

void newRSA(unsigned char *plaintext, unsigned char *ciphertext, int length, char const* modulus, char const* public_exp)
{
    // build BIGNUM values
    BIGNUM* num{ nullptr };
    BN_hex2bn(&num, modulus);
    assert(num != nullptr);
    auto bnN = make_handle(num, BN_free);
    num = nullptr;

    BN_hex2bn(&num, public_exp);
    assert(num != nullptr);
    auto bnE = make_handle(num, BN_free);

    // Build params to create PARAM array
    auto params_build = make_handle(OSSL_PARAM_BLD_new(), OSSL_PARAM_BLD_free);
    assert(params_build.get() != nullptr);

    auto result = OSSL_PARAM_BLD_push_BN(params_build.get(), "n", bnN.get());
    assert(result == 1);
    result = OSSL_PARAM_BLD_push_BN(params_build.get(), "e", bnE.get());
    assert(result == 1);
    result = OSSL_PARAM_BLD_push_BN(params_build.get(), "d", nullptr);
    assert(result == 1);

    // create PARAMS array
    auto params = make_handle(OSSL_PARAM_BLD_to_param(params_build.get()), OSSL_PARAM_free);

    // cleanup params build up
    params_build.reset();
    bnN.reset();
    bnE.reset();

    // Create RSA key from params
    auto ctx = make_handle(EVP_PKEY_CTX_new_from_name(nullptr, "RSA", nullptr), EVP_PKEY_CTX_free);
    assert(ctx.get() != nullptr);

    result = EVP_PKEY_fromdata_init(ctx.get());
    assert(result == 1);

    EVP_PKEY *key = nullptr;
    result = EVP_PKEY_fromdata(ctx.get(), &key, EVP_PKEY_KEYPAIR, params.get());
    assert(result == 1);

    auto keys = make_handle(key, EVP_PKEY_free);

    // cleanup params
    params.reset();


    // RSA_size equivalent
    int modulus_size = (EVP_PKEY_get_bits(keys.get()) + 7) / 8;
    assert(length == modulus_size);


    // setup encryption from the key generated above
    auto enc_ctx = make_handle(EVP_PKEY_CTX_new(keys.get(), nullptr), EVP_PKEY_CTX_free);
    assert(enc_ctx.get() != nullptr);

    result = EVP_PKEY_encrypt_init(enc_ctx.get());
    assert(result == 1);

    // encrypt using RSA_NO_PADDING
    result = EVP_PKEY_CTX_set_rsa_padding(enc_ctx.get(), RSA_NO_PADDING);
    assert(result == 1);

    // encrypt
    size_t outlen = length;
    result = EVP_PKEY_encrypt(enc_ctx.get(), ciphertext, &outlen, plaintext, length);
    assert(result == 1);

    assert(outlen == length);
}