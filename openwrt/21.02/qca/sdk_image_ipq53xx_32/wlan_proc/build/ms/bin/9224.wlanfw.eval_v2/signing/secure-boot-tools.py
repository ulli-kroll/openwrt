#============================================================================
#
# Certificate Generation / Image Signing and Packing
#
# GENERAL DESCRIPTION
#
# Copyright (c) 2022 by Qualcomm Technologies, Inc.
# All Rights Reserved.
# Qualcomm Technologies, Inc. Confidential and Proprietary
# QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------

import argparse
import os

#Setup OEM Specific Configs
#--------------------------------------------------------------------------------
sectools_path="/pkg/sectools/v2/latest/Linux/sectools"
mksquashfs_path="/pkg/software/ubuntu/mksquashfs/mksquashfs4"

qti_signed_images_path="/home/fw-src-2903/from-bin/BIN"
security_profile_path="/home/fw-src-2903/from-bin/signing"
wlan_fw_src_path="/home/fw-src-2903/SRC-WLAN"

oem_id="0xABCD"
oem_product_id="0x0"
country="US"
state="California"
location="San Diego"
organization="CUSTOMER"
organization_unit="Department or Bussiness unit at CUSTOMER"
common_name="www.customer.com"
#--------------------------------------------------------------------------------


# Initialize parser
parser = argparse.ArgumentParser(description="Script to generate custom OEM certificates,the fuseblower binary and to OEM sign the WKKv2 images with the generated certificates.")

arglist = parser.add_argument_group('List of supported arguments')

#Adding arguments
arglist.add_argument("--gen_cert_chain", help = "Include this argument to generate the Root CA / Attest CA certificate", action='store_true')
arglist.add_argument("--gen_fuse_bin", help = "Include this argument to generate the fuse blower binary", action='store_true')
arglist.add_argument("--binary", help = "Include this argument to indicate the FW is binary release and not built from source", action='store_true')
arglist.add_argument("--sign_pack_image", help = "Include this argument to OEM sign the WKKv2 images", action='store_true')
arglist.add_argument("--openssl_cfg_path", help = "Path to the directory which contains the openssl config files, namely v3.ext, v3_attest.ext,opensslroot.cfg and config.xml")
arglist.add_argument("--dual_mac", help = "Indicates that the build is dual MAC", action='store_true')
arglist.add_argument("--out_path", help = "Path to the output folder")



# Read arguments from command line
args = parser.parse_args()

#Sanity check on the inputs given
if args.gen_cert_chain is False and args.sign_pack_image is False and args.gen_fuse_bin is False:
        print("Provide valid argument: --gen_cert_chain or --sign_pack_image or --gen_fuse_bin")
	exit(0)


if args.openssl_cfg_path is None:
        args.openssl_cfg_path=security_profile_path


fuse_blower_bin_path=security_profile_path + "/fuse_blower.bin"

if not os.path.exists(security_profile_path):
	print("Invalid path given to security_profile_path config")
	exit(0)

if args.out_path is None:
        print("Missing argument --out_path")
        exit(0)
elif not os.path.exists(args.out_path):
	ret=os.system("mkdir -p {0}".format(args.out_path))
	
	if not os.path.exists(args.out_path):
		print("Invalid folder path given to --out_path argument")
		exit(0)

if args.gen_fuse_bin is True:
	print fuse_blower_bin_path
	if fuse_blower_bin_path is None:
        	print("Inavlid fuse_blower_bin_path")
        	exit(0)
	elif not os.path.exists(fuse_blower_bin_path):
        	print("Invalid path in fuse_blower_bin_path")
        	exit(0)

if args.gen_cert_chain is True:
	if args.openssl_cfg_path is None:
        	print("Missing argument --openssl_cfg_path")
        	exit(0)
	else:
		if not os.path.exists(os.path.join(args.openssl_cfg_path,"v3.ext")):
			print("Missing file v3.ext in the openssl config files path")
			exit(0)
		elif not os.path.exists(os.path.join(args.openssl_cfg_path,"v3_attest.ext")):
			print("Missing file v3_attest.ext in the openssl config files path")
			exit(0)
		elif not os.path.exists(os.path.join(args.openssl_cfg_path,"opensslroot.cfg")):
			print("Missing file opensslroot.cfg in the openssl config files path")
			exit(0)
		elif not os.path.exists(os.path.join(args.openssl_cfg_path,"config.xml")):
			print("Missing file config.xml in the openssl config files path")
			exit(0)

