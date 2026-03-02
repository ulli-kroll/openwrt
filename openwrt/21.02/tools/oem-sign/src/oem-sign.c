#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <unistd.h>

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/evp.h>

#if OPENSSL_VERSION_NUMBER < 0x10000000L
#define HAVE_ERR_REMOVE_STATE
#elif OPENSSL_VERSION_NUMBER < 0x10100000L
#define HAVE_ERR_REMOVE_THREAD_STATE
#endif

#define RSA_MAX_KEY_BITS		2048
#define RSA_MOD_LEN		(RSA_MAX_KEY_BITS/(8 * 4))

struct rsa_public_key {
	uint32_t len;		/* Length of modulus[] in number of uint32_t */
	uint32_t n0inv;		/* -1 / modulus[0] mod 2^32 */
	uint32_t modulus[RSA_MOD_LEN];	/* modulus as little endian array */
	uint32_t rr[RSA_MOD_LEN];		/* R^2 as little endian array */
}__packed;

extern int optind;
extern int opterr;
extern char *optarg;

#if (OPENSSL_VERSION_NUMBER < 0x10100005L) || defined(LIBRESSL_VERSION_NUMBER)
static void RSA_get0_key(const RSA *r,
                         const BIGNUM **n, const BIGNUM **e, const BIGNUM **d)
{
   if (n != NULL)
       *n = r->n;
   if (e != NULL)
       *e = r->e;
   if (d != NULL)
       *d = r->d;
}
#endif

static int rsa_err(const char *msg)
{
	unsigned long sslErr = ERR_get_error();

	fprintf(stderr, "%s", msg);
	fprintf(stderr, ": %s\n",
	ERR_error_string(sslErr, 0));

	return -1;
}

#if 1
static int rsa_get_pub_key(const char *path, RSA **rsap)
{
	EVP_PKEY *key;
	X509 *cert;
	RSA *rsa;
	FILE *f;
	int ret;

	*rsap = NULL;
	f = fopen(path, "r");
	if (!f) {
		fprintf(stderr, "Couldn't open RSA certificate: '%s': %s\n",
			path, strerror(errno));
		return -EACCES;
	}

	/* Read the certificate */
	cert = NULL;
	if (!PEM_read_X509(f, &cert, NULL, NULL)) {
		rsa_err("Couldn't read certificate");
		ret = -EINVAL;
		goto err_cert;
	}

	/* Get the public key from the certificate. */
	key = X509_get_pubkey(cert);
	if (!key) {
		rsa_err("Couldn't read public key\n");
		ret = -EINVAL;
		goto err_pubkey;
	}

	/* Convert to a RSA_style key. */
	rsa = EVP_PKEY_get1_RSA(key);
	if (!rsa) {
		rsa_err("Couldn't convert to a RSA style key");
		ret = -EINVAL;
		goto err_rsa;
	}
	fclose(f);
	EVP_PKEY_free(key);
	X509_free(cert);
	*rsap = rsa;

	return 0;

err_rsa:
	EVP_PKEY_free(key);
err_pubkey:
	X509_free(cert);
err_cert:
	fclose(f);
	return ret;
}
#endif

static int rsa_get_priv_key(const char *key_path, RSA **rsap)
{
	RSA *rsa;
	FILE *f;

	*rsap = NULL;
	f = fopen(key_path, "r");
	if (!f) {
		fprintf(stderr, "Couldn't open RSA private key: '%s': %s\n",
			key_path, strerror(errno));
		return -ENOENT;
	}

	rsa = PEM_read_RSAPrivateKey(f, 0, NULL, (void *)key_path);
	if (!rsa) {
		rsa_err("Failure reading private key");
		fclose(f);
		return -EPROTO;
	}
	fclose(f);
	*rsap = rsa;

	return 0;
}

static int rsa_init(void)
{
	int ret;

	ret = SSL_library_init();
	if (!ret) {
		fprintf(stderr, "Failure to init SSL library\n");
		return -1;
	}
	SSL_load_error_strings();

	OpenSSL_add_all_algorithms();
	OpenSSL_add_all_digests();
	OpenSSL_add_all_ciphers();

	return 0;
}

