#============================================================================
#
# Certificate Generation / Image Signing and Packing
#
# GENERAL DESCRIPTION
# Sample usage:
#  python secure-boot-tools.py --gen_cert_chain --gen_fuse_bin --chipset=chipid --out_path <OUT_DIR>
#  python secure-boot-tools.py --sign_pack_image --binary --chipset=chipid --license=license_file --out_path <OUT_DIR>
#  python secure-boot-tools.py --sign_pack_image --src_pd=pd0,pd1 --chipset=chipid --license=license_file --out_path <OUT_DIR>
#  python secure-boot-tools.py --sign_pack_image --src_pd=pd0,pd1 --chipset=chipid --license=license_file --out_path <OUT_DIR>
#  python secure-boot-tools.py --sign_pack_image --chipset=chipid --out_path <OUT_DIR>
#
# Copyright (c) 2023 by Qualcomm Technologies, Inc.
# All Rights Reserved.
# Qualcomm Technologies, Inc. Confidential and Proprietary
#
#----------------------------------------------------------------------------
import os
import argparse
import subprocess

#--------------------------------------------------------------------------------
# OEM Specific Configs starts here
#--------------------------------------------------------------------------------
sectools_path='/pkg/sectools/v2/latest/Linux/sectools'
mksquashfs_path='/pkg/software/ubuntu/mksquashfs/mksquashfs4'
security_profile_tz_file = './signing/Miami_tz_security_profile.xml'
security_profile_file = './signing/miami_v1_security_profile.xml'

oem_id="0xABCD"
oem_product_id="0x5332"
country="US"
state="California"
location="SanDiego"
organization="CUSTOMER"
organization_unit="DepartmentCUSTOMER"
common_name="www.customer.com"
#--------------------------------------------------------------------------------
# OEM Specific Configs ends here
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
# Initialize parser
#--------------------------------------------------------------------------------
parser = argparse.ArgumentParser(description="Script to generate custom OEM certificates, the fuse-blower binary, and OEM sign the images with the generated certificates")
arglist = parser.add_argument_group('List of supported arguments')

#Adding arguments
arglist.add_argument("--chipset", type=str, help = "Select chipset [ipq5322,ipq5332,ipq5332+qcn9160,ipq5322+qcn9160]")
arglist.add_argument("--gen_cert_chain", help = "Include this argument to generate the Root CA / Attest CA certificate", action='store_true')
arglist.add_argument("--gen_fuse_bin", help = "Include this argument to generate the fuse blower binary", action='store_true')
arglist.add_argument("--binary", help = "To double sign QTI signed binaries, include this. If FW is build from source, this is not needed", action='store_true')
arglist.add_argument("--src_pd", help = "To OEM sign the source generated user-pd and double sign QTI signed binaries of root-pd")
arglist.add_argument("--sign_pack_image", help = "Include this argument to OEM sign and pack as squashfs images", action='store_true')
arglist.add_argument("--openssl_cfg_path", help = "Path to the directory which contains the openssl config files, namely v3.ext, v3_attest.ext,opensslroot.cfg and config.xml")
arglist.add_argument("--out_path", help = "Path to the output folder")
arglist.add_argument("--license", help = "To pass the license file to be packed with the final squashfs")

#Read arguments from command line
args = parser.parse_args()
#-------------------------------------------------------------------------------
# Definitions
#-------------------------------------------------------------------------------
def log_msg(type,msg):
    if type == 'ERROR':
        print('{0:<5}:{1}'.format(type,msg))
        exit(0)
    elif type in ['WARN','INFO']:
        print('{0:<5}:{1}'.format(type,msg))
    else:
        print('{0}{1}'.format('!'*5,msg))