if args.sign_pack_image is True:
	if qti_signed_images_path is None:
        	print("Missing config qti_signed_images_path")
        	exit(0)
	else:
		if not os.path.exists(os.path.join(qti_signed_images_path,"SBL_RDDM_MERGED_V2_PROD_signed.bin")):
			print("Missing image SBL_RDDM_MERGED_V2_PROD_signed.bin in the QTI signed images directory")
			exit(0)
		elif not os.path.exists(os.path.join(qti_signed_images_path,"phy_ucode_signed.elf")):
			print("Missing image phy_ucode_signed.elf in the QTI signed images directory")
			exit(0)

	oem_wlan_fw_path_dual=wlan_fw_src_path + "/wlan_proc/build/ms/bin/9224.wlanfw.eval_v2_2mac"

	if args.binary is True:
		oem_wlan_fw_dual=oem_wlan_fw_path_dual + "/BIN/CNSS_RAM_V2_PROD_signed.bin"
	else:
		oem_wlan_fw_dual=oem_wlan_fw_path_dual + "/CNSS_RAM_V2_TO_LINK_PATCHED_9224.wlanfw.eval_v2_2macQ.mbn"
	
	oem_wlan_fw_path_single=wlan_fw_src_path + "/wlan_proc/build/ms/bin/9224.wlanfw.eval_v2"

	if args.binary is True:
		oem_wlan_fw_single=oem_wlan_fw_path_single + "/BIN/CNSS_RAM_V2_PROD_signed.bin"
	else:
		oem_wlan_fw_single=oem_wlan_fw_path_single + "/CNSS_RAM_V2_TO_LINK_PATCHED_9224.wlanfw.eval_v2Q.mbn"

	oem_wlan_fw_path_single_dual=wlan_fw_src_path + "/wlan_proc/build/ms/bin/9224.wlanfw.single_dualmac_v2"

	if args.dual_mac is True:
		oem_wlan_fw_path=oem_wlan_fw_path_dual
		oem_wlan_fw=oem_wlan_fw_dual
	else:
		oem_wlan_fw_path=oem_wlan_fw_path_single
		oem_wlan_fw=oem_wlan_fw_single

if args.sign_pack_image is True:
	cert_chain_path=args.out_path + "/generated_certs"

	if not os.path.exists(os.path.join(cert_chain_path,"qpsa_rootca0.cer")):
		print("Missing qpsa_rootca0.cer in the certificate chain path directory")
		exit(0)
	elif not os.path.exists(os.path.join(cert_chain_path,"qpsa_attestca.cer")):
		print("Missing qpsa_attestca.cer  in the certificate chain path directory")
		exit(0)
	elif not os.path.exists(os.path.join(cert_chain_path,"qpsa_attestca.key")):
		print("Missing image qpsa_attestca.key in the certificate chain path directory")
		exit(0)


if args.gen_cert_chain is True:
	openssl_cfg_path = os.path.join(args.openssl_cfg_path,"opensslroot.cfg")
	v3_ext_path = os.path.join(args.openssl_cfg_path,"v3.ext")
cert_out_path = os.path.join(args.out_path,"generated_certs")
if args.gen_fuse_bin is True:
	fusebin_out_path = os.path.join(args.out_path,"fuseblower_bin")
oem_signed_img_path = os.path.join(args.out_path,"OEM_SIGNED_IMAGES")
#Create the required output folders
if args.gen_cert_chain is True:
	if os.path.exists(os.path.join(cert_out_path,"rootca_pem0.crt")) or os.path.exists(os.path.join(cert_out_path,"attestca_pem.crt")):
		print("!!!!!! CAUTION: Certificates already exists in output folder.")
		print("!!!!!! Do you want to overwrite (Y/N)?")
		yes = {'yes', 'y'}
		no = {'no', 'n'}
		choice = raw_input().lower()
		if choice in yes:
			print("!!!!!! Deleteing the existing certificate folders")
			os.system("rm -rf {0}".format(cert_out_path))
		else:
			print("!!!!!! Aborting Certificate Generation")
			exit(0)	
	os.system("mkdir -p {0}".format(cert_out_path))