static void rsa_remove(void)
{
	CRYPTO_cleanup_all_ex_data();
	ERR_free_strings();
#ifdef HAVE_ERR_REMOVE_THREAD_STATE
	ERR_remove_thread_state(NULL);
#endif
#ifdef HAVE_ERR_REMOVE_STATE
	ERR_remove_state(0);
#endif
	EVP_cleanup();
}

#if 1
/*
 * rsa_get_params(): - Get the important parameters of an RSA public key
 */
static int rsa_get_params(RSA *key, uint32_t *n0_invp, BIGNUM **modulusp,
		   BIGNUM **r_squaredp)
{
	BIGNUM *big1, *big2, *big32, *big2_32;
	BIGNUM *n, *r, *r_squared, *tmp;
	BN_CTX *bn_ctx = BN_CTX_new();
	int ret = 0;

	const BIGNUM *bn_n;
	RSA_get0_key(key, &bn_n, NULL, NULL);

	/* Initialize BIGNUMs */
	big1 = BN_new();
	big2 = BN_new();
	big32 = BN_new();
	r = BN_new();
	r_squared = BN_new();
	tmp = BN_new();
	big2_32 = BN_new();
	n = BN_new();
	if (!big1 || !big2 || !big32 || !r || !r_squared || !tmp || !big2_32 ||
	    !n) {
		fprintf(stderr, "Out of memory (bignum)\n");
		return -ENOMEM;
	}

	if (!BN_copy(n, bn_n) || !BN_set_word(big1, 1L) ||
	    !BN_set_word(big2, 2L) || !BN_set_word(big32, 32L))
		ret = -1;

	/* big2_32 = 2^32 */
	if (!BN_exp(big2_32, big2, big32, bn_ctx))
		ret = -1;

	/* Calculate n0_inv = -1 / n[0] mod 2^32 */
	if (!BN_mod_inverse(tmp, n, big2_32, bn_ctx) ||
	    !BN_sub(tmp, big2_32, tmp))
		ret = -1;
	*n0_invp = BN_get_word(tmp);

	/* Calculate R = 2^(# of key bits) */
	if (!BN_set_word(tmp, BN_num_bits(n)) ||
	    !BN_exp(r, big2, tmp, bn_ctx))
		ret = -1;

	/* Calculate r_squared = R^2 mod n */
	if (!BN_copy(r_squared, r) ||
	    !BN_mul(tmp, r_squared, r, bn_ctx) ||
	    !BN_mod(r_squared, tmp, n, bn_ctx))
		ret = -1;

	*modulusp = n;
	*r_squaredp = r_squared;

	BN_free(big1);
	BN_free(big2);
	BN_free(big32);
	BN_free(r);
	BN_free(tmp);
	BN_free(big2_32);
	if (ret) {
		fprintf(stderr, "Bignum operations failed\n");
		return -ENOMEM;
	}

	return ret;
}

static int get_key_bignum(BIGNUM *num, int num_bits, uint32_t *key_mod)
{
	BIGNUM *tmp, *big2, *big32, *big2_32;
	BN_CTX *ctx;
	int ret;

	tmp = BN_new();
	big2 = BN_new();
	big32 = BN_new();
	big2_32 = BN_new();
	if (!tmp || !big2 || !big32 || !big2_32) {
		fprintf(stderr, "Out of memory (bignum)\n");
		return -1;
	}
	ctx = BN_CTX_new();
	if (!tmp) {
		fprintf(stderr, "Out of memory (bignum context)\n");
		return -1;
	}
	BN_set_word(big2, 2L);
	BN_set_word(big32, 32L);
	BN_exp(big2_32, big2, big32, ctx); /* B = 2^32 */

	for (ret = 0; ret <= 63; ret++) {
		BN_mod(tmp, num, big2_32, ctx); /* n = N mod B */
		key_mod[ret] = htonl(BN_get_word(tmp));
		BN_rshift(num, num, 32); /*  N = N/B */
	}

	BN_free(tmp);
	BN_free(big2);
	BN_free(big32);
	BN_free(big2_32);

	return 0;
}
#endif

