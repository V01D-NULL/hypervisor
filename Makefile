include Makefile.conf

hypervisor: $(ELF) limine
	@$(MAKE) limine/iso --no-print-directory
	@printf "Built hypervisor\n"

$(ELF): $(OBJECT)
	@ld -r -b binary -o font.o src/ports/limine-terminal-port/fonts/vgafont.bin
	$(LD) $(LD_FLAGS) $^ font.o -o $@

%.o: %.asm
	@printf "AS $<\n"
	@$(AS) $(AS_FLAGS) $< -o $@

%.o: %.cpp
	@printf "CPP $<\n"
	@$(CC) $(CC_FLAGS) $< -o $@

%.o: %.c
	@printf "CC $<\n"
	@clang $(C_FLAGS) -Wno-deprecated $< -o $@

limine:
	git clone https://github.com/limine-bootloader/limine.git --branch=v3.0-branch-binary --depth=1 || echo ""
	$(MAKE) -C limine

limine/iso:
	rm -rf iso_root
	mkdir -p iso_root
	cp $(ELF) res/term_bg.bmp \
		src/limine.cfg limine/limine.sys limine/limine-cd.bin limine/limine-cd-efi.bin iso_root/

	xorriso -as mkisofs -b limine-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-cd-efi.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o $(ISO)
	limine/limine-deploy $(ISO)
	rm -rf iso_root

clean:
	rm -rf $(OBJECT) $(ELF) $(ISO)

run:
	$(QEMU) $(QEMUFLAGS) $(ISO)

uefi:
	$(QEMU) -bios /usr/share/ovmf/OVMF.fd $(QEMUFLAGS) $(ISO)

debug:
	$(QEMU) $(QEMUFLAGS_DEBUG) $(ISO)