if args.gen_fuse_bin is True and not os.path.exists(fusebin_out_path):
        os.system("mkdir -p {0}".format(fusebin_out_path))

if args.sign_pack_image is True and not os.path.exists(oem_signed_img_path):
        os.system("mkdir -p {0}".format(oem_signed_img_path))

security_profile_file=security_profile_path + "/qcn9224_v2_security_profile.xml"

#====================================================================================================#
#========================1.Generate root certificates================================================#
#====================================================================================================#

if args.gen_cert_chain is True:
	subject_root="\"/C=" + country + "/ST=" + state + "/L=" + location + "/O=" + organization + "/OU=" + organization_unit + "/CN=" + common_name + "\""
	subject_CA="\"/C=" + country + "/ST=" + state + "/L=" + location + "/O=" + organization + "/OU=" + organization_unit + "/CN=" + common_name + " (CA)\""
        gen_root_key="openssl genrsa -out {0} 2048".format(os.path.join(cert_out_path,"qpsa_rootca0.key"))

        gen_root_cert = "openssl req -new -sha256 -key {0} -x509 -out {1} -subj {3} -days 7300 -set_serial 1 -config {2} -sigopt rsa_padding_mode:pss -sigopt rsa_pss_saltlen:-1 -sigopt digest:sha256".format(os.path.join(cert_out_path,"qpsa_rootca0.key"),os.path.join(cert_out_path,"rootca_pem0.crt"), openssl_cfg_path,subject_root)

        gen_root_cert_der= "openssl x509 -in {0} -inform PEM -out {1} -outform DER".format(os.path.join(cert_out_path,"rootca_pem0.crt"),os.path.join(cert_out_path,"qpsa_rootca0.cer"))

	
	gen_root_cert_hash = "openssl dgst -sha256 {0} > {1}".format(os.path.join(cert_out_path,"qpsa_rootca0.cer"),os.path.join(cert_out_path,"pkhash.txt"))
	

	print("Generating Root Certificate")
	os.system(gen_root_key)
	os.system(gen_root_cert)
	os.system(gen_root_cert_der)
	os.system(gen_root_cert_hash)

	if os.path.exists(os.path.join(cert_out_path,"rootca_pem0.crt")):
		print("!!!!!! Root Certificate Generation Success, Generated in " + cert_out_path)
	else:	
		print("!!!!!! Root Certificate Generation Failied")
		exit(0)		
	
#====================================================================================================#
#==========================2. Generate CA certificate================================================#
#====================================================================================================#

if args.gen_cert_chain is True:
        print("Generating Attest CA Certificate")

        gen_attest_ca = "openssl x509 -req -in {0} -CA {1} -CAkey {2} -out {3} -set_serial 5 -days 7300 -extfile {4} -sigopt rsa_padding_mode:pss -sigopt rsa_pss_saltlen:-1 -sigopt digest:sha256".format(os.path.join(cert_out_path,"attestca.csr"),os.path.join(cert_out_path,"rootca_pem0.crt"),os.path.join(cert_out_path,"qpsa_rootca0.key"),os.path.join(cert_out_path,"attestca_pem.crt"),v3_ext_path)

        os.system("openssl genrsa -out {0} 2048".format(os.path.join(cert_out_path,"qpsa_attestca.key")))

        os.system("openssl req -new -key {0} -out {1} -subj {3} -days 7300 -config {2}".format(os.path.join(cert_out_path,"qpsa_attestca.key"),os.path.join(cert_out_path,"attestca.csr"),openssl_cfg_path, subject_CA))

        os.system(gen_attest_ca)

        os.system("openssl x509 -inform PEM -in {0} -outform DER -out {1}".format(os.path.join(cert_out_path,"attestca_pem.crt"), os.path.join(cert_out_path,"qpsa_attestca.cer")))

	if os.path.exists(os.path.join(cert_out_path,"attestca_pem.crt")):
		print("!!!!!! Attest CA Certificate Generation Success, Generated in " + cert_out_path)
	else:	
		print("!!!!!! Attest CA Certificate Generation Failied")
		exit(0)		