static int oem_sign_with_key(RSA *rsa, const void *b_data,
							 const int b_data_size, const void *s_data,
							 const int s_data_size, uint8_t **sigp, uint *sig_size)
{
	EVP_PKEY *key;
	EVP_MD_CTX *context;
	int size, ret = 0;
	uint8_t *sig;

	key = EVP_PKEY_new();
	if (!key)
		return rsa_err("EVP_PKEY object creation failed");

	if (!EVP_PKEY_set1_RSA(key, rsa)) {
		ret = rsa_err("EVP key setup failed");
		goto err_set;
	}

	size = EVP_PKEY_size(key);
	sig = malloc(size);
	if (!sig) {
		fprintf(stderr, "Out of memory for signature (%d bytes)\n",
			size);
		ret = -ENOMEM;
		goto err_alloc;
	}

	context = EVP_MD_CTX_create();
	if (!context) {
		ret = rsa_err("EVP context creation failed");
		goto err_create;
	}
	EVP_MD_CTX_init(context);
	if (!EVP_SignInit(context, EVP_sha1())) {
		ret = rsa_err("Signer setup failed");
		goto err_sign;
	}

	if (!EVP_SignUpdate(context, b_data, b_data_size)) {
		ret = rsa_err("Signing bootloader failed");
		goto err_sign;
	}

	if (!EVP_SignUpdate(context, s_data, s_data_size)) {
		ret = rsa_err("Signing sysupgrade failed");
		goto err_sign;
	}

	if (!EVP_SignFinal(context, sig, sig_size, key)) {
		ret = rsa_err("Could not obtain signature");
		goto err_sign;
	}
	EVP_MD_CTX_destroy(context);
	EVP_PKEY_free(key);

	printf("Got signature: %d bytes, expected %d\n", *sig_size, size);
	*sigp = sig;
	*sig_size = size;

	return 0;

err_sign:
	EVP_MD_CTX_destroy(context);
err_create:
	free(sig);
	sig = NULL;

err_alloc:
err_set:
	EVP_PKEY_free(key);
	return ret;
}

int oem_sign(const char *private_path, const void *b_data,
				const int b_data_size, const void *s_data, const int s_data_size,
				uint8_t **sigp, uint *sig_len)
{
	RSA *rsa;
	int ret;

	ret = rsa_init();

	if (ret)
		return ret;

	ret = rsa_get_priv_key(private_path, &rsa);
	if (ret)
		goto err_priv;

	ret = oem_sign_with_key(rsa, b_data, b_data_size, s_data, s_data_size, sigp, sig_len);
	if (ret)
		goto err_sign;

	RSA_free(rsa);
	rsa_remove();

	return ret;

err_sign:
	RSA_free(rsa);
err_priv:
	rsa_remove();
	return ret;
}