def gen_cert_chain(out_dir, openssl_dir):
    '''
    Generates root certificates and CA certificate

    '''
    subject_root ='/C="{}"/ST="{}"/L="{}"/O="{}"/OU="{}"/CN="{}"'.format(country,state,location,organization,organization_unit,common_name)
    subject_CA ='/C="{}"/ST="{}"/L="{}"/O="{}"/OU="{}"/CN="{}"'.format(country,state,location,organization,organization_unit,common_name+'CA')
    cert_key = os.path.join(out_dir, "qpsa_rootca0.key")
    cert_der = os.path.join(out_dir, "qpsa_rootca0.cer")
    cert_out = os.path.join(out_dir, "rootca_pem0.crt")
    cert_has = os.path.join(out_dir, "pkhash.txt")

    cert_key_att = os.path.join(out_dir,"qpsa_attestca.key")
    cert_csr_att = os.path.join(out_dir, "attestca.csr")
    cert_out_att = os.path.join(out_dir, "attestca_pem.crt")

    openssl_cfg = os.path.join(openssl_dir,"opensslroot.cfg")
    v3_ext_path = os.path.join(openssl_dir,"v3.ext")

    gen_root_cert = 'openssl req -new -x509 -nodes -newkey ec:<(openssl ecparam -name secp384r1) -keyout {0} -out {1} -subj {2} -days 3650 -sha384 -config {3}'.format(cert_key,cert_out,subject_root,openssl_cfg)
    gen_root_cert_der = 'openssl x509 -in {0} -inform PEM -out {1} -outform DER'.format(cert_out, cert_der)
    gen_root_cert_hash = 'openssl dgst -sha384 {0} > {1}'.format(cert_der, cert_has)
    
    gen_csr = 'openssl req -new -nodes -newkey ec:<(openssl ecparam -name secp384r1) -keyout {0} -out {1} -subj {3} -days 3650 -config {2}'.format(cert_key_att, cert_csr_att, openssl_cfg, subject_CA)
    gen_attest_ca = 'openssl x509 -req -in {0} -CAcreateserial -CA {1} -CAkey {2} -out {3} -days 3650 -sha384 -extfile {4}'.format(cert_csr_att, cert_out, cert_key, cert_out_att, v3_ext_path)
    gen_attest_der = 'openssl x509 -in {0} -inform PEM -out {1} -outform DER'.format(os.path.join(out_dir, 'attestca_pem.crt'), os.path.join(out_dir, 'qpsa_attestca.cer'))

    log_msg('INFO','Root Certificate Generation')
    scmd = subprocess.Popen(gen_root_cert, shell=True, executable='/bin/bash')
    scmd.wait()
    scmd = subprocess.Popen(gen_root_cert_der, shell=True, executable='/bin/bash')
    scmd.wait()
    scmd = subprocess.Popen(gen_root_cert_hash, shell=True, executable='/bin/bash')
    scmd.wait()
    
    log_msg('INFO','CA Certificate attestation')
    scmd = subprocess.Popen(gen_csr, shell=True, executable='/bin/bash')
    scmd.wait()
    scmd = subprocess.Popen(gen_attest_ca, shell=True, executable='/bin/bash')
    scmd.wait()
    scmd = subprocess.Popen(gen_attest_der, shell=True, executable='/bin/bash')
    scmd.wait()

    if os.path.exists(cert_out) and os.path.exists(cert_out_att):
        return (cert_out, gen_root_cert_der, gen_root_cert_hash, cert_csr_att, cert_out_att, cert_key_att)
    else:
        return (None)

def gen_fuse_bin(base_dir):
    '''
    Genrate sec.dat
    '''
    sec_dat = os.path.join(base_dir, 'sec.dat')
    sec_dat_inspect = os.path.join(base_dir, 'sec_dat_inspect.txt')

    log_msg('INFO','Generating Fuse Blower Binary')
    root_cert_pk_hash = os.popen('sha384sum {0}'.format(os.path.join(base_dir,'../generated_certs', 'qpsa_rootca0.cer'))).read()
    root_cert_pk_hash = root_cert_pk_hash.split(" ")

    log_msg('INFO','Root CA PK hash:{}'.format(root_cert_pk_hash[0]))
    secdat_gen_cmd='{0} fuse-blower --security-profile {1} --generate --outfile {2} --fuse-oemsecuritypolicy=0x41 --fuse-tme-oem-mrc-hash=0x{3}'.format(sectools_path, security_profile_tz_file,sec_dat,root_cert_pk_hash[0])
    secdat_inspect_cmd='{0} fuse-blower --security-profile {1} {2} --inspect > {3}'.format(sectools_path, security_profile_tz_file, sec_dat, sec_dat_inspect)

    oem_id_sec_dat = " --fuse-oemid=" + oem_id
    if oem_product_id != "0x0":
       	oem_id_sec_dat = oem_id_sec_dat + " --fuse-oemproductid=" + oem_product_id
    secdat_gen_cmd = secdat_gen_cmd + oem_id_sec_dat

    os.system(secdat_gen_cmd)
    os.system(secdat_inspect_cmd)
    
    if os.path.exists(sec_dat):
        return (sec_dat, sec_dat_inspect)
    else:
        return (None)