#====================================================================================================#
#==========================3. Generate sec.dat binary================================================#
#====================================================================================================#

if args.gen_fuse_bin is True:
        print("Generating Fuse Blower Binary")

        root_cert_pk_hash = os.popen("sha256sum {0}".format(os.path.join(cert_out_path,"qpsa_rootca0.cer"))).read()
        root_cert_pk_hash = root_cert_pk_hash.split(" ")
        print("Root CA PK hash:")
        print(root_cert_pk_hash[0])
#security_profile_file=security_profile_path + "/qcn9224_v2_security_profile.xml"
        sec_dat_gen_cmd= sectools_path + " fuse-blower --security-profile {0} --generate --outfile {1} --fuse-secure-boot0-auth-en --fuse-oem-pk-hash=0x{2}".format(security_profile_file, os.path.join(fusebin_out_path,"sec.dat"),root_cert_pk_hash[0] )
        sec_dat_inspect_cmd= sectools_path + " fuse-blower --security-profile {0} {1} --inspect > {2}".format(security_profile_file, os.path.join(fusebin_out_path,"sec.dat"),os.path.join(fusebin_out_path,"sec_dat_inspect.txt"))

        oem_id_sec_dat = " --fuse-oem-id=" + oem_id

	if oem_product_id != "0x0":
        	oem_id_sec_dat = oem_id_sec_dat + " --fuse-model-id=" + oem_product_id

	sec_dat_gen_cmd=sec_dat_gen_cmd + oem_id_sec_dat

        os.system(sec_dat_gen_cmd)
	os.system(sec_dat_inspect_cmd)
	#concatenate fuse_blower with sec.dat
	os.system("cat {0} {1} > {2}".format(fuse_blower_bin_path,os.path.join(fusebin_out_path,"sec.dat"),os.path.join(fusebin_out_path,"fuse_blower_sec_dat.bin")))

	if os.path.exists(os.path.join(fusebin_out_path,"fuse_blower_sec_dat.bin")):
		print("!!!!!! Fuse Blower Binary Generation Success, Generated in " + fusebin_out_path)
	else:	
		print("!!!!!! Fuse Blower Binary Generation Failied")
		exit(0)		