static int oem_sign_img(const char *k_path, const char *bootloader_path, const char *sysupgrade_path, const char *o_path)
{
	int bootloader_fd;
	int sysupgrade_fd;
	FILE *output_fp;
	uint8_t *value = NULL;
	uint value_len;
	struct stat b_stat_buf;
	struct stat s_stat_buf;
	void *b_map;
	void *s_map;
	int ret;

	if (stat(bootloader_path, &b_stat_buf) < 0) {
		fprintf(stderr, "file %s not exist.\n", bootloader_path);
		return -1;
	}

	if (0 == S_ISREG(b_stat_buf.st_mode)) {
		fprintf(stderr, "file %s not regular file.\n", bootloader_path);
		return -1;
	}

	bootloader_fd = open(bootloader_path, O_RDONLY);
	if (bootloader_fd < 0) {
		fprintf(stderr, "can't open %s file.\n", bootloader_path);
		return -1;
	}

	b_map = mmap(NULL, b_stat_buf.st_size, PROT_READ, MAP_PRIVATE, bootloader_fd, 0);

	if (b_map == MAP_FAILED) {
		fprintf(stderr, "mmap %s failed.\n", bootloader_path);
		close(bootloader_fd);
		return -1;
	}

	if (stat(sysupgrade_path, &s_stat_buf) < 0) {
		fprintf(stderr, "file %s not exist.\n", sysupgrade_path);
		goto err_s_open;
	}

	if (0 == S_ISREG(s_stat_buf.st_mode)) {
		fprintf(stderr, "file %s not regular file.\n", sysupgrade_path);
		goto err_s_open;
	}

	sysupgrade_fd = open(sysupgrade_path, O_RDONLY);
	if (sysupgrade_fd < 0) {
		fprintf(stderr, "can't open %s file.\n", sysupgrade_path);
		goto err_s_open;
	}

	s_map = mmap(NULL, s_stat_buf.st_size, PROT_READ, MAP_PRIVATE, sysupgrade_fd, 0);

	if (s_map == MAP_FAILED) {
		fprintf(stderr, "mmap file failed.\n");
		goto err_s_map;
	}

	ret = oem_sign(k_path, b_map, b_stat_buf.st_size, s_map, s_stat_buf.st_size, &value, &value_len);

	if (ret) {
		printf("Failed to sign signature node: %d\n", ret);
		goto err_s_map;
	}
	ret = -1;

	output_fp = fopen(o_path, "w+");

	if (!output_fp) {
		fprintf(stderr, "open %s file failed.\n", o_path);
		goto err_out;
	}

#if 0
	if (fwrite(key, sizeof(struct rsa_public_key), 1, output_fp) < 0) {
		fprintf(stderr, "write public key to %s file failed.\n", o_path);
		fclose(output_fp);
		goto err_out;
	}
#endif

	if (fwrite(value, 1, value_len, output_fp) < 0) {
		fprintf(stderr, "write signature value to %s file failed.\n", o_path);
		fclose(output_fp);
		goto err_out;
	}

	ret = 0;

err_out:
	munmap(s_map, s_stat_buf.st_size);

err_s_map:
	close(sysupgrade_fd);

err_s_open:
	munmap(b_map, b_stat_buf.st_size);
	close(bootloader_fd);

	if (value)
		free(value);

	return ret;
}

static int oem_sign_with_key_single(RSA *rsa, const void *data, const int data_size,
					const void *b_name, const int b_size, uint8_t **sigp, uint *sig_size)
{
	EVP_PKEY *key;
	EVP_MD_CTX *context;
	int size, ret = 0;
	uint8_t *sig;

	key = EVP_PKEY_new();
	if (!key)
		return rsa_err("EVP_PKEY object creation failed");

	if (!EVP_PKEY_set1_RSA(key, rsa)) {
		ret = rsa_err("EVP key setup failed");
		goto err_set;
	}

	size = EVP_PKEY_size(key);
	sig = malloc(size);
	if (!sig) {
		fprintf(stderr, "Out of memory for signature (%d bytes)\n",
			size);
		ret = -ENOMEM;
		goto err_alloc;
	}

	context = EVP_MD_CTX_create();
	if (!context) {
		ret = rsa_err("EVP context creation failed");
		goto err_create;
	}
	EVP_MD_CTX_init(context);
	if (!EVP_SignInit(context, EVP_sha1())) {
		ret = rsa_err("Signer setup failed");
		goto err_sign;
	}

	if (!EVP_SignUpdate(context, data, data_size)) {
		ret = rsa_err("Signing data failed");
		goto err_sign;
	}

	if (!EVP_SignUpdate(context, b_name, b_size)) {
		ret = rsa_err("Signing board name failed");
		goto err_sign;
	}

	if (!EVP_SignFinal(context, sig, sig_size, key)) {
		ret = rsa_err("Could not obtain signature");
		goto err_sign;
	}
	EVP_MD_CTX_destroy(context);
	EVP_PKEY_free(key);

	*sigp = sig;
	*sig_size = size;

	return 0;

err_sign:
	EVP_MD_CTX_destroy(context);
err_create:
	free(sig);
err_alloc:
err_set:
	EVP_PKEY_free(key);
	return ret;
}

