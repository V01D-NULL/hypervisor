# Virtualization notes

Definitions:
- vmm - Has complete control over the hardware, controls the guest os using virtualization

## Intel:

**LIFE CYCLE OF VMM SOFTWARE**:
* Software enters VMX operation by executing a VMXON instruction.
* Using VM entries, a VMM can then enter guests into virtual machines (one at a time). The VMM effects a
VM entry using instructions VMLAUNCH and VMRESUME; it regains control using VM exits.
* VM exits transfer control to an entry point specified by the VMM. The VMM can take action appropriate to the
cause of the VM exit and can then return to the virtual machine using a VM entry.
* Eventually, the VMM may decide to shut itself down and leave VMX operation. It does so by executing the
VMXOFF instruction.

----
- vmx root operation - This is usually the mode in which the vmm is operating
- vmx non-root operation - This is usually the mode in which the guest is operating
- vmcs:
	- vmcs stands for: Virtual Machine Control data-Structure.
	- It manages manage transitions into and out of vmx non-root (aka vm enter and vm exit)
	- There can be one vmcs per virtual machine, and even one per virtual processor for each virtual machine.