def oem_sign_and_pack_ipq53xx(base_dir):
    '''
    OEM sign the images with the generated certificates
    '''
    root_cert = os.path.join(base_dir,'generated_certs', 'qpsa_rootca0.cer')
    ca_cert   = os.path.join(base_dir,'generated_certs', 'qpsa_attestca.cer')
    ca_key    = os.path.join(base_dir,'generated_certs', 'qpsa_attestca.key')
    if not (os.path.exists(root_cert) and os.path.exists(ca_cert) and os.path.exists(ca_key)):
	    log_msg('ERROR','Missing certificate. Please ensure below files are available.\n{}\n{}\n{}'.format(root_cert, ca_cert, ca_key))

    if os.path.exists(os.path.join(base_dir,'signing','img_pack_builder.py')):
        pack_script = os.path.join(base_dir,'signing','img_pack_builder.py')
    elif os.path.exists(os.path.join(os.path.dirname(__file__),'img_pack_builder.py')):
        pack_script = os.path.join(os.path.dirname(__file__),'img_pack_builder.py')
    else:
        log_msg('ERROR','img_pack_builder.py is not found,({},{})'.format(os.path.join(base_dir,'signing','img_pack_builder.py'),os.path.join(os.path.dirname(__file__),'img_pack_builder.py')))

    signing_cmd = sectools_path + " secure-image {inf} --outfile {outf} --security-profile {prof} --image-id {id} --secondary-software-id 0x{secId} --sign --signing-mode LOCAL --root-certificate {root_cert} --ca-certificate {ca_cert} --ca-key {ca_key} --oem-id=" + oem_id + " --oem-product-id=" + oem_product_id
    inspect_cmd = sectools_path + " secure-image {out_img} --inspect > {inspect_output_path}"

    #Base directories
    if not os.path.exists(base_dir):
        img_path = os.path.dirname(__file__) + '/../FW_IMAGES'
        bin_path = os.path.dirname(__file__) + '/../BIN'
    else:
        img_path = os.path.abspath(base_dir) + '/FW_IMAGES'
        bin_path = os.path.abspath(base_dir) + '/BIN'

    if os.path.exists(bin_path):
        if args.chipset in ['ipq5332','ipq5322']:
            imgs = [[bin_path + '/qdsp6_rpd_prod_signed.mbn', 'Q6-RPD', 0x0, bin_path + '/qdsp6_rpd_oem_signed.mbn']]
            if args.binary is True:
                imgs.append([bin_path + '/qdsp6_user_pd0_prod_signed.mbn', 'Q6-UPD', 0x1, bin_path + '/qdsp6_user_pd0_oem_signed.mbn']) #ipq5332 user-pd
            else:
                imgs.append([bin_path + '/qdsp6_user_pd0.mbn', 'Q6-UPD-OEM', 0x1, bin_path + '/qdsp6_user_pd0_oem_signed.mbn']) #ipq5332 user-pd
            imgs.append([bin_path + '/phy_ucode_prod_signed.elf', 'IU', 0x1, bin_path + '/phy_ucode_oem_signed.elf']) #ucode

        elif args.chipset in ['ipq5332+qcn9160','ipq5322+qcn9160']:
            imgs = [[bin_path + '/qdsp6_rpd_prod_signed.mbn', 'Q6-RPD', 0x0, bin_path + '/qdsp6_rpd_oem_signed.mbn']]
            #By default added the Binary images and replace based on src_pd
            imgs.append([bin_path + '/qdsp6_user_pd0_prod_signed.mbn', 'Q6-UPD', 0x1, bin_path + '/qdsp6_user_pd0_oem_signed.mbn'])
            imgs.append([bin_path + '/qdsp6_user_pd1.mbn', 'Q6-UPD-OEM', 0x2, bin_path + '/qdsp6_user_pd1_oem_signed.mbn'])
            if args.src_pd is not None:
                src_pds = args.src_pd.split(',')
                if "pd0" in src_pds:
                    imgs[1]= ([bin_path + '/qdsp6_user_pd0.mbn', 'Q6-UPD-OEM', 0x1, bin_path + '/qdsp6_user_pd0_oem_signed.mbn'])
                elif "pd1" in src_pds:
                    imgs[2] = ([bin_path + '/qdsp6_user_pd1.mbn', 'Q6-UPD-OEM', 0x2, bin_path + '/qdsp6_user_pd1_oem_signed.mbn'])
            imgs.append([bin_path + '/phy_ucode_prod_signed.elf', 'IU', 0x1, bin_path + '/phy_ucode_oem_signed.elf'])
        elif args.chipset in ['ipq5332-tmel','ipq5322-tmel']:
            imgs = [[bin_path + 'tmel-ipq53xx-patch_signed.elf', 'TMEL-RAM-PATCH', 0x0, bin_path + 'tmel-ipq53xx-patch_oem_signed.elf']]

    else:
        log_msg('ERROR','Source image for signing is not found:{}'.format(bin_path))

    for img in imgs:
        scmd = signing_cmd.format(inf=img[0], outf=img[3], prof=security_profile_file, id=img[1], secId=img[2], root_cert=root_cert, ca_cert=ca_cert, ca_key=ca_key)
        icmd = inspect_cmd.format(out_img=img[3], inspect_output_path=img[3]+'_inspect.txt')

        os.system(scmd)
        os.system(icmd)

        if not os.path.exists(img[3]):
    	    log_msg('ERROR','OEM signing failed, {}'.format(img[3]))

    #Pack signed images back into squashfs
    pds = ''
    for img in imgs[:-1]:
        pds = pds + os.path.abspath(img[3]) + ','

    iu = os.path.abspath(imgs[-1][3])
    
    license_file = ''
    if args.license is not None:
        license_file = os.path.abspath(args.license)
        if not os.path.exists(license_file):
            print("Error: license_file {} not found".format(license_file))
            exit(1)

    pack_cmd = 'python {} --base_dir={} --chipset={} --ops=pack --oem_pds={} --iu={} --license={}'.format(pack_script, img_path, args.chipset, pds[:-1], iu, license_file)
    os.system(pack_cmd)

    return (bin_path, img_path)