int oem_sign_single(const char *private_path, const void *data, const int size,
			 const void *b_name, const int b_size, uint8_t **sigp, uint *sig_len)
{
	RSA *rsa;
	int ret;

	ret = rsa_init();

	if (ret)
		return ret;

	ret = rsa_get_priv_key(private_path, &rsa);
	if (ret)
		goto err_priv;

	ret = oem_sign_with_key_single(rsa, data, size, b_name, b_size, sigp, sig_len);
	if (ret)
		goto err_sign;

	RSA_free(rsa);
	rsa_remove();

	return ret;

err_sign:
	RSA_free(rsa);
err_priv:
	rsa_remove();
	return ret;
}

static int oem_sign_file(const char *k_path, const char *i_path, const char *o_path, const char *board_name)
{
	int input_fd;
	FILE *output_fp;
	uint8_t *value;
	uint value_len;
	struct stat stat_buf;
	void *i_map;
	int ret;
	char b_name[16];

	if (stat(i_path, &stat_buf) < 0) {
		fprintf(stderr, "file %s not exist.\n", i_path);
		return -1;
	}

	if (0 == S_ISREG(stat_buf.st_mode)) {
		fprintf(stderr, "file %s not regular file.\n", i_path);
		return -1;
	}

	input_fd = open(i_path, O_RDONLY);
	if (input_fd < 0) {
		fprintf(stderr, "can't open %s file with read mode.\n", i_path);
		return -1;
	}

	i_map = mmap(NULL, stat_buf.st_size, PROT_READ, MAP_PRIVATE, input_fd, 0);

	if (i_map == MAP_FAILED) {
		fprintf(stderr, "mmap file failed.\n");
		close(input_fd);
		return -1;
	}

	memset(b_name, 0, sizeof(b_name));

	if (board_name) {
		/*
		 * board name: 16
		 */
		strncpy(b_name, board_name, sizeof(b_name) - 1);
	}

	ret = oem_sign_single(k_path, i_map, stat_buf.st_size, b_name, sizeof(b_name), &value, &value_len);

	if (ret) {
		printf("Failed to sign signature node: %d\n", ret);
		munmap(i_map, stat_buf.st_size);
		close(input_fd);
		return -1;
	}
	ret = -1;

	output_fp = fopen(o_path, "w+");

	if (!output_fp) {
		fprintf(stderr, "open %s file failed.\n", o_path);
		goto err_out;
	}

#if 0
	if (fwrite(key, sizeof(struct rsa_public_key), 1, output_fp) < 0) {
		fprintf(stderr, "write public key to %s file failed.\n", o_path);
		goto err_out_out;
	}
#endif

	if (fwrite(b_name, 1, sizeof(b_name), output_fp) <= 0) {
		fprintf(stderr, "write board name to %s file failed.\n", o_path);
		goto err_out_out;
	}

	if (fwrite(value, 1, value_len, output_fp) <= 0) {
		fprintf(stderr, "write signature value to %s file failed.\n", o_path);
		goto err_out_out;
	}

	ret = 0;

err_out_out:
	fclose(output_fp);

err_out:
	munmap(i_map, stat_buf.st_size);
	close(input_fd);
	free(value);

	return ret;
}