#====================================================================================================#
#=======================4.OEM sign the images with the generated certificates =======================#
#====================================================================================================#
if args.sign_pack_image is True:

	print("Signing the images with OEM Certificates")
	signing_cmd = sectools_path +  " secure-image {qti_signed_image_path} --outfile {oem_signed_image_path} --security-profile {security_profile} --image-id {image_id} --sign --signing-mode LOCAL --root-certificate {root_cert_path} --ca-certificate {ca_cert_path} --ca-key {ca_key_path}"
	inspect_cmd = sectools_path + " secure-image {oem_signed_image_path} --inspect > {inspect_output_path}"

	wlan_part_1_oem_signed_image_path=os.path.join(oem_signed_img_path,"SBL_RDDM_MERGED_V2_OEM_signed.bin")
	wlan_part_2_oem_signed_image_path=os.path.join(oem_signed_img_path,"CNSS_RAM_V2_OEM_signed.bin")
	iu_oem_signed_image_path=os.path.join(oem_signed_img_path,"phy_ucode_oem_signed.elf")
	amss_oem_signed_image_path=os.path.join(oem_signed_img_path,"amss.bin")
	m3_oem_signed_image_path=os.path.join(oem_signed_img_path,"m3.bin")

	wlan_part_1_oem_signed_inspect_path=os.path.join(oem_signed_img_path,"wlan_part1_inspect.txt")
	wlan_part_2_oem_signed_inspect_path=os.path.join(oem_signed_img_path,"wlan_part2_inspect.txt")
	iu_oem_signed_inspect_path=os.path.join(oem_signed_img_path,"iu_fw_inspect.txt")


	amss_image_id = "AMSS"
	if args.binary is True:
		oem_fw_image_id = "AMSS"
	else:
		oem_fw_image_id = "OEM-WLAN-FW"

	iu_image_id = "IU-FW"

	oem_id_sign_arg = " --oem-id=" + oem_id

	signing_cmd=signing_cmd + oem_id_sign_arg + " --oem-product-id=" + oem_product_id

	fusebin_out_path = os.path.join(args.out_path,"fuseblower_bin")
	fuse_blower_oem_signed_image_path=os.path.join(fusebin_out_path,"fuse_blower_sec_data_oem_signed.bin")
	fuse_blower_oem_signed_inspect_path=os.path.join(fusebin_out_path,"fuse_blower_inspect.txt")

	#Signing fuse blower
	os.system(signing_cmd.format(qti_signed_image_path=os.path.join(fusebin_out_path,"fuse_blower_sec_dat.bin"), oem_signed_image_path=fuse_blower_oem_signed_image_path,security_profile=security_profile_file, image_id= amss_image_id, root_cert_path=os.path.join(cert_chain_path,"qpsa_rootca0.cer"),ca_cert_path=os.path.join(cert_chain_path,"qpsa_attestca.cer"),ca_key_path=os.path.join(cert_chain_path,"qpsa_attestca.key")))


	#Signing WLAN-PART-1 image
	os.system(signing_cmd.format(qti_signed_image_path=os.path.join(qti_signed_images_path,"SBL_RDDM_MERGED_V2_PROD_signed.bin"), oem_signed_image_path=wlan_part_1_oem_signed_image_path,security_profile=security_profile_file, image_id= amss_image_id, root_cert_path=os.path.join(cert_chain_path,"qpsa_rootca0.cer"), ca_cert_path=os.path.join(cert_chain_path,"qpsa_attestca.cer"),ca_key_path=os.path.join(cert_chain_path,"qpsa_attestca.key")))
	#Signing WLAN-PART-2 image
	os.system(signing_cmd.format(qti_signed_image_path=oem_wlan_fw, oem_signed_image_path=wlan_part_2_oem_signed_image_path,security_profile=security_profile_file, image_id= oem_fw_image_id, root_cert_path=os.path.join(cert_chain_path,"qpsa_rootca0.cer"), ca_cert_path=os.path.join(cert_chain_path,"qpsa_attestca.cer"),ca_key_path=os.path.join(cert_chain_path,"qpsa_attestca.key")))

	#Signing IU-FW image
	os.system(signing_cmd.format(qti_signed_image_path=os.path.join(qti_signed_images_path,"phy_ucode_signed.elf"), oem_signed_image_path=iu_oem_signed_image_path,security_profile=security_profile_file, image_id= iu_image_id, root_cert_path=os.path.join(cert_chain_path,"qpsa_rootca0.cer"), ca_cert_path=os.path.join(cert_chain_path,"qpsa_attestca.cer"),ca_key_path=os.path.join(cert_chain_path,"qpsa_attestca.key")))

	#Save the inspect command output
	os.system(inspect_cmd.format(oem_signed_image_path=wlan_part_1_oem_signed_image_path,inspect_output_path=wlan_part_1_oem_signed_inspect_path))
	os.system(inspect_cmd.format(oem_signed_image_path=wlan_part_2_oem_signed_image_path,inspect_output_path=wlan_part_2_oem_signed_inspect_path))
	os.system(inspect_cmd.format(oem_signed_image_path=iu_oem_signed_image_path,inspect_output_path=iu_oem_signed_inspect_path))
	os.system(inspect_cmd.format(oem_signed_image_path=fuse_blower_oem_signed_image_path,inspect_output_path=fuse_blower_oem_signed_inspect_path))

	
	if not os.path.exists(wlan_part_1_oem_signed_image_path):
		print("!!!!!! Error in OEM image signing")
		exit(0)

	if not os.path.exists(wlan_part_2_oem_signed_image_path):
		print("!!!!!! Error in OEM image signing")
		exit(0)

	if not os.path.exists(iu_oem_signed_image_path):
		print("!!!!!! Error in OEM image signing")
		exit(0)

	print("!!!!!! Successfully signed the images with OEM Certificates ")

	# Concatenate the WLAN Part 1 and Part 2 Images to form amss.bin
	os.system("cat {0} {1} > {2}".format(wlan_part_1_oem_signed_image_path, wlan_part_2_oem_signed_image_path, amss_oem_signed_image_path))

	#Strip uCode (remove redundant data section)
	os.system ("dd if={0} of={1} bs=1k skip=0 count=412".format(iu_oem_signed_image_path,m3_oem_signed_image_path))

	#Make Squashfs (Prop)
	if args.binary is True:
		#Unsquashfs the existing file. BIN_FILES wont exist
		os.system("rm -rf {0}".format(os.path.join(oem_wlan_fw_path,"BIN_FILES")))
		os.system("unsquashfs -d {0} {1}".format(os.path.join(oem_wlan_fw_path,"BIN_FILES"), os.path.join(oem_wlan_fw_path,"wifi_fw_qcn9224_v2_squashfs.img")))

	os.system("cp -f {0} {1}".format(amss_oem_signed_image_path, os.path.join(oem_wlan_fw_path,"BIN_FILES/qcn9224/amss.bin"))) 
	os.system("cp -f {0} {1}".format(m3_oem_signed_image_path, os.path.join(oem_wlan_fw_path,"BIN_FILES/qcn9224/m3.bin"))) 	
	os.system("{0} {1} {2} -noappend -root-owned -comp xz -Xpreset 9 -Xe -Xlc 0 -Xlp 2 -Xpb 2 -Xbcj arm -b 256k -processors 1".format(mksquashfs_path, os.path.join(oem_wlan_fw_path,"BIN_FILES"),  os.path.join(oem_wlan_fw_path,"wifi_fw_qcn9224_v2_squashfs.img")))
	
	if not os.path.exists(os.path.join(oem_wlan_fw_path,"wifi_fw_qcn9224_v2_squashfs.img")):
		print("!!!!!! Error in packing wifi_fw_qcn9224_v2_squashfs.img")
		exit(0)

	print("!!!!!! Successfully packed : " +  os.path.join(oem_wlan_fw_path,"wifi_fw_qcn9224_v2_squashfs.img"))

	#Make Squashfs (upstream)
	if args.binary is False:
		os.system("cp -f {0} {1}".format(amss_oem_signed_image_path, os.path.join(oem_wlan_fw_path,"upstream/BIN_FILES/qcn9224/amss.bin"))) 
		os.system("cp -f {0} {1}".format(m3_oem_signed_image_path, os.path.join(oem_wlan_fw_path,"upstream/BIN_FILES/qcn9224/m3.bin"))) 	
		os.system("{0} {1} {2} -noappend -root-owned -comp xz -Xpreset 9 -Xe -Xlc 0 -Xlp 2 -Xpb 2 -Xbcj arm -b 256k -processors 1".format(mksquashfs_path, os.path.join(oem_wlan_fw_path,"upstream/BIN_FILES"),  os.path.join(oem_wlan_fw_path,"upstream/wifi_fw_qcn9224_v2_squashfs.img")))
	

		if not os.path.exists(os.path.join(oem_wlan_fw_path,"upstream/wifi_fw_qcn9224_v2_squashfs.img")):
			print("!!!!!! Error in packing upstream wifi_fw_qcn9224_v2_squashfs.img")
			exit(0)	

	print("!!!!!! Successfully packed upstream : " +  os.path.join(oem_wlan_fw_path,"upstream/wifi_fw_qcn9224_v2_squashfs.img"))

	if args.dual_mac is True:

		if args.binary is True:
			#Unsquashfs the existing file. BIN_FILES wont exist
			os.system("rm -rf {0}".format(os.path.join(oem_wlan_fw_path_single_dual, "BIN_FILES")))
			os.system("unsquashfs -d {0} {1}".format(os.path.join(oem_wlan_fw_path_single_dual, "BIN_FILES"), os.path.join(oem_wlan_fw_path_single_dual,"wifi_fw_qcn9224_v2_dualmac_squashfs.img")))

		# Create a squashfs that has single MAC FW and dual MAC FW (Prop)
		os.system("cp -f {0} {1}".format(os.path.join(oem_wlan_fw_path_single,"BIN_FILES/qcn9224/amss.bin"), os.path.join(oem_wlan_fw_path_single_dual,"BIN_FILES/qcn9224/amss.bin"))) 
		os.system("cp -f {0} {1}".format(os.path.join(oem_wlan_fw_path_single,"BIN_FILES/qcn9224/m3.bin"), os.path.join(oem_wlan_fw_path_single_dual,"BIN_FILES/qcn9224/m3.bin"))) 
		os.system("cp -f {0} {1}".format(os.path.join(oem_wlan_fw_path_dual,"BIN_FILES/qcn9224/amss.bin"), os.path.join(oem_wlan_fw_path_single_dual,"BIN_FILES/qcn9224/amss_dualmac.bin"))) 
		os.system("cp -f {0} {1}".format(os.path.join(oem_wlan_fw_path_dual,"BIN_FILES/qcn9224/m3.bin"), os.path.join(oem_wlan_fw_path_single_dual,"BIN_FILES/qcn9224/m3.bin"))) 
		os.system("{0} {1} {2} -noappend -root-owned -comp xz -Xpreset 9 -Xe -Xlc 0 -Xlp 2 -Xpb 2 -Xbcj arm -b 256k -processors 1".format(mksquashfs_path, os.path.join(oem_wlan_fw_path_single_dual,"BIN_FILES"),  os.path.join(oem_wlan_fw_path_single_dual,"wifi_fw_qcn9224_v2_dualmac_squashfs.img")))


		if not os.path.exists(os.path.join(oem_wlan_fw_path_single_dual,"wifi_fw_qcn9224_v2_dualmac_squashfs.img")):
			print("!!!!!! Error in packing wifi_fw_qcn9224_v2_dualmac_squashfs.img")
			exit(0)
		
		print("!!!!!! Successfully packed : " +  os.path.join(oem_wlan_fw_path_single_dual,"wifi_fw_qcn9224_v2_dualmac_squashfs.img"))

		# Create a squashfs that has single MAC FW and dual MAC FW (upstream)
		if args.binary is False:
			os.system("cp -f {0} {1}".format(os.path.join(oem_wlan_fw_path_single,"upstream/BIN_FILES/qcn9224/amss.bin"), os.path.join(oem_wlan_fw_path_single_dual,"upstream/BIN_FILES/qcn9224/amss.bin"))) 
			os.system("cp -f {0} {1}".format(os.path.join(oem_wlan_fw_path_single,"upstream/BIN_FILES/qcn9224/m3.bin"), os.path.join(oem_wlan_fw_path_single_dual,"upstream/BIN_FILES/qcn9224/m3.bin"))) 
			os.system("cp -f {0} {1}".format(os.path.join(oem_wlan_fw_path_dual,"upstream/BIN_FILES/qcn9224/amss.bin"), os.path.join(oem_wlan_fw_path_single_dual,"upstream/BIN_FILES/qcn9224/amss_dualmac.bin"))) 
			os.system("cp -f {0} {1}".format(os.path.join(oem_wlan_fw_path_dual,"upstream/BIN_FILES/qcn9224/m3.bin"), os.path.join(oem_wlan_fw_path_single_dual,"upstream/BIN_FILES/qcn9224/m3.bin"))) 
			os.system("{0} {1} {2} -noappend -root-owned -comp xz -Xpreset 9 -Xe -Xlc 0 -Xlp 2 -Xpb 2 -Xbcj arm -b 256k -processors 1".format(mksquashfs_path, os.path.join(oem_wlan_fw_path_single_dual,"upstream/BIN_FILES"),  os.path.join(oem_wlan_fw_path_single_dual,"upstream/wifi_fw_qcn9224_v2_dualmac_squashfs.img")))

			if not os.path.exists(os.path.join(oem_wlan_fw_path_single_dual,"upstream/wifi_fw_qcn9224_v2_dualmac_squashfs.img")):
				print("!!!!!! Error in packing upstream wifi_fw_qcn9224_v2_dualmac_squashfs.img")
				exit(0)
		
			print("!!!!!! Successfully packed upstream : " +  os.path.join(oem_wlan_fw_path_single_dual,"upstream/wifi_fw_qcn9224_v2_dualmac_squashfs.img"))