#==============================================================================#
#==============================================================================#
#Sanity check on the inputs given
if args.gen_cert_chain is False and args.sign_pack_image is False and args.gen_fuse_bin is False:
    log_msg('ERROR','Provide valid argument: --gen_cert_chain or --sign_pack_image or --gen_fuse_bin')

if args.openssl_cfg_path is None:
    args.openssl_cfg_path=os.path.dirname(security_profile_file)

if args.out_path is None:
    log_msg('ERROR','Missing argument --out_path')

if not os.path.exists(args.out_path):
    os.system("mkdir -p {0}".format(args.out_path))

if args.gen_cert_chain is True:
    #1. Generate root certificates
    #2. Generate CA certificate
    if args.openssl_cfg_path is None:
       	log_msg('ERROR','Missing argument --openssl_cfg_path')
    elif not os.path.exists(os.path.join(args.openssl_cfg_path,"v3.ext")):
        log_msg('ERROR','Missing file v3.ext in the openssl config files path')
    elif not os.path.exists(os.path.join(args.openssl_cfg_path,"v3_attest.ext")):
        log_msg('ERROR','Missing file v3_attest.ext in the openssl config files path')
    elif not os.path.exists(os.path.join(args.openssl_cfg_path,"opensslroot.cfg")):
	log_msg('ERROR','Missing file opensslroot.cfg in the openssl config files path')
    elif not os.path.exists(os.path.join(args.openssl_cfg_path,"config.xml")):
	log_msg('ERROR','Missing file config.xml in the openssl config files path')

    cert_out_path = os.path.join(args.out_path,"generated_certs")
    if os.path.exists(cert_out_path):
	log_msg('WARN','Certificates output folder already exists.')
        log_msg('PROMPT','Do you want to overwrite (Y/N)?')
	yes = {'yes', 'y'}
	no = {'no', 'n'}
	choice = raw_input().lower()
	if choice in yes:
            log_msg('MSG','Deleteing the existing certificate folders')
	    os.system("rm -rf {0}".format(cert_out_path))
	else:
	    log_msg('ERROR','Aborting Certificate Generation')

    os.system('mkdir -p '+ cert_out_path)
    certs = gen_cert_chain(cert_out_path, args.openssl_cfg_path)

    if os.path.exists(certs[0]):
	log_msg('MSG','Root and CA ertificates generated in:{}'.format(cert_out_path))
    else:	
	log_msg('ERROR','Certificate Generation Failied')

if args.gen_fuse_bin is True:
    #3. Generate sec.dat binary
    fusebin_out_path = os.path.join(args.out_path,"fuseblower_bin")
    if not os.path.exists(fusebin_out_path):
        os.system("mkdir -p {0}".format(fusebin_out_path))

    sec_dat = gen_fuse_bin(fusebin_out_path)

    if os.path.exists(sec_dat[0]):
        log_msg('MSG','Fuse Blower Binary Generated in:{}'.format(sec_dat))
    else:	
	log_msg('ERROR','Fuse Blower Binary Generation Failied')

if args.sign_pack_image is True:
    #4. OEM sign the images with the generated certificates
    log_msg('INFO','Signing the images with OEM Certificates')

    if args.chipset in ['ipq5332','ipq5322', 'ipq5332+qcn9160','ipq5322+qcn9160']:
        out = oem_sign_and_pack_ipq53xx(args.out_path)
        log_msg('MSG','OEM Signed binaries available at:{}'.format(out[0]))
        log_msg('MSG','OEM Packed Images available at:{}'.format(out[1]))