#if 1
static int get_pub_key(const char *path, struct rsa_public_key *key)
{
	BIGNUM *modulus, *r_squared;
	uint32_t n0_inv;
	int ret;
	int bits;
	RSA *rsa;
	int i;

	ret = rsa_get_pub_key(path, &rsa);
	if (ret)
		return ret;
	ret = rsa_get_params(rsa, &n0_inv, &modulus, &r_squared);
	if (ret)
		return ret;

	bits = BN_num_bits(modulus);
	get_key_bignum(modulus, bits, key->modulus);
	get_key_bignum(r_squared, bits, key->rr);

	key->len = htonl(bits);
	key->n0inv = htonl(n0_inv);

	printf("%u\n", key->len);
	printf("%u\n", key->n0inv);
	printf("modules:\n");
	for (i = 0; i < RSA_MOD_LEN; i++) {
		if (i%8 == 0)
			printf("\n");
		else
			printf("%u, ", key->modulus[i]);
	}

	printf("\nrr:\n");
	for (i = 0; i < RSA_MOD_LEN; i++) {
		if (i%8 == 0)
			printf("\n");
		else
			printf("%u, ", key->rr[i]);
	}

	BN_free(modulus);
	BN_free(r_squared);

	return 0;
}
#endif

static void usage(void)
{
	fprintf(stderr, "Usage: oem-sign -b|-s|-o|-k|-c|-i\n");
	fprintf(stderr, "                       -b [file]: set bootloader file\n");
	fprintf(stderr, "                       -s [file]: set sysupgrade file\n");
	fprintf(stderr, "                       -i [file]: set input file\n");
	fprintf(stderr, "                       -o [file]: set output file\n");
	fprintf(stderr, "                       -n [name]: set board name\n");
	fprintf(stderr, "                       -k [file]: set the openssl rsa private key file\n");

	exit(1);
}

int main(int argc, char *argv[])
{
	int op;
	char *sysupgrade_file = NULL;
	char *bootloader_file = NULL;
	char *input_file = NULL;
	char *output_file = NULL;
	char *board_name = NULL;

#if 1
	struct rsa_public_key pub_key;
	char *cert_path = NULL;
#endif

	char *key_path = NULL;

	while ((op = getopt(argc, argv, "b:i:s:o:k:n:c:")) != -1) {
		switch (op) {
		case 'b':
			if (optarg[0] == '0' && optarg[1] == 0) {
				fprintf(stderr, "booloader file format error.\n");
				usage();
			}

			bootloader_file = optarg;
			break;

		case 's':
			if (optarg[0] == '0' && optarg[1] == 0) {
				fprintf(stderr, "sysupgrade file format error.\n");
				usage();
			}

			sysupgrade_file = optarg;
			break;

		case 'i':
			if (optarg[0] == '0' && optarg[1] == 0) {
				fprintf(stderr, "input file format error.\n");
				usage();
			}

			input_file = optarg;
			break;

		case 'o':
			if (optarg[0] == '0' && optarg[1] == 0) {
				fprintf(stderr, "output file format error.\n");
				usage();
			}

			output_file = optarg;
			break;

		case 'k':
			if (optarg[0] == '0' && optarg[1] == 0) {
				fprintf(stderr, "rsa key file format error.\n");
				usage();
			}

			key_path = optarg;
			break;

		case 'n':
			if (optarg[0] == '0' && optarg[1] == 0) {
				fprintf(stderr, "board name error.\n");
				usage();
			}

			board_name = optarg;
			break;
#if 1
		case 'c':
			if (optarg[0] == '0' && optarg[1] == 0) {
				fprintf(stderr, "rsa cert file format error.\n");
				usage();
			}

			cert_path = optarg;
			break;
#endif

		default:
			usage();
		}
	}

	if (!output_file)
		usage();

#if 1
	if (cert_path) {
		if (get_pub_key(cert_path, &pub_key) != 0) {
			fprintf(stderr, "get public key from %s file failed.\n", cert_path);
			usage();
		}
	}
#endif

	if (input_file) {
		if (oem_sign_file(key_path, input_file, output_file, board_name) != 0) {
			fprintf(stderr, "signature file failed\n");
			usage();
		}
	} else {
		if (oem_sign_img(key_path, bootloader_file, sysupgrade_file, output_file) != 0) {
			fprintf(stderr, "signature file failed\n");
			usage();
		}
	}

	return 0;
}
