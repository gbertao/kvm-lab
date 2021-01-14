#include <assert.h>
#include <fcntl.h>
#include <linux/kvm.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stddef.h>


struct kvm {
    int sys_fd;
    int vm_fd;
    int api_ver;
    char *mem;
};

struct vcpu {
    int fd;
    struct kvm_run *kvm_run;
};

void kvm_init(struct kvm *kvm) {
    struct kvm_userspace_memory_region mem_reg;
    int ret;

    kvm->sys_fd = open("/dev/kvm", O_RDWR);
    assert(kvm->sys_fd >= 0);

    kvm->api_ver = ioctl(kvm->sys_fd, KVM_GET_API_VERSION, 0);
    assert(kvm->api_ver == 12);

    kvm->vm_fd = ioctl(kvm->sys_fd, KVM_CREATE_VM, (unsigned long)0);
    assert(kvm->vm_fd >= 0);

    kvm->mem = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    assert(kvm->mem != MAP_FAILED);

    mem_reg.slot = 0;
    mem_reg.flags = 0;
    mem_reg.guest_phys_addr = 0x1000;
    mem_reg.memory_size = 0x1000;
    mem_reg.userspace_addr = (unsigned long)kvm->mem;
    ret = ioctl(kvm->vm_fd, KVM_SET_USER_MEMORY_REGION, &mem_reg);
    assert(ret >= 0);
}

int main(void) {
    struct kvm kvm;

    kvm_init(&kvm);


    return 0;
}
