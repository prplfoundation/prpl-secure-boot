# prplSecureBoot™

The prplSecureBoot™ implements a secure boot process to ensure "trusted execution", anchored to a
root-of-trust, in embedded connected devices that power the Internet of Things. The current version
is targed to Microchip Technology’s PIC32MZ microcontrollers.

We define a secure boot process as one that utilizes a cryptographic public key to verify signed code,
ensuring the authenticity (that it is provided by the vendor/manufacturer) and integrity (that it has
not been modified) of the "main code" (e.g., next-stage bootloader or prplHypervisor™) prior to execution.
This main code must perform the same kind of verification for other applications creating a chain-of-trust.

prplSecureBoot™ and prplHypervisor™ technologies are part of the prplSecurity™ open source 
framework and are released under prpl Foundation permissive license – see http://prplfoundation.org/ip-policy.


## How to build

	make && make test


